---
name: v0.37 AST Emitter
overview: "Add `std/compiler/emitter.a` -- an AST-to-source pretty printer that converts tagged-map ASTs back into clean, formatted \"a\" source code. This closes the metaprogramming loop: parse -> transform -> emit -> eval."
todos:
  - id: emitter-scaffold
    content: Create std/compiler/emitter.a with emit() entry point, indent helper, and top-level dispatch (Program, FnDecl, UseDecl, ModDecl, TypeDecl)
    status: completed
  - id: emitter-stmts
    content: Implement emit_stmt covering Let, Assign, Return, ExprStmt, If/Else, Match, For, While, Break, Continue, destructuring forms
    status: completed
  - id: emitter-exprs
    content: Implement emit_expr covering all expression tags (literals, BinOp, Call, FieldAccess, Index, Lambda, Pipe, Array, Map, Interpolation, Try, etc.)
    status: completed
  - id: emitter-patterns-types
    content: Implement emit_pattern (all pattern tags) and emit_type (all type expression tags)
    status: completed
  - id: emitter-tests
    content: Create tests/test_emitter.a with roundtrip tests (parse -> emit -> reparse -> execute) and per-construct verification
    status: completed
  - id: emitter-planning
    content: Update PLANNING.md with v0.37 milestone
    status: completed
isProject: false
---

# v0.37 -- AST Emitter: Code Generation from ASTs

## Why this matters

The self-hosted compiler can go **source -> AST -> bytecode**, and `eval` can execute code at runtime. But there is no way to go **AST -> source**. The Rust side has `src/formatter.rs`, but the self-hosted pipeline has no equivalent.

Without this, programmatic code generation means string concatenation -- fragile and ugly. With an emitter, the full metaprogramming loop is:

```mermaid
flowchart LR
    Source["Source Code"] -->|"lex + parse"| AST["Tagged-Map AST"]
    AST -->|"transform"| AST2["Modified AST"]
    AST2 -->|"emit"| Source2["New Source Code"]
    Source2 -->|"eval / write"| Execute["Execute or Save"]
```

This is what makes Lisp powerful -- homoiconic manipulation. With this module, I can construct ASTs using the `mk_*` builders from [std/compiler/ast.a](std/compiler/ast.a), transform them programmatically, and emit clean source. Code that writes code, cleanly.

## The module: `std/compiler/emitter.a`

A direct port of the formatting logic from [src/formatter.rs](src/formatter.rs), but operating on tagged maps (`#{"tag": "...", ...}`) instead of Rust enum types. Uses `match node["tag"]` dispatch throughout.

### API

- **`emit(ast)`** -- entry point. Takes a `Program` AST, returns formatted source string
- **`emit_top_level(node, level)`** -- dispatches on FnDecl, TypeDecl, ModDecl, UseDecl
- **`emit_fn(node, level)`** -- function with params, return type, effects, pre/post, body
- **`emit_block(node, level)`** -- `{ ... }` with indented statements
- **`emit_stmt(node, level)`** -- Let, Assign, Return, If, Match, For, While, Break, Continue, destructuring
- **`emit_expr(node)`** -- all expression tags (Int, Float, String, BinOp, Call, Lambda, Pipe, etc.)
- **`emit_pattern(node)`** -- PatIdent, PatWildcard, PatLiteral, PatConstructor, PatArray, PatMap
- **`emit_type(node)`** -- TyPrim, TyArray, TyMap, TyFn, TyNamed, TyInfer

### Formatting conventions (matching `formatter.rs`)

- 2-space indentation per nesting level
- BinOp: `(left OP right)` with spaces around operator
- Blocks: `{\n` + indented stmts + `}`
- Functions: `fn name(p1, p2) -> RetType` then effects/pre/post on next lines
- Match arms: `pattern => expr` or `pattern => {\n  body\n}`
- Strings with `\"` escaping, interpolation parts as `{expr}`

### Tags to handle (66 total from [std/compiler/ast.a](std/compiler/ast.a))

- **Top-level (5):** Program, FnDecl, TypeDecl, ModDecl, UseDecl
- **Type bodies (5):** TypeRecord, TypeSum, TypeAlias, Variant, Field
- **Type exprs (8):** TyPrim, TyNamed, TyArray, TyTuple, TyRecord, TyFn, TyMap, TyInfer
- **Statements (12):** Let, Assign, Return, ExprStmt, If, Match, For, While, LetDestructure, ForDestructure, Break, Continue
- **Else (2):** ElseBlock, ElseIf
- **Match (1):** MatchArm
- **Patterns (8):** PatIdent, PatConstructor, PatLiteral, PatWildcard, PatArray, PatElem, PatRest, PatMap, PatMapEntry
- **Expressions (22):** Int, Float, String, Bool, Void, Ident, BinOp, UnaryOp, Call, FieldAccess, Index, Try, TryBlock, IfExpr, MatchExpr, BlockExpr, Array, Record, Lambda, Pipe, Interpolation, MapLiteral, Spread
- **Sub-nodes (4):** Param, Block, RecordField, MapEntry, InterpLit, InterpExpr

## Test suite: `tests/test_emitter.a`

The gold standard test: **parse real .a files, emit them, parse the emitted code, and verify both versions compile and execute identically.**

- **Roundtrip tests**: parse source -> emit -> parse again -> compile both -> compare execution output
- **Individual construct tests**: verify each language construct emits correctly (fn, if/else, match, for, while, lambda, pipe, etc.)
- **AST builder tests**: construct ASTs programmatically using `mk_*`, emit to source, verify output

## Changes to PLANNING.md

Add v0.37 milestone entry.

## Estimated scale

- `std/compiler/emitter.a`: ~500-700 lines (mechanical tag dispatch + string building)
- `tests/test_emitter.a`: ~150-200 lines (roundtrip and construct tests)
- `PLANNING.md`: milestone entry
