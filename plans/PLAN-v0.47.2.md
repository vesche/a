# v0.47.2 -- Codegen Retain/Release

**Goal**: Make the C code generator (`std/compiler/cgen.a`) emit `a_retain`
and `a_release` calls so that every generated function correctly manages the
lifetime of its local variables. This is the core phase that transforms
generated native code from "leak everything" to "balanced refcounting".

**Prerequisite**: v0.47.1 (runtime foundation) must be complete.

---

## Design Principle

**All expressions produce owned values.** The caller of any expression owns
a +1 reference count. The code generator must ensure:

1. Every local variable is zero-initialized (so `a_release` on an unassigned
   local is safe).
2. On assignment, the old value is released, the new value is retained (if
   it comes from another variable; constructors already return owned).
3. On every scope exit (return, implicit return, end-of-function), all
   locals are released.
4. Return values are retained before local cleanup so they survive.

---

## 1. Zero-initialize all locals

### Current codegen (in `emit_fn`, line ~1347)

```
AValue x, y, z;
```

### Target codegen

```
AValue x = {0}, y = {0}, z = {0};
```

A zero-initialized `AValue` has `tag = TAG_INT` (0) and `ival = 0`. This is
benign for `a_release` since `a_release` only acts on TAG_STRING/ARRAY/MAP/
CLOSURE/RESULT. Alternatively we could use `a_void()` but `{0}` is simpler
and avoids function call overhead.

### Change in `cgen.a`

In `emit_fn` (line ~1348), where locals are declared:

```
; BEFORE:
out = str.concat(out, str.concat("\n    AValue ", str.concat(str.join(decl_vars, ", "), ";")))

; AFTER:
let mut init_parts = []
for v in decl_vars { init_parts = push(init_parts, str.concat(v, " = {0}")) }
out = str.concat(out, str.concat("\n    AValue ", str.concat(str.join(init_parts, ", "), ";")))
```

Apply the same pattern in:
- `emit_fn` (top-level functions, line ~1347)
- Lambda block body (line ~589-590)
- `For` loop body locals (line ~1173)
- `ForDestructure` body locals (line ~1200)
- `TryBlock` locals (line ~664)

---

## 2. Release locals at function exit

### Collecting local names

`emit_fn` already collects `decl_vars` (locals excluding params). We also
need to release parameters at function exit. The full set of locals to
release = `decl_vars` + `param_names`.

### Emitting cleanup

Create a helper function `_emit_cleanup(vars)` in `cgen.a` that returns a
string of `a_release(var);` calls:

```
fn _emit_cleanup(vars, depth) -> str {
  let ind = _indent(depth)
  let mut out = ""
  for v in vars {
    out = str.concat(out, str.concat("\n", str.concat(ind, str.concat("a_release(", str.concat(v, ");")))))
  }
  ret out
}
```

### Exit points that need cleanup

Every path that leaves a function body must release locals:

1. **Explicit `return expr;`** (tag == "Return" in `emit_stmt`)
   - Emit: `AValue __ret = <expr>; <cleanup>; return __ret;`
   - The return value expression produces an owned value, so we stash it,
     clean up, then return it.

2. **Implicit return** (last statement is ExprStmt, or fallthrough `return a_void()`)
   - Same pattern: stash result, cleanup, return.

3. **`break` / `continue`** in loops
   - These don't exit the function, they exit the loop scope. Loop-scoped
     variables need release at break/continue, but that's a Phase 3
     optimization. For now, loop vars are function-scoped and will be
     released at function exit.

### Change in `emit_stmt` for Return (line ~1097)

```
; BEFORE:
if tag == "Return" {
  if type_of(node["expr"]) == "void" { ret _R(str.concat(ind, "return a_void();"), li) }
  let vr = emit_expr(node["expr"], bm, ctx, li)
  ret _RL(str.concat(ind, str.concat("return ", str.concat(vr[0], ";"))), vr[1], vr[2])
}

; AFTER:
if tag == "Return" {
  let cleanup = _emit_cleanup(ctx["cleanup_vars"], depth)
  if type_of(node["expr"]) == "void" {
    ret _R(str.concat(ind, str.concat("{ AValue __ret = a_void();", str.concat(cleanup, str.concat("\n", str.concat(ind, "return __ret; }"))))), li)
  }
  let vr = emit_expr(node["expr"], bm, ctx, li)
  ret _RL(str.concat(ind, str.concat("{ AValue __ret = ", str.concat(vr[0], str.concat(";", str.concat(cleanup, str.concat("\n", str.concat(ind, "return __ret; }"))))))), vr[1], vr[2])
}
```

### Change in `emit_fn` for implicit return (line ~1371)

```
; BEFORE:
if !has_return {
  out = str.concat(out, "\n    return a_void();")
}

; AFTER:
let cleanup = _emit_cleanup(all_cleanup_vars, 1)
if !has_return {
  out = str.concat(out, str.concat(cleanup, "\n    return a_void();"))
}
```

### Threading `cleanup_vars` through context

Add `"cleanup_vars"` to the `ctx` map, set at the top of `emit_fn`:

```
let all_cleanup_vars = concat_arr(param_names, decl_vars)
let fn_ctx = map.set(fn_ctx, "cleanup_vars", all_cleanup_vars)
```

Lambda bodies need the same treatment, using their own locals (captures +
params + body locals).

---

## 3. Retain on assignment from another variable

### The problem

When a variable is assigned from another variable or from a collection
access, the value is now shared:

```
let x = some_array[0]   // x gets owned value (from a_array_get + retain)
let y = x               // y gets same AValue; rc is NOT bumped
```

If we later release both `x` and `y`, we double-release.

### Solution: retain on Let/Assign when RHS is an Ident

In `emit_stmt` for `Let` (line ~1057):

```
; BEFORE:
let vr = emit_expr(node["value"], bm, ctx, li)
ret _RL(..., str.concat(_mangle(node["name"]), str.concat(" = ", str.concat(vr[0], ";"))))

; AFTER:
; Check if the value expression is just an Ident (variable reference)
let val = node["value"]
let vr = emit_expr(val, bm, ctx, li)
let assign_expr = vr[0]
if val["tag"] == "Ident" && val["name"] != "true" && val["name"] != "false" {
  assign_expr = str.concat("a_retain(", str.concat(assign_expr, ")"))
}
ret _RL(..., str.concat(_mangle(node["name"]), str.concat(" = ", str.concat(assign_expr, ";"))))
```

**However**, a simpler and more robust approach: wrap ALL assignments in
`a_retain` + release-old pattern. Since constructors and function calls
already return owned values, retaining them bumps rc to 2; but since we
release on scope exit, the net is correct. The overhead is one extra
retain/release pair for non-shared values.

**Better approach**: Since all expressions produce owned values by convention,
we do NOT need extra retain on assignment. The expression already produced
an owned result. We just need to release the old value before overwriting:

```
; For Assign (reassignment of mutable variable):
a_release(x);          // release old value
x = <new_expr>;       // new_expr produces owned value

; For Let (first assignment):
; x was zero-initialized, so a_release(x) on {0} is a no-op.
; We can still emit it for uniformity:
a_release(x);
x = <new_expr>;
```

For Ident expressions (reading a variable), we DO need retain because the
caller expects an owned value:

In `emit_expr` for Ident (line ~305):

```
; BEFORE:
if tag == "Ident" {
  ...
  ret _R(_mangle(name), li)
}

; AFTER:
if tag == "Ident" {
  ...
  ret _R(str.concat("a_retain(", str.concat(_mangle(name), ")")), li)
}
```

**Wait -- this is too aggressive.** If every Ident usage emits `a_retain`,
then `x + y` would retain both `x` and `y` just to pass them to `a_add`,
which doesn't release its arguments. This would leak.

### Refined strategy

The correct approach for Phase 2 (without escape analysis) is:

1. **Assignments (`Let`, `Assign`)**: release the old value, store the new
   value directly. No extra retain -- the RHS expression already produced
   an owned value.

2. **Ident in assignment RHS**: when the RHS of a `Let` or `Assign` is a
   bare `Ident`, the value is being *aliased*, not moved. Emit `a_retain`:
   ```
   a_release(y);
   y = a_retain(x);
   ```

3. **Ident as function argument**: function calls *consume* owned arguments
   (the function may release them or store them). So pass `a_retain(x)` when
   the argument is an Ident. **Actually, by convention, function arguments are
   borrowed** -- the callee does NOT release arguments it receives. So we
   pass variables directly, no retain needed.

4. **Return from function**: the return value must be owned. If the return
   expression is an Ident, emit `a_retain(name)` so the value survives
   local cleanup.

### Final Phase 2 rules

| Context | Old value | New value | Retain? |
|---|---|---|---|
| `let x = <expr>` | release `x` (no-op, was `{0}`) | store result | Only if `<expr>` is Ident |
| `x = <expr>` (reassign) | release `x` | store result | Only if `<expr>` is Ident |
| `return <expr>` | n/a | stash result, cleanup, return | Only if `<expr>` is Ident |
| `fn_call(x)` | n/a | pass `x` directly | No (borrowed) |
| `a_add(x, y)` | n/a | pass directly | No (borrowed) |
| End of function | release all locals + params | n/a | n/a |

**"Only if Ident"** means: if the expression is a bare variable reference,
we need `a_retain` because we're creating a second owner. All other
expressions (calls, constructors, binops) produce fresh owned values.

### Implementation helper

Add `_is_ident(expr_node)` helper:

```
fn _is_ident(node) -> bool {
  ret node["tag"] == "Ident" && node["name"] != "true" && node["name"] != "false"
}
```

---

## 4. Lambda cleanup

Lambdas have their own scope. The lifted lambda function must:

1. Zero-initialize its body locals.
2. Release all locals (captures + params + body locals) at every exit.
3. Retain capture values loaded from `__env` -- actually `a_array_get`
   already returns owned (after Phase 1), so no extra retain needed.

### Capture env array

When creating a closure, the env array stores captured values. Each captured
variable is passed to `a_array_new(N, cap1, cap2, ...)`. Since `a_array_new`
does **not** retain its arguments (it stores them directly), we need to
`a_retain` each capture before passing:

```
; BEFORE:
env_code = a_array_new(2, x, y)

; AFTER:
env_code = a_array_new(2, a_retain(x), a_retain(y))
```

This ensures the closure env holds its own references, independent of the
enclosing scope's cleanup.

---

## 5. For-loop scoping

For loops declare variables inside a C block scope `{ ... }`. Currently these
are not released. After this phase:

```c
{
    AValue __iter_arr = a_iterable(...);
    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
        AValue item = {0};
        item = a_array_get(__iter_arr, a_int(__fi));
        // ... body ...
        a_release(item);   // release loop var at end of iteration
        // release body locals at end of iteration
    }
    a_release(__iter_arr);  // release iterator after loop
}
```

---

## 6. Self-hosting constraint

Since `cgen.a` is the self-hosting compiler and it compiles itself, every
change to the codegen must maintain the three-stage bootstrap invariant.
The approach:

1. Make changes to `cgen.a` incrementally.
2. After each incremental change, generate C₁ via VM, compile to native₁,
   generate C₂ via native₁, compile to native₂, generate C₃ via native₂.
3. Verify C₁ = C₂ = C₃ (fixed point).

The generated C will change (now includes `a_release` calls), but both the
VM-generated and native-generated versions must produce the same C.

---

## Files Modified

- `std/compiler/cgen.a`:
  - `emit_fn`: zero-init locals, add cleanup_vars to ctx, emit cleanup
    at implicit return
  - `emit_stmt` (Let): emit `a_release(old)` before assignment, `a_retain`
    when RHS is Ident
  - `emit_stmt` (Assign): emit `a_release(old)` before assignment, `a_retain`
    when RHS is Ident
  - `emit_stmt` (Return): stash return value, emit cleanup, return
  - `emit_stmt` (For, ForDestructure): release loop vars per iteration,
    release iterator after loop
  - `emit_expr` (Lambda): zero-init body locals, emit cleanup at exits,
    retain captures in env array
  - New helpers: `_emit_cleanup`, `_is_ident`

---

## Acceptance Criteria

- [ ] All pre-declared locals use `= {0}` initialization
- [ ] Every function exit (explicit return, implicit return) releases all locals
- [ ] `let y = x` emits `a_retain(x)` for Ident RHS
- [ ] `x = <expr>` emits `a_release(x)` before reassignment
- [ ] `return x` emits `a_retain(x)` before cleanup when returning an Ident
- [ ] Lambda captures are retained when building env array
- [ ] For-loop iterators and loop variables are released
- [ ] Three-stage bootstrap passes
- [ ] All test programs produce correct output
- [ ] No new memory leaks introduced (verified by code review; Valgrind is Phase 6)
