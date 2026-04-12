# v0.45 -- Error Handling, Destructuring & Spread in Native Compilation

## Motivation

The native compiler now handles closures (v0.43) and pattern matching (v0.44). But real "a" programs use `try { }` blocks, the `?` operator, destructuring let/for, and array spread everywhere. Programs like `search.a`, `todo_scan.a`, and `project_stats.a` all use `try { io.read_file(path)? }`. The standard library's `std/meta.a` uses `for [k, v] in node` and spread arrays. Without these features, the most interesting "a" programs can't compile natively.

This version adds four features to close the biggest remaining gaps between VM and native.

## Feature 1: Try/? Error Handling

### AST Nodes

- `Try`: `#{"tag": "Try", "expr": <expr>}` — produced by both `expr?` and `try expr`
- `TryBlock`: `#{"tag": "TryBlock", "block": <block>}` — produced by `try { ... }`

### C Implementation

**Runtime try stack** using `setjmp`/`longjmp`:

```c
#include <setjmp.h>
#define A_TRY_STACK_MAX 64
jmp_buf a_try_stack[A_TRY_STACK_MAX];
AValue a_try_err;
int a_try_depth = 0;
```

**`a_try_unwrap(v)`** — for `expr?`:
- If `v` is `Ok(x)`, return `x`
- If `v` is `Err(e)` and inside a try block, `longjmp` with the error
- If `v` is `Err(e)` and no try block, `a_fail(e)` and exit

**`TryBlock`** emission:
```c
{
    a_try_depth++;
    if (setjmp(a_try_stack[a_try_depth - 1]) == 0) {
        // block body
        a_try_depth--;
        __try_result = a_ok(<last_value>);
    } else {
        a_try_depth--;
        __try_result = a_err(a_try_err);
    }
}
```

**`Try`** emission (for `expr?`):
```c
a_try_unwrap(<expr>)
```

## Feature 2: Let Destructuring

### AST Node

`LetDestructure`: `#{"tag": "LetDestructure", "bindings": [str], "rest": str, "value": <expr>}`

- `bindings`: array of identifier names (or `""` for `_`)
- `rest`: `""` if no rest, otherwise the name after `...`
- `value`: RHS expression

### C Emission

```c
AValue __dest = <value>;
a = a_array_get(__dest, a_int(0));
b = a_array_get(__dest, a_int(1));
// if rest:
rest = a_drop(__dest, a_int(2));
```

## Feature 3: For Destructuring

### AST Node

`ForDestructure`: `#{"tag": "ForDestructure", "bindings": [str], "iter": <expr>, "body": <block>}`

### C Emission

Same loop structure as `For`, but each iteration indexes into the element:

```c
{
    AValue __iter_arr = <iter>;
    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
        AValue k, v, __elem;
        __elem = a_array_get(__iter_arr, a_int(__fi));
        k = a_array_get(__elem, a_int(0));
        v = a_array_get(__elem, a_int(1));
        // body
    }
}
```

## Feature 4: Array Spread

### AST

Array `elems` may contain `Spread` nodes: `#{"tag": "Spread", "expr": <expr>}`.

### C Emission

When any element is a Spread, switch from `a_array_new(N, ...)` to incremental building:

```c
a_array_push(a_concat_arr(a_array_push(a_array_new(0), elem1), spread_arr), elem3)
```

Each normal element: wrap in `a_array_push(acc, elem)`.
Each spread element: wrap in `a_concat_arr(acc, spread_expr)`.

## Implementation Order

1. C runtime: try stack globals + `a_try_unwrap()` + `#include <setjmp.h>`
2. cgen.a: `Try` expression emission (calls `a_try_unwrap`)
3. cgen.a: `TryBlock` expression emission (setjmp/longjmp scaffold)
4. cgen.a: `LetDestructure` statement emission
5. cgen.a: `ForDestructure` statement emission (+ var collection)
6. cgen.a: Array spread detection + incremental build emission
7. Test program: `examples/c_targets/features.a`
8. Re-verify bootstrap fixed point
