---
name: Pattern Matching Overhaul
overview: Add array patterns, map patterns, and guard clauses to match expressions, fix the MatchLiteral VM bug, and enable full recursive nested pattern matching -- making match the primary tool for structural data routing.
todos:
  - id: pm-fix-literal
    content: "Fix MatchLiteral: change to two-field opcode (const_idx, skip_target), update compiler and VM"
    status: completed
  - id: pm-ast
    content: "Add Pattern::Array, Pattern::Map, ArrayPatElem, and guard: Option<Expr> to MatchArm in ast.rs"
    status: completed
  - id: pm-parser
    content: "Parse array patterns ([a, ...rest]), map patterns (#{key: pat}), and guard clauses (pat if cond =>) in parser.rs"
    status: completed
  - id: pm-interpreter
    content: Implement match_pattern for Array/Map patterns and guard evaluation in interpreter.rs
    status: completed
  - id: pm-compiler
    content: Compile array/map patterns and guards to bytecode using inline checks in compiler.rs
    status: completed
  - id: pm-checker-fmt
    content: Update bind_pattern in checker.rs and format_pattern in formatter.rs for new patterns
    status: completed
  - id: pm-tests
    content: Add integration tests and native .a tests for array/map patterns, guards, and nesting
    status: completed
  - id: pm-planning
    content: Update PLANNING.md with v0.27 milestone
    status: completed
isProject: false
---

# Pattern Matching Overhaul (v0.27)

## The Problem

`match` currently supports only `Wildcard`, `Ident`, `Literal`, and `Constructor` patterns. There is no way to match on array structure (`[first, ...rest]`) or map keys (`#{"status": 200}`). Guard clauses (`pat if cond => body`) are not supported. Additionally, `MatchLiteral` in the VM has a latent bug where `patch_jump` overwrites the constant index with a skip target but the handler never actually jumps on mismatch.

## New Syntax

```
; Array patterns
match args {
  [] => print_help()
  [cmd] => run_single(cmd)
  [cmd, ...rest] => run_with_args(cmd, rest)
}

; Map patterns
match response {
  #{"status": 200, "data": d} => process(d)
  #{"error": msg} => handle_error(msg)
  _ => fail("unknown")
}

; Guard clauses
match n {
  x if x > 0 => "positive"
  x if x < 0 => "negative"
  _ => "zero"
}

; Nested
match event {
  #{"type": "click", "pos": [x, y]} => handle_click(x, y)
  #{"type": "key", "code": code} => handle_key(code)
}
```

## Phase 1: Fix MatchLiteral VM bug

In [src/bytecode.rs](src/bytecode.rs):
- Change `MatchLiteral(usize)` to `MatchLiteral(usize, usize)` -- `(const_idx, skip_target)`
- Update `patch_jump` to patch the second field: `Op::MatchLiteral(_, ref mut t) => *t = target`

In [src/compiler.rs](src/compiler.rs):
- Emit `MatchLiteral(cidx, 0)` and patch the skip target

In [src/vm.rs](src/vm.rs):
- `MatchLiteral(const_idx, skip)`: compare top of stack to `constants[const_idx]`; on mismatch, jump to `skip`

## Phase 2: AST additions

In [src/ast.rs](src/ast.rs):
- Add new Pattern variants:

```rust
pub enum ArrayPatElem {
    Pat(Pattern),
    Rest(String),   // ...name
}

pub enum Pattern {
    Wildcard,
    Ident(String),
    Literal(Literal),
    Constructor(String, Vec<Pattern>),
    Array(Vec<ArrayPatElem>),            // NEW
    Map(Vec<(String, Pattern)>),         // NEW
}
```

- Add guard to MatchArm:

```rust
pub struct MatchArm {
    pub pattern: Pattern,
    pub guard: Option<Expr>,   // NEW
    pub body: MatchBody,
}
```

## Phase 3: Parser

In [src/parser.rs](src/parser.rs) `parse_pattern` (~line 602):
- `TokenKind::LBracket` -> parse array pattern: comma-separated elements where each is either `...ident` (rest) or a recursive `parse_pattern()`, until `]`
- `TokenKind::Hash` -> expect `{`, parse map pattern: `string_key: pattern` pairs, until `}`

In `parse_match_arm` (~line 589):
- After parsing the pattern and before `expect(FatArrow)`, check for `TokenKind::If` -- if present, parse guard expression

## Phase 4: Interpreter

In [src/interpreter.rs](src/interpreter.rs) `match_pattern` (~line 815):

- `Pattern::Array(elems)`: check `Value::Array`, count fixed elements vs rest, verify length, recursively match each element, collect rest into array if present
- `Pattern::Map(entries)`: check `Value::Map`, for each `(key, sub_pat)` verify key exists in map and recursively match the value

In `exec_stmt` / `eval_expr` match handling (~line 388, 623):
- After successful `match_pattern`, push scope, define bindings, then if `arm.guard.is_some()`, evaluate the guard expression; if falsy, pop scope and continue to next arm

## Phase 5: Compiler

In [src/compiler.rs](src/compiler.rs) `compile_match_stmt` and `compile_match_expr`:

For `Pattern::Array(elems)`:
1. `Dup` the scrutinee
2. Emit `type_of` call, compare to `"array"` constant, `JumpIfFalse(skip)`
3. Emit `len` call, compare to expected length (exact or `>=` if rest), `JumpIfFalse(skip)`
4. For each fixed element: `Dup`, `Const(index)`, `Index`, then recursively compile the sub-pattern (bind ident via `SetLocal`, or further nested checks)
5. For rest: `Dup`, emit `drop(n)` or slice to get remaining elements, `SetLocal`

For `Pattern::Map(entries)`:
1. `Dup` the scrutinee
2. Emit `type_of` call, compare to `"map"` constant, `JumpIfFalse(skip)`
3. For each `(key, sub_pat)`: `Dup`, `Const(key_string)`, emit `map.has` call, `JumpIfFalse(skip)`, then `Dup`, `Const(key_string)`, `Index`, recursively compile the sub-pattern

For guard clauses:
1. After all pattern bindings succeed, compile the guard expression
2. `JumpIfFalse(next_arm)` -- if guard is false, skip to next arm
3. Need `PopScope` handling to clean up bindings from failed guard

## Phase 6: Checker and Formatter

In [src/checker.rs](src/checker.rs) `bind_pattern`:
- `Pattern::Array(elems)`: for each `Pat(p)` recurse, for `Rest(name)` define as `Type::Unknown`
- `Pattern::Map(entries)`: for each `(_, pat)` recurse
- Guard: `check_expr` on guard if present

In [src/formatter.rs](src/formatter.rs) `format_pattern`:
- `Pattern::Array`: format as `[pat, pat, ...name]`
- `Pattern::Map`: format as `#{"key": pat, "key2": pat}`
- Guard: append ` if guard_expr` after pattern in arm formatting

## Phase 7: Tests

- Rust integration tests for array patterns: empty, fixed-length, rest, nested
- Rust integration tests for map patterns: single key, multiple keys, nested, missing key fails
- Rust integration tests for guards: basic, with bindings, multiple guarded arms
- Rust integration tests for nested patterns: map inside array, array inside map, literals inside both
- Native `.a` test file: `tests/test_patterns.a`
- Verify all existing match tests still pass

## Phase 8: PLANNING.md

Add v0.27 milestone with feature summary and test counts.
