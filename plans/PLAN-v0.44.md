# v0.44 -- Pattern Matching in Native Compilation

## Motivation

The native compiler (v0.43) handles closures, HOFs, pipes, and the full imperative/functional core. But "a" programs use `match` for structured dispatch -- Result handling (`Ok(v)` / `Err(e)`), tagged-map dispatch, array destructuring, and literal switching. Without native match support, programs like `examples/resilient.a` can't compile, and the path to compiling the bytecode compiler itself stays blocked.

## AST Shapes

The parser produces these nodes (from `std/compiler/ast.a`):

### Match statement
```
{"tag": "Match", "expr": <scrutinee>, "arms": [<MatchArm>, ...]}
```

### Match arm
```
{"tag": "MatchArm", "pattern": <Pat*>, "guard": <expr or Void>, "body": <Block>}
```

### Pattern nodes

| Tag | Fields | Example |
|-----|--------|---------|
| `PatWildcard` | (none) | `_` |
| `PatIdent` | `name: str` | `x` |
| `PatLiteral` | `value: <Int\|Float\|String\|Bool>` | `42`, `"hello"`, `true` |
| `PatConstructor` | `name: str, args: [Pat*]` | `Ok(v)`, `Err(e)`, `Some(x)` |
| `PatArray` | `elems: [PatElem\|PatRest]` | `[a, b, c]`, `[head, ...tail]` |
| `PatElem` | `pattern: Pat*` | wrapper for one position |
| `PatRest` | `name: str` | `...rest` |
| `PatMap` | `entries: [PatMapEntry]` | `#{"tag": t, "value": v}` |
| `PatMapEntry` | `key: str, pattern: Pat*` | one key-value pair |

## C Emission Strategy

Each `match` statement compiles to a chain of `if`/`else if` blocks. A temporary variable holds the scrutinee. Each arm tests its pattern, binds variables on success, checks the guard (if any), then runs the body.

### Scaffold

```c
{
    AValue __match = <scrutinee>;
    if (<pat0 condition>) {
        <pat0 bindings>
        if (<guard0, or true>) {
            <body0>
        }
    }
    else if (<pat1 condition>) { ... }
    ...
}
```

### Pattern compilation

**PatWildcard / PatIdent (no guard):** Always matches. PatIdent binds: `var = __match;`. This is the catch-all default arm.

**PatLiteral:** Compare with `a_eq`:
```c
if (a_truthy(a_eq(__match, a_int(42)))) { ... }
if (a_truthy(a_eq(__match, a_string("hello")))) { ... }
```

**PatConstructor:** For `Ok(v)` and `Err(e)`, use the Result tag:
```c
if (__match.tag == TAG_RESULT && __match.rval.is_ok) {
    AValue v = *__match.rval.inner;
    ...
}
if (__match.tag == TAG_RESULT && !__match.rval.is_ok) {
    AValue e = *__match.rval.inner;
    ...
}
```

For generic constructors (future-proofing): tag string comparison + field extraction. For now, only `Ok`/`Err` are used in practice.

**PatArray:** Length and element checks:
```c
// [a, b, c] -- exact length
if (__match.tag == TAG_ARRAY && __match.aval->len == 3) {
    AValue a = __match.aval->items[0];
    AValue b = __match.aval->items[1];
    AValue c = __match.aval->items[2];
    ...
}

// [head, ...tail] -- at least 1
if (__match.tag == TAG_ARRAY && __match.aval->len >= 1) {
    AValue head = __match.aval->items[0];
    AValue tail = a_drop(__match, a_int(1));
    ...
}
```

Nested patterns within array elements use recursive condition generation.

**PatMap:** Key existence + value extraction:
```c
if (__match.tag == TAG_MAP
    && a_truthy(a_map_has(__match, a_string("tag")))
    && a_truthy(a_map_has(__match, a_string("value")))) {
    AValue t = a_map_get(__match, a_string("tag"));
    AValue v = a_map_get(__match, a_string("value"));
    // nested pattern checks on t and v if needed
    ...
}
```

**Guard clauses:** After the pattern matches and variables are bound, add a secondary condition:
```c
if (<pattern_condition>) {
    <bindings>
    if (a_truthy(<guard_expr>)) {
        <body>
    }
}
```

Guards with `PatIdent`/`PatWildcard` still need the outer `if` to scope the bindings; the guard becomes the only condition.

### Nested patterns

For nested structural patterns (e.g., `[Ok(x), Err(e)]` or `#{"data": [first, ...rest]}`), the condition is composed recursively. Each sub-pattern adds its conditions to a conjunction, and its bindings after the combined condition succeeds.

**Strategy:** Generate the pattern match as a helper `_emit_pat_check(pat, target_var, li)` → `[cond_code, bindings_code, li, lifted]` where:
- `cond_code` is a C boolean expression (may be `1` for wildcard/ident)
- `bindings_code` is statements that declare/assign pattern variables
- Nested patterns concatenate conditions with `&&`

## C Runtime Additions

```c
// Raw boolean checks (return C int, not AValue) for pattern match conditions
static inline int a_is_ok_raw(AValue v) { return v.tag == TAG_RESULT && v.rval.is_ok; }
static inline int a_is_err_raw(AValue v) { return v.tag == TAG_RESULT && !v.rval.is_ok; }
static inline AValue a_unwrap_unsafe(AValue v) { return *v.rval.inner; }
```

These avoid the overhead of wrapping in `a_bool()` just to immediately `a_truthy()` it.

## Code Generator Changes

### New functions in `cgen.a`

```
_emit_pat_check(pat, target_var, bm, ctx, li) -> [cond_code, bindings_code, li, lifted]
_emit_match(node, depth, bm, ctx, li) -> [code, li, lifted]
```

### emit_stmt addition

Add a `Match` branch in `emit_stmt` that:
1. Emits the scrutinee into a temp variable `__match`
2. Iterates over arms, calling `_emit_pat_check` for each pattern
3. Handles guard clauses
4. Emits arm bodies via existing `emit_stmt` calls
5. Chains arms with `else if`

### Variable pre-declaration

Pattern-bound variables (`PatIdent`, `PatRest`) need to be collected by `_collect_vars_in_stmts` so they're pre-declared at function scope. Add a `_collect_pattern_vars(pat)` helper and integrate it into the `Match` case of `_collect_vars_in_stmts`.

## Implementation Order

1. C runtime: `a_is_ok_raw`, `a_is_err_raw`, `a_unwrap_unsafe` inline helpers
2. `_collect_pattern_vars` for variable pre-declaration
3. `_emit_pat_check` for each pattern type (start simple: Wildcard, Ident, Literal, then Constructor, Array, Map)
4. `_emit_match` tying it together (scrutinee temp, arm chain, guards, bodies)
5. Wire into `emit_stmt` (Match tag) and `emit_expr` (MatchExpr tag)
6. Test program: `examples/c_targets/patterns.a`
7. Re-verify bootstrap fixed point

## Test Program

`examples/c_targets/patterns.a` covering:
- Literal match (int, string, bool)
- Wildcard and ident binding
- Ok/Err constructor patterns
- Array patterns: fixed, with rest
- Map patterns
- Guard clauses
- Nested patterns
- Match as the primary dispatch mechanism

## Notes

- The parser only produces `Match` statements (not `MatchExpr`). We implement both to be safe, but `MatchExpr` can be deferred if no parsed code produces it.
- `PatConstructor` in the bytecode compiler only binds `PatIdent` sub-patterns (not nested structural patterns). We should match that limitation initially, then extend.
- The `__variant_field__` mechanism in the bytecode compiler is for generic algebraic data types. For now, the C backend only needs `Ok`/`Err` -- the only constructors used in practice.
