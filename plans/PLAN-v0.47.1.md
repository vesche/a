# v0.47.1 -- Runtime Foundation

**Goal**: Make the C runtime memory-safe and leak-free *internally*, before
touching the code generator. After this phase every runtime helper owns its
return value (rc = 1), releases its temporaries, and `a_release` is safe to
call on zero-initialized / double-freed values.

---

## 1. Harden `a_release`

### Current problem

```c
void a_release(AValue v) {
    if (v.tag == TAG_STRING && v.sval) {
        if (--v.sval->rc <= 0) free(v.sval);   // use-after-free if called twice
    }
    // ...same pattern for ARRAY, MAP, CLOSURE, RESULT
}
```

If the same `AValue` is released twice, `rc` goes negative and `free` is
called on already-freed memory.  There is also no protection against releasing
a stack-local `AValue` whose union bytes happen to look like a valid pointer.

### Changes to `c_runtime/runtime.c`

1. **Null-after-free**: after `free`, set the pointer to `NULL` (callers may
   still hold a copy of the struct, but the NULL check at the top of each
   branch prevents double-free for the canonical case).

2. **Guard `rc > 0` before decrement**: skip the release entirely if `rc` is
   already ≤ 0 (defensive against misuse during bring-up).

3. **Recursive children first**: ensure children are released before the
   parent struct is freed (already the case, just verify).

Target implementation:

```c
void a_release(AValue v) {
    if (v.tag == TAG_STRING && v.sval) {
        if (v.sval->rc <= 0) return;
        if (--v.sval->rc == 0) { free(v.sval); }
    } else if (v.tag == TAG_ARRAY && v.aval) {
        if (v.aval->rc <= 0) return;
        if (--v.aval->rc == 0) {
            for (int i = 0; i < v.aval->len; i++) a_release(v.aval->items[i]);
            free(v.aval->items);
            free(v.aval);
        }
    } else if (v.tag == TAG_CLOSURE && v.cval) {
        if (v.cval->rc <= 0) return;
        if (--v.cval->rc == 0) {
            a_release(v.cval->env);
            free(v.cval);
        }
    } else if (v.tag == TAG_MAP && v.mval) {
        if (v.mval->rc <= 0) return;
        if (--v.mval->rc == 0) {
            for (int i = 0; i < v.mval->len; i++) {
                free(v.mval->keys[i]);
                a_release(v.mval->vals[i]);
            }
            free(v.mval->keys);
            free(v.mval->vals);
            free(v.mval);
        }
    } else if (v.tag == TAG_RESULT && v.rval.inner) {
        a_release(*v.rval.inner);
        free(v.rval.inner);
    }
}
```

Key difference from current: `<= 0` early return prevents double-free, and
`== 0` (not `<= 0`) is the free trigger.

---

## 2. Fix helper leaks

Several runtime helpers allocate intermediate `AValue`s (via `a_to_str`) but
never release them.

### `a_str_concat`

```c
// BEFORE: leaks sa and sb
AValue a_str_concat(AValue a, AValue b) {
    AValue sa = a_to_str(a);
    AValue sb = a_to_str(b);
    // ... build ns ...
    return (AValue){.tag = TAG_STRING, .sval = ns};
}
```

```c
// AFTER: release temporaries
AValue a_str_concat(AValue a, AValue b) {
    AValue sa = a_to_str(a);
    AValue sb = a_to_str(b);
    int newlen = sa.sval->len + sb.sval->len;
    AString* ns = malloc(sizeof(AString) + newlen + 1);
    ns->rc = 1;
    ns->len = newlen;
    memcpy(ns->data, sa.sval->data, sa.sval->len);
    memcpy(ns->data + sa.sval->len, sb.sval->data, sb.sval->len);
    ns->data[newlen] = '\0';
    a_release(sa);
    a_release(sb);
    return (AValue){.tag = TAG_STRING, .sval = ns};
}
```

### `a_concat_n`

```c
// AFTER: release each part after copying
AValue a_concat_n(int n, ...) {
    va_list ap;
    va_start(ap, n);
    AValue parts[64];
    int total_len = 0;
    for (int i = 0; i < n && i < 64; i++) {
        parts[i] = a_to_str(va_arg(ap, AValue));
        total_len += parts[i].sval->len;
    }
    va_end(ap);
    AString* ns = malloc(sizeof(AString) + total_len + 1);
    ns->rc = 1; ns->len = total_len;
    int pos = 0;
    for (int i = 0; i < n && i < 64; i++) {
        memcpy(ns->data + pos, parts[i].sval->data, parts[i].sval->len);
        pos += parts[i].sval->len;
    }
    ns->data[total_len] = '\0';
    for (int i = 0; i < n && i < 64; i++) a_release(parts[i]);
    return (AValue){.tag = TAG_STRING, .sval = ns};
}
```

### `a_str_join`

Currently calls `a_to_str` twice per element (once for length, once for copy).
Fix: single pass that stores the converted strings, then copy + release.

```c
AValue a_str_join(AValue arr, AValue sep) {
    if (arr.tag != TAG_ARRAY || sep.tag != TAG_STRING) return a_string("");
    int count = arr.aval->len;
    AValue* parts = malloc(sizeof(AValue) * (count > 0 ? count : 1));
    int total = 0;
    for (int i = 0; i < count; i++) {
        parts[i] = a_to_str(arr.aval->items[i]);
        total += parts[i].sval->len;
        if (i > 0) total += sep.sval->len;
    }
    AString* ns = malloc(sizeof(AString) + total + 1);
    ns->rc = 1; ns->len = total;
    int pos = 0;
    for (int i = 0; i < count; i++) {
        if (i > 0) { memcpy(ns->data + pos, sep.sval->data, sep.sval->len); pos += sep.sval->len; }
        memcpy(ns->data + pos, parts[i].sval->data, parts[i].sval->len); pos += parts[i].sval->len;
    }
    ns->data[total] = '\0';
    for (int i = 0; i < count; i++) a_release(parts[i]);
    free(parts);
    return (AValue){.tag = TAG_STRING, .sval = ns};
}
```

### Other helpers to audit

Scan for every `a_to_str` call that stores the result in a local and does not
release it. At minimum:

| Function | Leaks | Fix |
|---|---|---|
| `a_str_concat` | `sa`, `sb` | release after memcpy |
| `a_concat_n` | `parts[0..n]` | release after memcpy |
| `a_str_join` | two `a_to_str` per element | single-pass + release |
| `val_to_buf` (recursive) | none (writes to stack buffer) | OK |
| `a_to_str` | none (returns owned) | OK |
| `a_println` / `a_eprintln` | possible via `print_val` | audit; `print_val` uses stack buf |

---

## 3. Owned-return convention for collection access

### Current problem

`a_array_get` and `a_map_get` return **borrowed** references: they hand back
the raw `AValue` stored inside the collection without retaining it. If the
collection is released (or mutated) before the caller uses the returned value,
it becomes a dangling pointer.

### Changes

Add `a_retain` to the return path:

```c
AValue a_array_get(AValue arr, AValue idx) {
    if (arr.tag == TAG_MAP && idx.tag == TAG_STRING) {
        return a_map_get(arr, idx);
    }
    if (idx.tag != TAG_INT) return a_void();
    int i = (int)idx.ival;
    if (arr.tag == TAG_STRING) {
        if (i < 0 || i >= arr.sval->len) return a_void();
        return a_string_len(arr.sval->data + i, 1);  // already owned (new alloc)
    }
    if (arr.tag != TAG_ARRAY) return a_void();
    if (i < 0 || i >= arr.aval->len) return a_void();
    return a_retain(arr.aval->items[i]);  // <-- retain before returning
}

AValue a_map_get(AValue m, AValue key) {
    if (m.tag != TAG_MAP) return a_void();
    int idx = map_find(m.mval, val_as_key(key));
    return idx >= 0 ? a_retain(m.mval->vals[idx]) : a_void();  // <-- retain
}
```

Similarly audit `a_unwrap`:

```c
AValue a_unwrap(AValue v) {
    if (v.tag == TAG_RESULT && v.rval.inner)
        return a_retain(*v.rval.inner);  // <-- retain
    return v;
}
```

### Impact on existing generated code

Since `cgen.a` currently emits **zero** `a_release` calls, adding `a_retain`
to getters will increase reference counts but nothing will decrement them.
This is *still* a leak, but no worse than before. Once Phase 2 (codegen
retain/release) lands, the counts will balance.

---

## 4. Audit other allocating helpers

Review every function in `runtime.c` that returns `TAG_STRING`, `TAG_ARRAY`,
`TAG_MAP`, or `TAG_CLOSURE` and confirm it returns owned (rc = 1 for new
allocations, or `a_retain` for borrowed data). Functions that create new
collections (like `a_map_set`, `a_map_delete`, `a_map_entries`) already
allocate with rc = 1, so they are fine. Key concern: any function that
copies an `AValue` from a source collection into a new collection must
`a_retain` the element.

Specific spots to check:

- `a_map_entries` line 677: `arr->items[i] = a_array_new(2, ...)` -- inner
  values are passed to `a_array_new` which stores them; the map's values
  are now shared. Need `a_retain` on each map value being wrapped.
- `a_map_from_entries`: values extracted from entries and passed to
  `a_map_set` -- `a_map_set` should retain values it stores.
- `a_sort`, `a_reverse_arr`, `a_concat_arr`, `a_drop`, `a_array_slice`:
  all copy `AValue`s from source to new array. Each copied element needs
  `a_retain`.

---

## 5. Bootstrap verification

After all runtime changes, run the three-stage bootstrap to confirm the
compiler still produces identical C output:

```
cgen.a  →(VM)→  C₁  →(gcc)→  native₁
cgen.a  →(native₁)→  C₂  →(gcc)→  native₂
cgen.a  →(native₂)→  C₃
diff C₁ C₂ C₃  # must be identical
```

Since this phase only changes `runtime.c` / `runtime.h` (not `cgen.a`), the
generated C should be byte-identical. The native binaries just link against
the updated runtime.

Run existing test programs (`examples/c_targets/*.a`) through native compile
and verify they still produce correct output.

---

## Acceptance Criteria

- [ ] `a_release` is safe on zero-initialized AValues and double-release
- [ ] `a_str_concat`, `a_concat_n`, `a_str_join` release intermediates
- [ ] `a_array_get`, `a_map_get`, `a_unwrap` return retained values
- [ ] Collection-copying helpers retain elements they copy
- [ ] Three-stage bootstrap passes
- [ ] All existing native test programs still pass
