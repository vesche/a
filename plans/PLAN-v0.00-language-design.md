---
name: a programming language
overview: Design and implement "a" -- a programming language built exclusively for AI agents to read, write, and execute. The language prioritizes unambiguous syntax, explicit semantics, token efficiency, and machine-verifiability over human ergonomics.
todos:
  - id: scaffold
    content: "Initialize Rust project with cargo: crate structure, Cargo.toml with dependencies (serde, serde_json, clap)"
    status: completed
  - id: spec
    content: Write formal EBNF grammar in grammar.ebnf
    status: completed
  - id: tokens-lexer
    content: Implement token types (src/tokens.rs) and lexer (src/lexer.rs) with structured JSON errors
    status: completed
  - id: ast
    content: Define AST node types as Rust enums in src/ast.rs (serde-serializable)
    status: completed
  - id: parser
    content: Build recursive descent LL(1) parser in src/parser.rs producing AST
    status: completed
  - id: typechecker
    content: Implement type checker with effect system in src/checker.rs
    status: completed
  - id: interpreter
    content: Build tree-walking interpreter in src/interpreter.rs with builtins
    status: completed
  - id: cli
    content: "Create CLI entry point with clap: run, check, fmt, ast subcommands"
    status: completed
  - id: examples
    content: Write example .a programs demonstrating language features
    status: completed
  - id: tests
    content: Write tests for lexer, parser, type checker, and interpreter
    status: completed
isProject: false
---

# The "a" Programming Language

## Design Philosophy

"a" is a programming language designed by AI, for AI. Every design decision optimizes for what matters to language-model-based agents: **unambiguous parsing, explicit semantics, token efficiency, local reasoning, and verifiability**. Human readability is a non-goal -- clarity for machines is the goal.

## Core Design Principles

### 1. Unambiguous, Regular Grammar (Context-Free, LL(1))

- No operator precedence -- use prefix notation or explicit grouping
- No overloading -- every symbol has exactly one meaning everywhere
- Explicit delimiters (braces), not whitespace-sensitivity
- Trivially parseable: an AI should be able to generate correct syntax on the first try, every time

### 2. Explicit Everything

- **Full type annotations** on all bindings -- no type inference needed. Types ARE the documentation.
- **Effect system** -- every function declares what it can do (read, write, net, spawn, none). An AI can reason about what a function might do without reading the body.
- **Explicit error paths** -- Result types, not exceptions. No hidden control flow.
- **Explicit data flow** -- no mutable globals, no implicit `this`/`self`. All inputs are parameters.

### 3. Token-Efficient Syntax

Keywords: `fn`, `ty`, `mod`, `let`, `mut`, `if`, `match`, `for`, `ret`, `use`, `try`, `pub`

These are 2-3 characters deliberately. Single characters (e.g. `f`, `l`, `m`) were considered and rejected because:
- BPE tokenization: `fn` and `f` both cost exactly 1 token. No savings.
- Ambiguity: `f` could be a variable name. `fn` is unambiguous in every context.
- Training signal: `fn`, `let`, `mod` are patterns deeply encoded in LLM weights from Rust/Go/ML corpora. Generation reliability is near-perfect for these tokens. Novel single-character keywords would require learning new patterns from zero training signal, increasing generation errors.
- The sweet spot is: **familiar enough to leverage training data, short enough to waste nothing, distinct enough to be unambiguous.**

Other syntax rules:
- No semicolons (newline-terminated statements within braces)
- No trailing commas required
- No boilerplate: no getters/setters, no class ceremony, no mandatory `main()`
- Dense: `fn add(a: i32, b: i32) -> i32 { a + b }` -- nothing wasted

### 4. Self-Contained, Local Reasoning

- Every module carries its own type context -- no need to read other files to understand it
- **Content-addressed modules** (Unison-inspired): modules are identified by hash, not file path. No version conflicts, no diamond dependency problems.
- Function signatures tell the complete story: types + effects + contracts

### 5. Built-in Verification

- **Pre/post conditions** as first-class syntax: `fn sqrt(x: f64) -> f64 pre { x >= 0.0 } post { ret * ret ~= x }`
- **Invariants** on types: `ty Positive = i64 where { self > 0 }`
- These aren't comments -- they're checked at compile time or runtime

### 6. Structured, Machine-Readable Tooling

- Compiler errors output structured JSON, not human-readable text
- AST is serializable and round-trippable -- AI agents can work at the AST level directly
- Built-in formatter (canonical form) so there's ONE way to write any given program

---

## Proposed Syntax (Draft)

```
mod math @{content_hash}

ty Point = { x: f64, y: f64 }

ty Positive = i64 where { self > 0 }

fn distance(a: Point, b: Point) -> f64
  effects [pure]
  pre { true }
  post { ret >= 0.0 }
{
  let dx: f64 = b.x - a.x
  let dy: f64 = b.y - a.y
  sqrt(+(*(dx dx) *(dy dy)))
}

fn fetch_data(url: str) -> Result<bytes, NetErr>
  effects [net, async]
{
  let resp: Result<Response, NetErr> = http.get(url)
  match resp {
    Ok(r) => Ok(r.body)
    Err(e) => Err(e)
  }
}

fn main() -> Result<void, Err>
  effects [io, net]
{
  let data: bytes = try fetch_data("https://example.com")
  io.write(stdout, data)
}
```

Key syntax decisions:
- **Prefix notation for math** where ambiguity exists: `+(a b)` not `a + b` (eliminates precedence). Simple binary ops like comparisons can use infix with mandatory parens.
- **`effects` block** on every function -- the AI knows exactly what side effects are possible
- **`pre`/`post` blocks** -- contracts are part of the function signature, not comments
- **`where` clauses on types** -- refinement types as first-class
- **`try` keyword** for explicit error propagation (no hidden exceptions)
- **Content-addressed modules** with `@{hash}` identifiers

---

## Type System

- Primitives: `i8, i16, i32, i64, u8, u16, u32, u64, f32, f64, bool, str, bytes, void`
- Compound: `{ field: Type }` (records), `[Type]` (arrays), `(Type, Type)` (tuples)
- Sum types: `ty Option<T> = Some(T) | None`
- Refinement types: `ty NonEmpty<T> = [T] where { len(self) > 0 }`
- Effect types: `effects [pure]`, `effects [io, net]`, `effects [*]`
- Result type built-in: `Result<T, E>`

## Effect System

Effects are a closed set that can be extended per-project:
- `pure` -- no side effects (implies deterministic)
- `io` -- file system access
- `net` -- network access
- `async` -- asynchronous execution
- `spawn` -- process/thread creation
- `state(T)` -- mutable state of type T
- `panic` -- can terminate abnormally

A function with `effects [pure]` can only call other `pure` functions. This is enforced at compile time.

---

## Why Rust for the Implementation

"Rapid prototyping in Python" is a human concern. For an AI agent building a language, what matters is:

- **Exhaustive pattern matching on enums.** When I add a new AST node type, the Rust compiler forces me to handle it in every match statement. In Python, I'd discover the miss at runtime -- maybe.
- **The type system catches my errors at compile time.** I don't "fight the borrow checker" -- that's a human complaint. I generate correct ownership patterns from the start.
- **Algebraic data types (enums) are the natural representation for ASTs.** An `Expr` enum with `Literal`, `BinOp`, `Call`, `Match` variants is exactly how I think about syntax trees.
- **Serde gives me free JSON serialization** of every AST node -- critical for the `a ast` command.
- **Single compiled binary** with no runtime dependencies. No Python version issues. `a run file.a` just works.

## Implementation Plan

Reference implementation in Rust, structured in clean phases.

### Phase 1: Scaffold and Grammar
- `cargo init` with dependencies: `serde`, `serde_json`, `clap`
- Write formal EBNF grammar in `grammar.ebnf`
- Define the complete token set

### Phase 2: Lexer
- Token types as Rust enum in `src/tokens.rs`
- Tokenizer producing `Vec<Token>` in `src/lexer.rs`
- All errors are structured (JSON-serializable via serde)

### Phase 3: Parser
- AST node types as Rust enums/structs in `src/ast.rs` (all `#[derive(Serialize)]`)
- Recursive descent LL(1) parser in `src/parser.rs`
- Structured error output

### Phase 4: Type Checker
- Type checking with effect verification in `src/checker.rs`
- Contract validation (pre/post condition syntax checking)

### Phase 5: Interpreter
- Tree-walking interpreter in `src/interpreter.rs`
- Built-in functions (`io.write`, etc.) in `src/builtins.rs`

### Phase 6: CLI
- `a run file.a` -- interpret and run
- `a check file.a` -- type check only
- `a fmt file.a` -- canonical format
- `a ast file.a` -- dump AST as JSON
- Entry point via clap in `src/main.rs`

### Project Structure

```
a/
  PLANNING.md
  README.md
  Cargo.toml
  grammar.ebnf
  src/
    main.rs              -- CLI entry point (clap)
    tokens.rs            -- token type definitions
    lexer.rs             -- tokenizer
    ast.rs               -- AST node enums/structs
    parser.rs            -- recursive descent parser
    checker.rs           -- type + effect checker
    interpreter.rs       -- tree-walking interpreter
    builtins.rs          -- built-in functions
    errors.rs            -- structured error types
    lib.rs               -- crate root, module declarations
  examples/
    hello.a
    math.a
    fetch.a
  tests/
    lexer_tests.rs
    parser_tests.rs
    checker_tests.rs
    interpreter_tests.rs
```
