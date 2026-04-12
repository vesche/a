# v0.47.3 -- Escape Analysis

**Goal**: Classify every local variable in a function as **escaping** or
**non-escaping** so that later phases can optimize memory management. A
non-escaping variable's value never outlives its scope, making it a candidate
for arena allocation (Phase 4) or elided retain/release pairs.

**Prerequisite**: v0.47.2 (codegen retain/release) must be complete.

---

## What is escape analysis?

A variable **escapes** its scope if its value is observable after the scope
exits. Concretely, a variable `x` escapes if any of these hold:

1. `x` appears in a `return` expression (value flows to caller).
2. `x` is captured by a lambda/closure (value flows to closure env).
3. `x` is stored into a collection that itself escapes (transitive escape).
4. `x` is passed to a function that stores it (conservative: any non-builtin
   call with `x` as argument is treated as potentially escaping).

A variable is **non-escaping** if none of the above apply. Non-escaping
variables are only used within their declaring scope for computation
(passed to builtins, used in binops, indexed, etc.) and their values
can be reclaimed as soon as the scope exits.

---

## 1. Analysis pass in `cgen.a`

Add a new function `_escape_analysis(body_stmts, params, captures)` that
returns a set (array) of variable names that escape.

### Algorithm

```
fn _escape_analysis(stmts, params) -> [str] {
  let mut escaping = []

  ; Pass 1: find directly escaping variables
  _walk_stmts_for_escapes(stmts, escaping)

  ; Pass 2: transitive closure
  ; If x escapes and y = x, then y escapes too.
  ; (For Phase 3, we keep this conservative and simple.)

  ret escaping
}
```

### `_walk_stmts_for_escapes`

Recursive walk over all statements and expressions:

- **Return(expr)**: every Ident in `expr` escapes. For compound expressions
  (e.g., `return [x, y]`), walk the expression tree and mark all Idents.
- **Lambda body**: every Ident in the capture set of the lambda escapes from
  the enclosing scope. Use the existing `_compute_captures` function.
- **Call(func, args)**: if `func` is a non-builtin (user function), then
  conservatively mark all Ident arguments as escaping. Builtins that are
  known not to store their arguments (like `a_add`, `a_println`, `a_len`,
  `a_truthy`, etc.) are safe.
- **Assign to collection**: `arr[i] = x` means `x` escapes if `arr` escapes.
  Conservative: mark `x` as escaping.

### Builtin safety classification

Classify builtins into two categories:

**Safe (non-storing)**: these builtins examine or transform their arguments
but do not store references to them:
- All arithmetic: `a_add`, `a_sub`, `a_mul`, `a_div`, `a_mod`, `a_neg`
- All comparison: `a_eq`, `a_neq`, `a_lt`, `a_gt`, `a_lteq`, `a_gteq`
- All logic: `a_not`, `a_and`, `a_or`
- String queries: `a_len`, `a_to_str`, `a_str_contains`, `a_str_find`
- I/O: `a_println`, `a_eprintln`
- Type: `a_type_of`, `a_truthy`

**Storing (potentially escaping)**: these builtins create new containers
that may hold references to arguments:
- `a_array_new`, `a_array_push`, `a_map_set`, `a_concat_arr`
- `a_closure` (stores env)

For Phase 3, the classification is static and hard-coded as an array in
`cgen.a`.

---

## 2. Annotating variables in context

After `_escape_analysis` runs at the top of `emit_fn`, store the result in
the context map:

```
let escaping = _escape_analysis(body["stmts"], params)
let fn_ctx = map.set(fn_ctx, "escaping_vars", escaping)
```

This is consumed by Phase 4 (arena allocation) to decide which variables
get arena-allocated values vs. heap-allocated values.

---

## 3. Optimized retain/release elision

With escape information, we can optimize the retain/release patterns from
Phase 2:

### Non-escaping variable + Ident RHS

If `y` does not escape and `x` does not escape:
```
let y = x
```
Currently emits `y = a_retain(x);`. Since neither escapes, we could skip
the retain and instead just "move" the ownership. But this is only safe if
`x` is not used after the assignment. This is a more advanced optimization
(liveness analysis) that we defer.

**For Phase 3**: the primary output is the escape classification. The actual
optimization will be:

1. **Non-escaping locals do not need retain on read.** If `x` is
   non-escaping and used as a function argument (borrowed), no retain.
2. **Non-escaping locals that are only assigned once** can potentially be
   arena-allocated (Phase 4).

### Cleanup ordering optimization

Variables that don't escape can have their `a_release` calls elided at
function exit if we can prove they hold non-heap values (ints, floats,
bools). This is a type-based optimization we defer to Phase 4.

---

## 4. For-loop escape analysis

For-loop variables are interesting: the loop variable `item` is reassigned
each iteration, so its value from the previous iteration is dead. If `item`
does not escape the loop body:

- We can release it at the end of each iteration (already done in Phase 2).
- In Phase 4, we can arena-allocate the loop variable.

The escape analysis should run on the loop body as a sub-scope, classifying
the loop variable and body locals independently from the outer function.

---

## 5. Lambda escape analysis

Lambda bodies are analyzed separately (they're lifted functions). The
interesting part is the *enclosing scope*: which variables are captured?

The existing `_compute_captures` function already identifies captured
variables. For escape analysis:

- A variable that is captured by a lambda **escapes** from the enclosing
  scope (it will live inside the closure env).
- Inside the lambda body, the captured variables are loaded from the env
  and treated as local (non-escaping unless they escape from the lambda body).

---

## Files Modified

- `std/compiler/cgen.a`:
  - New function: `_escape_analysis(stmts, params)` -> `[str]`
  - New function: `_walk_expr_for_escapes(expr, escaping_set)` (recursive)
  - New function: `_walk_stmts_for_escapes(stmts, escaping_set)`
  - New constant: `_non_storing_builtins` (array of safe builtin names)
  - `emit_fn`: call `_escape_analysis` and store result in context
  - Lambda emission: call `_escape_analysis` for lambda body

---

## Acceptance Criteria

- [ ] `_escape_analysis` correctly identifies returned variables as escaping
- [ ] Variables captured by lambdas are marked as escaping
- [ ] Variables only used in builtins/binops are marked as non-escaping
- [ ] Escape info is stored in context for later phases
- [ ] No behavioral changes to generated code (analysis only, no codegen diff)
- [ ] Three-stage bootstrap passes (C output unchanged)
- [ ] Unit-testable: running escape analysis on known ASTs produces expected
      classification
