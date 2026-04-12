# v0.47.5 -- Mark-and-Sweep GC

**Goal**: Implement a mark-and-sweep garbage collector to reclaim memory held
by cyclic data structures, particularly closures that capture references to
each other. Reference counting alone cannot collect cycles.

**Prerequisite**: v0.47.2 (codegen retain/release) must be complete. v0.47.3
and v0.47.4 are parallel and not required, but the GC should be compatible
with arena allocation.

---

## Why GC?

Closures in the "a" language can form cycles:

```
fn make_pair() {
  let mut a = fn() => void
  let mut b = fn() => void
  a = fn() { b() }    // a captures b
  b = fn() { a() }    // b captures a
  ret [a, b]
}
```

After `make_pair` returns, `a`'s closure env holds `b`, and `b`'s closure env
holds `a`. Their reference counts are both > 0, but they are unreachable from
any root. Reference counting will never reclaim them.

Mark-and-sweep walks from known roots, marks all reachable objects, then
sweeps (frees) everything unmarked.

---

## 1. Allocation tracking list

### Design

Every heap-allocated object (AString, AArray, AMap, AClosure) is registered
in a global linked list so the GC can iterate over all live objects.

Add a common header to all heap types:

```c
typedef struct AHeapHeader {
    struct AHeapHeader* gc_next;
    uint8_t gc_mark;
} AHeapHeader;
```

Prepend this to each heap struct:

```c
typedef struct AString {
    AHeapHeader gc;    // NEW
    int rc;
    int len;
    char data[];
} AString;
```

Similarly for `AArray`, `AMap`, `AClosure`.

### Global allocation list

```c
static AHeapHeader* gc_alloc_list = NULL;
static int gc_alloc_count = 0;
static int gc_threshold = 1024;  // trigger GC after this many allocations
```

Every `malloc` for a heap object inserts into the list:

```c
static void gc_register(AHeapHeader* obj) {
    obj->gc_next = gc_alloc_list;
    obj->gc_mark = 0;
    gc_alloc_list = obj;
    gc_alloc_count++;
}
```

### Hooking into constructors

`a_string`, `a_string_len`, `a_array_new`, `a_map_new`, `a_closure`:
after `malloc`, call `gc_register(&obj->gc)`.

---

## 2. Root shadow stack

The GC needs to know which objects are reachable. Roots are:
- Global variables (none in current "a" language)
- Local variables on the C call stack
- Function arguments

Since we cannot portably scan the C stack, we use a **shadow stack**: an
explicit array of `AValue*` pointers that the generated code pushes/pops.

```c
#define GC_ROOT_STACK_MAX 4096
static AValue* gc_root_stack[GC_ROOT_STACK_MAX];
static int gc_root_sp = 0;

static void gc_push_root(AValue* root) {
    if (gc_root_sp < GC_ROOT_STACK_MAX)
        gc_root_stack[gc_root_sp++] = root;
}

static void gc_pop_roots(int n) {
    gc_root_sp -= n;
    if (gc_root_sp < 0) gc_root_sp = 0;
}
```

### Public API

```c
void a_gc_push_root(AValue* root);
void a_gc_pop_roots(int n);
void a_gc_collect(void);
```

---

## 3. Mark phase

Walk from all roots, recursively marking reachable objects:

```c
static void gc_mark_value(AValue v) {
    AHeapHeader* h = NULL;
    if (v.tag == TAG_STRING && v.sval) h = &v.sval->gc;
    else if (v.tag == TAG_ARRAY && v.aval) h = &v.aval->gc;
    else if (v.tag == TAG_MAP && v.mval) h = &v.mval->gc;
    else if (v.tag == TAG_CLOSURE && v.cval) h = &v.cval->gc;
    else return;

    if (!h || h->gc_mark) return;  // already marked or non-heap
    h->gc_mark = 1;

    // Recurse into children
    if (v.tag == TAG_ARRAY) {
        for (int i = 0; i < v.aval->len; i++)
            gc_mark_value(v.aval->items[i]);
    } else if (v.tag == TAG_MAP) {
        for (int i = 0; i < v.mval->len; i++)
            gc_mark_value(v.mval->vals[i]);
    } else if (v.tag == TAG_CLOSURE) {
        gc_mark_value(v.cval->env);
    } else if (v.tag == TAG_RESULT && v.rval.inner) {
        gc_mark_value(*v.rval.inner);
    }
}

static void gc_mark_roots(void) {
    for (int i = 0; i < gc_root_sp; i++) {
        gc_mark_value(*gc_root_stack[i]);
    }
}
```

---

## 4. Sweep phase

Walk the allocation list, free unmarked objects, reset marks on survivors:

```c
static void gc_sweep(void) {
    AHeapHeader** p = &gc_alloc_list;
    while (*p) {
        if (!(*p)->gc_mark) {
            AHeapHeader* dead = *p;
            *p = dead->gc_next;
            gc_alloc_count--;
            // Free based on type -- need a type tag in the header
            gc_free_object(dead);
        } else {
            (*p)->gc_mark = 0;  // reset for next cycle
            p = &(*p)->gc_next;
        }
    }
}
```

### Type discrimination for sweep

The `AHeapHeader` needs a type tag to know how to free:

```c
typedef enum { GC_STRING, GC_ARRAY, GC_MAP, GC_CLOSURE } GCType;

typedef struct AHeapHeader {
    struct AHeapHeader* gc_next;
    GCType gc_type;
    uint8_t gc_mark;
} AHeapHeader;
```

```c
static void gc_free_object(AHeapHeader* obj) {
    switch (obj->gc_type) {
        case GC_STRING: free(obj); break;
        case GC_ARRAY: {
            AArray* a = (AArray*)((char*)obj - offsetof(AArray, gc));
            free(a->items);
            free(a);
            break;
        }
        case GC_MAP: {
            AMap* m = (AMap*)((char*)obj - offsetof(AMap, gc));
            for (int i = 0; i < m->len; i++) free(m->keys[i]);
            free(m->keys);
            free(m->vals);
            free(m);
            break;
        }
        case GC_CLOSURE: free(obj); break;
    }
}
```

**Alternative (simpler)**: put the `AHeapHeader` as the first field of each
struct, so `(AString*)obj` is valid when `gc_type == GC_STRING`. This avoids
`offsetof` gymnastics:

```c
typedef struct AString {
    AHeapHeader gc;   // MUST be first field
    int rc;
    int len;
    char data[];
} AString;
```

Then `gc_free_object` simply casts to the appropriate type.

---

## 5. GC trigger

GC runs when `gc_alloc_count` exceeds `gc_threshold`. After each collection,
if more than half the objects survived, double the threshold.

```c
void a_gc_collect(void) {
    gc_mark_roots();
    gc_sweep();
    if (gc_alloc_count > gc_threshold / 2)
        gc_threshold *= 2;
}

static void gc_maybe_collect(void) {
    if (gc_alloc_count >= gc_threshold)
        a_gc_collect();
}
```

Call `gc_maybe_collect()` from allocation functions (after `gc_register`).

---

## 6. Codegen changes (`std/compiler/cgen.a`)

### Push/pop GC roots

At the start of each function, push all locals (including params) as GC
roots. At function exit (before cleanup), pop them.

```c
AValue fn_foo(AValue x, AValue y) {
    AValue z = {0}, w = {0};
    a_gc_push_root(&x);
    a_gc_push_root(&y);
    a_gc_push_root(&z);
    a_gc_push_root(&w);
    // ... body ...
    a_gc_pop_roots(4);
    return __ret;
}
```

The push/pop pairs must be emitted by `emit_fn` and lambda emission.

### Interaction with retain/release

GC and refcounting coexist:

- **Refcounting** handles the common case (non-cyclic, immediate reclaim).
- **GC** handles the rare case (cycles, typically closures).

When `a_release` drops rc to 0, it frees immediately (as before). GC only
reclaims objects whose rc is > 0 but which are unreachable (part of a cycle).

**Important**: GC sweep must set `rc = 0` before freeing, or use a separate
free path that doesn't consult rc.

### Arena interaction

Arena-allocated objects (`rc = -1`) are NOT registered with the GC. They are
not in `gc_alloc_list` and are not scanned/freed by the GC. Arena lifetime
is fully scope-based.

---

## 7. Phased rollout

The GC can be implemented incrementally:

1. **Phase 5a**: Add `AHeapHeader` to all heap types, register in alloc list.
   No marking/sweeping yet.  Verify bootstrap still works with the struct
   layout change.

2. **Phase 5b**: Implement mark + sweep in runtime. Add `a_gc_collect` API.
   No codegen changes yet -- GC can be triggered manually from test programs.

3. **Phase 5c**: Add GC root push/pop to codegen. Enable automatic GC
   triggering.

---

## Files Modified

- `c_runtime/runtime.h`:
  - Add `AHeapHeader`, `GCType` enum
  - Restructure `AString`, `AArray`, `AMap`, `AClosure` to include
    `AHeapHeader` as first field
  - Declare `a_gc_push_root`, `a_gc_pop_roots`, `a_gc_collect`
- `c_runtime/runtime.c`:
  - Implement `gc_register`, `gc_mark_value`, `gc_mark_roots`, `gc_sweep`,
    `gc_free_object`, `gc_maybe_collect`, `a_gc_collect`
  - Update all constructors to call `gc_register`
  - Update `a_release` to handle gc header
- `std/compiler/cgen.a`:
  - `emit_fn`: emit `a_gc_push_root` for all locals + params at function
    entry, `a_gc_pop_roots` at every exit
  - Lambda emission: same pattern

---

## Acceptance Criteria

- [ ] `AHeapHeader` added to all heap types without breaking existing code
- [ ] All heap allocations registered in `gc_alloc_list`
- [ ] `gc_mark_roots` + `gc_sweep` correctly collect unreachable cycles
- [ ] Reachable objects are never collected
- [ ] GC trigger fires automatically at allocation threshold
- [ ] Generated code pushes/pops GC roots at function boundaries
- [ ] Arena objects are excluded from GC tracking
- [ ] Three-stage bootstrap passes
- [ ] Closure cycle test program shows memory reclaimed
- [ ] All existing test programs produce correct output
