# v0.47.4 -- Arena Allocator

**Goal**: Introduce a scoped arena (bump) allocator for short-lived values
that do not escape their scope. Non-escaping allocations in loops and
function bodies go into an arena and are freed in bulk when the scope exits,
avoiding per-object `malloc`/`free` overhead.

**Prerequisite**: v0.47.3 (escape analysis) must be complete.

---

## Design

### What is an arena?

A contiguous block of memory with a bump pointer. Allocations advance the
pointer. Deallocation is a single reset of the pointer to a saved position
(or free of the entire block). No individual frees.

### When to use arenas

Arenas are used for values that **do not escape** their scope:
- Loop temporaries (strings built during iteration, intermediate arrays)
- Function-local temporaries that are only used within the function

The escape analysis from Phase 3 tells us which variables are candidates.

### Arena-allocated values and refcounting

Arena-allocated objects should **not** be refcounted (their rc is a sentinel
value). When `a_release` sees the sentinel, it skips the free. When the
arena is popped, all objects are freed in bulk without consulting rc.

Sentinel value: `rc = -1` (negative rc means "arena-managed, do not free").

---

## 1. Runtime additions (`c_runtime/runtime.h` + `runtime.c`)

### Arena struct

```c
typedef struct AArena {
    char* buf;
    int size;
    int pos;
} AArena;
```

### Arena API

```c
AArena* a_arena_new(int initial_size);
void    a_arena_free(AArena* arena);
void*   a_arena_alloc(AArena* arena, int bytes);
int     a_arena_save(AArena* arena);
void    a_arena_restore(AArena* arena, int saved_pos);
```

- `a_arena_new`: allocates arena with initial buffer (e.g., 4096 bytes).
- `a_arena_free`: frees the arena buffer and struct.
- `a_arena_alloc`: bump-allocates `bytes` from the arena. If the arena is
  full, doubles the buffer (realloc). Returns aligned pointer.
- `a_arena_save`: returns current `pos` (for nested scopes).
- `a_arena_restore`: resets `pos` to saved value, effectively freeing
  everything allocated since the save point.

### Arena-aware constructors

Add variants of allocation functions that take an arena:

```c
AValue a_string_arena(AArena* arena, const char* s);
AValue a_string_len_arena(AArena* arena, const char* s, int len);
AValue a_array_new_arena(AArena* arena, int n, ...);
```

These allocate from the arena and set `rc = -1`.

### Updated `a_release` for sentinel

```c
void a_release(AValue v) {
    if (v.tag == TAG_STRING && v.sval) {
        if (v.sval->rc <= 0) return;  // rc=-1 (arena) or rc=0 (already freed)
        // ... existing logic ...
    }
    // same pattern for ARRAY, MAP, CLOSURE
}
```

The `rc <= 0` guard from Phase 1 already handles this. Arena objects with
`rc = -1` will be skipped by `a_release`, and bulk-freed when the arena
is restored/freed.

### Thread-local arena stack

For simplicity (no threading in v0.47), use a global arena pointer:

```c
extern AArena* a_current_arena;
```

Functions that want to arena-allocate check `a_current_arena != NULL`.

---

## 2. Codegen changes (`std/compiler/cgen.a`)

### Arena for for-loops

For-loops are the primary arena target. The codegen wraps the loop body:

```c
// BEFORE (Phase 2):
{
    AValue __iter_arr = a_iterable(...);
    for (int __fi = 0; ...) {
        AValue item = {0};
        item = a_array_get(__iter_arr, a_int(__fi));
        // ... body ...
        a_release(item);
    }
    a_release(__iter_arr);
}

// AFTER (Phase 4):
{
    AValue __iter_arr = a_iterable(...);
    AArena* __arena = a_arena_new(4096);
    AArena* __prev_arena = a_current_arena;
    a_current_arena = __arena;
    for (int __fi = 0; ...) {
        int __arena_mark = a_arena_save(__arena);
        AValue item = {0};
        item = a_array_get(__iter_arr, a_int(__fi));
        // ... body (non-escaping allocs use arena) ...
        a_arena_restore(__arena, __arena_mark);
    }
    a_current_arena = __prev_arena;
    a_arena_free(__arena);
    a_release(__iter_arr);
}
```

### Conditional arena usage

Only emit arena code if escape analysis determined that the loop body has
non-escaping allocations. If all variables escape, skip the arena overhead.

Check: `if len(non_escaping_loop_vars) > 0 { emit arena wrapper }`.

### Function-level arena

For functions with many non-escaping temporaries (e.g., heavy string
concatenation), a function-level arena can be beneficial. This is optional
for Phase 4 and can be deferred to a later optimization pass.

---

## 3. How arena allocation integrates with retain/release

- **Arena-allocated values** (`rc = -1`): `a_retain` is a no-op (returns
  the value unchanged). `a_release` is a no-op (skipped by guard).
- **Heap-allocated values** (`rc >= 1`): normal retain/release behavior.
- **Mixed containers**: an arena-allocated array might contain heap-allocated
  strings. When the arena is restored, the array struct is reclaimed but the
  strings are NOT released (since we bulk-free without walking). This means
  we must only arena-allocate containers whose **elements** are also
  non-escaping or arena-allocated. This is the transitive escape constraint.

### Conservative approach for Phase 4

Only arena-allocate **leaf values** (strings, integers) in loops. Arrays and
maps remain heap-allocated even if non-escaping, because their elements may
be mixed. This keeps the implementation simple and correct.

---

## 4. `a_retain` sentinel handling

Update `a_retain` to handle arena values:

```c
AValue a_retain(AValue v) {
    if (v.tag == TAG_STRING && v.sval && v.sval->rc > 0) v.sval->rc++;
    else if (v.tag == TAG_ARRAY && v.aval && v.aval->rc > 0) v.aval->rc++;
    else if (v.tag == TAG_MAP && v.mval && v.mval->rc > 0) v.mval->rc++;
    else if (v.tag == TAG_CLOSURE && v.cval && v.cval->rc > 0) v.cval->rc++;
    return v;
}
```

The `rc > 0` check prevents incrementing arena-managed objects' sentinel.

---

## 5. Benchmark targets

After this phase, tight loops like:

```
for i in range(0, 1000000) {
  let s = str.concat("hello", to_str(i))
}
```

should produce significantly less memory pressure, as the intermediate
strings are arena-allocated and bulk-freed each iteration instead of
individually malloced/freed.

---

## Files Modified

- `c_runtime/runtime.h`: add `AArena` struct, arena API declarations,
  `a_current_arena` extern
- `c_runtime/runtime.c`: implement arena functions, update `a_retain` guard
- `std/compiler/cgen.a`:
  - `emit_stmt` (For, ForDestructure): emit arena setup/teardown when
    escape analysis indicates non-escaping variables
  - New helper: `_emit_arena_loop_wrapper`
  - Consult `ctx["escaping_vars"]` to decide arena usage

---

## Acceptance Criteria

- [ ] `AArena` runtime API implemented and compiles
- [ ] Arena-allocated values have `rc = -1`
- [ ] `a_retain`/`a_release` skip arena-managed objects
- [ ] For-loops with non-escaping variables use arena allocation
- [ ] For-loops where all variables escape skip arena (no overhead)
- [ ] Three-stage bootstrap passes
- [ ] All test programs produce correct output
- [ ] Loop-heavy programs use bounded memory (no unbounded growth)
