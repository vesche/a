# v0.43 -- Closures & Higher-Order Functions in Native Compilation

## Motivation

The native compiler (v0.42) handles the core language: functions, control flow, data structures, 45+ builtins. But real "a" programs use closures everywhere -- every `map`, `filter`, `reduce`, `sort_by`, `find` call takes a lambda. Pipes chain them. Without closures, the native compiler can only build imperative programs.

This version closes the gap. After v0.43, programs like `search.a`, `inspect.a`, `lines.a` will be one step closer to compiling natively.

## Design

### Closure Representation in C

Closures in C are a struct pairing a function pointer with a captured environment:

```c
typedef AValue (*AClosureFn)(AValue env, int argc, AValue* argv);

typedef struct AClosure {
    int rc;
    AClosureFn fn;
    AValue env;  // TAG_ARRAY of captured values, or TAG_VOID if none
} AClosure;
```

`AValue` gains a new tag `TAG_CLOSURE` and a `AClosure* cval` union member.

### Lambda Lifting

When the code generator encounters a Lambda expression:

1. **Free variable analysis**: Walk the lambda body, collect Ident references not in the lambda's own params or local bindings. These are captures.
2. **Lift to top-level C function**: Generate `AValue __lambda_N(AValue __env, int __argc, AValue* __argv)` where N is a sequential ID. Captures are loaded from `__env`, params from `__argv`.
3. **Emit closure creation**: At the lambda's original location, emit `a_closure(__lambda_N, a_array_new(K, capture1, capture2, ...))` (or `a_closure(__lambda_N, a_void())` if no captures).

### HOF Builtins

The C runtime gains higher-order function implementations that accept closures:

```c
AValue a_hof_map(AValue arr, AValue closure);     // map(arr, fn)
AValue a_hof_filter(AValue arr, AValue closure);   // filter(arr, fn)
AValue a_hof_reduce(AValue arr, AValue init, AValue closure); // reduce(arr, init, fn)
void   a_hof_each(AValue arr, AValue closure);     // each(arr, fn)
AValue a_hof_sort_by(AValue arr, AValue closure);  // sort_by(arr, fn)
AValue a_hof_find(AValue arr, AValue closure);     // find(arr, fn)
AValue a_hof_any(AValue arr, AValue closure);      // any(arr, fn)
AValue a_hof_all(AValue arr, AValue closure);      // all(arr, fn)
AValue a_hof_flat_map(AValue arr, AValue closure);
AValue a_hof_min_by(AValue arr, AValue closure);
AValue a_hof_max_by(AValue arr, AValue closure);
```

### Closure-Aware Call Dispatch

When a Call target is an Ident that isn't a known function or builtin, emit `a_closure_call(var, argc, arg1, arg2, ...)` instead of `fn_name(args)`. This handles closures stored in variables.

### Pipe Operator

`expr |> fn(args)` desugars to `fn(expr, args)` -- the left-hand side is inserted as the first argument of the right-hand side call.

### Array Utilities

Non-HOF array builtins added to the C runtime:

- `enumerate(arr)` → `[[0, a], [1, b], ...]`
- `zip(a, b)` → `[[a0, b0], [a1, b1], ...]`
- `take(arr, n)` / `drop(arr, n)`
- `unique(arr)` -- deduplicate preserving order
- `chunk(arr, n)` -- split into sub-arrays

## Code Generator Refactoring

The emit functions are refactored to return `[code, lambda_idx, lifted_fns]` instead of plain strings. This threads the lambda counter through emission and accumulates lifted function definitions:

```
emit_expr(node, bm, ctx, li) -> [code_str, next_li, [lifted_fn_strs]]
emit_stmt(node, depth, bm, ctx, li) -> [code_str, next_li, [lifted_fn_strs]]
emit_fn(node, bm, ctx, li) -> [code_str, next_li, [lifted_fn_strs]]
```

## Test Programs

1. `closures_basic.a` -- lambda creation, closure calls, captures
2. `hof.a` -- map, filter, reduce, each, sort_by, find, any, all
3. `pipes.a` -- pipe operator with HOFs
4. `closure_captures.a` -- closures capturing outer variables

## Implementation Order

1. C runtime: TAG_CLOSURE, AClosure, a_closure(), a_closure_call()
2. C runtime: HOF builtins + array utilities
3. Refactor emit functions to return [code, li, lambdas]
4. Lambda lifting + free variable analysis
5. Pipe operator desugaring
6. HOF + array builtins in builtin map
7. Closure-aware call dispatch
8. Test programs
9. Re-verify bootstrap fixed point
