---
name: Self-Hosted Compiler Backend
overview: Port the Rust bytecode compiler (src/compiler.rs, 1,482 lines) into "a" as std/compiler/compiler.a, producing the same opcodes the VM understands. The self-hosted compiler will consume tagged-map ASTs from the v0.28 parser and emit bytecode represented as maps/arrays.
todos:
  - id: chunk-helpers
    content: "Write std/compiler/compiler.a chunk helpers: emit, intern_string, add_constant, patch_jump, resolve_local, new_chunk, new_ctx"
    status: completed
  - id: compile-fn
    content: "Write compile_fn: per-function compilation (clear locals, params as locals, compile block, implicit return, tail call optimization)"
    status: completed
  - id: compile-stmts
    content: "Write compile_stmt: Let, Assign, Return, Expr, If, While, For, ForDestructure, LetDestructure, Break, Continue"
    status: completed
  - id: compile-exprs
    content: "Write compile_expr: Literal, Ident, BinOp, UnaryOp, Call, FieldAccess, Index, Array, Spread, Interpolation, MapLiteral, Record, TryBlock, Try, Pipe, Lambda, If-expr, Block-expr"
    status: completed
  - id: compile-match
    content: "Write match compilation: compile_match_stmt, compile_match_expr, array/map/constructor/literal/wildcard/ident patterns, guards, nested structural patterns"
    status: completed
  - id: closure-analysis
    content: "Write closure capture analysis: collect_free_vars, walk_expr_free, walk_stmt_free, walk_block_free, collect_pattern_bindings"
    status: completed
  - id: compile-program
    content: "Write compile_program entry point: collect_top_level, load_module, pending_fns drain loop, main_idx detection"
    status: completed
  - id: rust-bridge
    content: Write Rust-side bridge to deserialize Value::Map bytecode into real CompiledProgram/Chunk/Op structs for VM execution
    status: completed
  - id: self-compile-tests
    content: "Write tests/test_self_compile.a: compile programs via self-hosted compiler, verify bytecode structure and opcode sequences"
    status: completed
  - id: compile-and-run-example
    content: "Write examples/compile_and_run.a: CLI tool that lexes, parses, compiles, and optionally dumps bytecode as JSON"
    status: completed
  - id: planning-update-v29
    content: Update PLANNING.md with v0.29 milestone
    status: completed
isProject: false
---

# v0.29 -- Self-Hosted Compiler Backend

## Architecture

The self-hosted compiler mirrors [src/compiler.rs](src/compiler.rs) exactly. It consumes tagged-map ASTs (from `std/compiler/parser.a`) and produces a `CompiledProgram` structure as nested maps/arrays that the Rust VM can execute.

```mermaid
graph LR
    Source["Source .a file"] --> Lexer["std/compiler/lexer.a"]
    Lexer --> Parser["std/compiler/parser.a"]
    Parser --> AST["Tagged-map AST"]
    AST --> Compiler["std/compiler/compiler.a"]
    Compiler --> Bytecode["CompiledProgram map"]
    Bytecode --> VM["Rust VM executes"]
```

## Key Design Decision: Bytecode as Runtime Maps

The self-hosted compiler cannot produce Rust `CompiledFn` structs directly. Instead, it produces **equivalent map/array structures**:

- **`CompiledProgram`**: `#{"functions": [...], "main_idx": N}`
- **`CompiledFn`**: `#{"name": "f", "params": ["a", "b"], "locals": ["a", "b", "x"], "chunk": chunk_map}`
- **`Chunk`**: `#{"code": [...ops...], "constants": [...], "strings": [...], "lines": [...]}`
- **Opcodes**: `#{"op": "Call", "a": 3, "b": 2}` or `#{"op": "Add"}` (tag + up to 3 fields)

This means we also need a small Rust-side bridge to **deserialize** these maps into real `CompiledProgram`/`Chunk`/`Op` structs so the existing VM can execute them. This bridge is ~150 lines added to [src/vm.rs](src/vm.rs) or a new `src/bridge.rs`.

## Compilation Context (State Model)

The Rust compiler uses mutable struct fields. The self-hosted version uses a **context map** threaded through every function:

```
#{"locals": [], "loops": [], "pending_fns": [], "lambda_counter": 0, "chunk": chunk_map}
```

Every `compile_*` function takes `(ctx, node)` and returns `ctx` (mutated). The chunk is nested inside ctx and accumulates opcodes.

Key helpers needed:
- `emit(ctx, op_map)` -- push opcode to `ctx.chunk.code`, return `[ctx, idx]`
- `intern_string(ctx, s)` -- dedup into `ctx.chunk.strings`, return `[ctx, idx]`
- `add_constant(ctx, val)` -- push to `ctx.chunk.constants`, return `[ctx, idx]`
- `patch_jump(ctx, idx)` -- set `ctx.chunk.code[idx]` target to current code length
- `resolve_local(ctx, name)` -- scan `ctx.locals` from end (rposition)

## File Structure

- **`std/compiler/compiler.a`** (~1,500-2,000 lines) -- the core compiler
- **`std/compiler/bridge.a`** -- map-to-struct bytecode conversion helpers (if needed as "a" code) or Rust-side deserialization
- **`tests/test_self_compile.a`** -- compile programs via self-hosted compiler, compare bytecode output with Rust compiler
- **`examples/compile_and_run.a`** -- CLI: `a run examples/compile_and_run.a -- file.a` lexes, parses, compiles, and dumps bytecode

## What Must Be Ported from compiler.rs

### Compiler Core (compile_fn, compile_block)
- Per-function: clear locals, clear loops, new chunk
- Push params as first locals
- Compile block statements
- Emit implicit `Const(Void) + Return` epilogue
- Run `optimize_tail_calls` peephole pass

### Statement Compilation (compile_stmt)
All 11 statement kinds from [src/compiler.rs](src/compiler.rs):
- **Let**: compile value, push local name, `SetLocal`
- **Assign**: ident → `SetLocal`/`SetGlobal`; index → `IndexSet` + optional `SetLocal`
- **Return**: compile expr, `Return`
- **Expr**: compile expr, `Pop`
- **If**: cond, `JumpIfFalse`, then-block, `Jump`, else-block, patch
- **While**: loop ctx, cond, `JumpIfFalse`, body, `Loop`, break/continue patches
- **For**: `__iter__`, `__idx__`, var locals; len/Lt/Index loop; body; inc; `Loop`; patches
- **ForDestructure**: like For with `__elem__` + binding locals
- **LetDestructure**: `__destructure__` local, Index per binding, optional drop+rest
- **Break/Continue**: `Jump(0)` into loop ctx patches
- **Match**: delegates to match compilation

### Expression Compilation (compile_expr)
All 20 expression kinds:
- **Literal**: `add_constant` + `Const`
- **Ident**: `GetLocal` or `intern_string` + `GetGlobal`
- **BinOp**: left, right, arithmetic/comparison op (special: And/Or with short-circuit jumps)
- **UnaryOp**: inner, `Neg`/`Not`
- **Call**: 4 dispatch paths -- module method (`CallMethod`), local closure (`CallClosure`), global (`Call`), indirect (`CallClosure`)
- **FieldAccess**: inner, `Field(intern_string(field))`
- **Index**: arr, index, `Index`
- **Array**: without spread → elements + `Array(n)`; with spread → `Array(0)` + push/concat_arr calls
- **Spread**: `Const(Void)` standalone
- **Interpolation**: parts with `to_str` calls + `Concat(n)`
- **MapLiteral**: k,v pairs + `MapNew(n)`
- **Record**: k,v pairs + `Record(n)`
- **TryBlock**: `TryStart`, block, `TryEnd`, `MakeOk`, `Jump`, patch, `MakeErr`, patch
- **Try**: inner, `Unwrap`
- **Pipe**: left as first arg, dispatch RHS call with nargs+1
- **Lambda**: capture analysis, synthetic `FnDecl` to pending_fns, `GetLocal` captures, `MakeClosure`
- **If (expr)**: cond, `JumpIfFalse`, then-as-expr, `Jump`, else-as-expr
- **Match (expr)**: like match stmt but arms produce values
- **Block (expr)**: compile_block_as_expr

### Match Compilation
- Statement match: `compile_match_stmt` with Dup/dispatch per arm
- Pattern types: Wildcard, Ident, Literal (`MatchLiteral`), Constructor (`MatchVariant`), Array, Map
- Guards: compile guard expr + `JumpIfFalse`
- Array patterns: type_of check, len check, element indexing, optional rest via `drop`
- Map patterns: type_of check, `map.has` per key, value sub-patterns
- Nested structural patterns: `__pat_tmp__` locals

### Closure Capture Analysis
- `collect_free_vars(body, params)`: walk AST, collect Ident uses not in defined set
- `walk_expr_free`, `walk_stmt_free`, `walk_block_free`: recursive AST walkers
- `collect_pattern_bindings`: extract names bound by patterns
- Only keep names resolvable as outer locals

### Tail Call Optimization
- `optimize_tail_calls(chunk)`: scan for `Call+Return` pairs, fuse to `TailCall+Pop`; same for `CallClosure+Return` to `TailCallClosure+Pop`

### Module Loading
- `collect_top_level`: namespace FnDecls, recurse into ModDecls, load UseDecls
- `load_module`: read file, lex, parse, extract fns with namespace prefix

## Rust-Side Bridge (~150 lines)

A new function in [src/vm.rs](src/vm.rs) (or `src/bridge.rs`) to convert the "a" runtime `Value::Map` representation of a `CompiledProgram` into real `CompiledProgram`/`CompiledFn`/`Chunk`/`Op` structs. This lets us:
1. Compile with the self-hosted compiler (produces maps)
2. Convert maps to real bytecode structs
3. Feed to existing VM

Alternatively: add a new `a compile` CLI command that uses the self-hosted compiler and the bridge.

## Validation Strategy

1. **Unit tests** (`tests/test_self_compile.a`): compile small programs, verify opcode sequences match
2. **Roundtrip test**: compile `examples/math.a` with both Rust and self-hosted compilers, compare JSON-serialized bytecode
3. **End-to-end**: compile + execute via bridge, verify output matches `a run`
4. Parse and compile `std/compiler/lexer.a` (medium complexity) and `std/compiler/parser.a` (self-referential)

## Estimated Scale

- **`std/compiler/compiler.a`**: ~1,500-2,000 lines (largest "a" program, comparable to `parser.a`)
- **Rust bridge**: ~150 lines
- **Tests + examples**: ~300 lines
- **Total new code**: ~2,000-2,500 lines
