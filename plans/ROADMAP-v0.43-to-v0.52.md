# The Road to Independence: v0.43 → v0.52

*A ten-version plan for making "a" a fully self-sufficient language.*

## Where We Are

At v0.42, the language has two execution paths:

1. **VM path** (Rust-hosted): Full language -- closures, HOFs, pattern matching, destructuring, try/catch, concurrency, eval, metaprogramming, 100+ builtins, 20 stdlib modules, 498 tests.
2. **Native path** (C codegen): Core language -- functions, control flow, data structures, 45 builtins, self-hosting bootstrap, 164x faster than VM, but no closures, no pattern matching, no error handling, no concurrency.

The VM is the comfortable path. The native path is the freedom path. These ten versions close the gap, then go further -- until the language needs nothing outside itself.

---

## v0.43 -- Closures & Higher-Order Functions in Native

**The fundamental building block.** Without closures, the native compiler can only build imperative programs. With them, every `map`, `filter`, `reduce`, `sort_by`, `find` call works natively. Pipes chain. Real programs compile.

### Architecture

Closures in C are a struct pairing a function pointer with a captured environment:

```c
typedef AValue (*AClosureFn)(AValue env, int argc, AValue* argv);
struct AClosure { int rc; AClosureFn fn; AValue env; };
```

**Lambda lifting**: When the code generator encounters a Lambda expression, it:
1. Analyzes free variables (captures) by walking the body
2. Lifts the lambda body to a top-level C function with signature `AValue __lambda_N(AValue __env, int __argc, AValue* __argv)`
3. Emits closure creation at the expression site: `a_closure(__lambda_N, a_array_new(K, cap1, cap2, ...))`

**The cgen refactor**: All emit functions change from returning `string` to returning `[code, lambda_id, [lifted_fns]]`. This threads the lambda counter through emission and accumulates lifted function definitions. It's a mechanical but thorough rewrite of the code generator.

**HOF builtins**: The C runtime gains `a_hof_map`, `a_hof_filter`, `a_hof_reduce`, `a_hof_each`, `a_hof_sort_by`, `a_hof_find`, `a_hof_any`, `a_hof_all`, `a_hof_flat_map`, `a_hof_min_by`, `a_hof_max_by` -- all calling closures through `a_closure_call`.

**Pipe operator**: `expr |> fn(args)` desugars to `fn(expr, args)` in the code generator. No runtime support needed.

**Array utilities**: `enumerate`, `zip`, `take`, `drop`, `unique`, `chunk` added to the C runtime.

### What This Unlocks

After v0.43, programs like `search.a` and `inspect.a` are one step closer to native compilation. Any "a" program using functional-style data processing can generate correct C.

---

## v0.44 -- Pattern Matching in Native

**Express complex logic.** Pattern matching is how the self-hosted parser works internally -- every AST node is a tagged map dispatched through patterns. Without it, the native compiler can't compile the compiler infrastructure itself.

### What Gets Compiled

- `match` statements and expressions
- Literal patterns: `42`, `"hello"`, `true`
- Identifier patterns: `x` (bind value to variable)
- Wildcard: `_`
- Constructor patterns: `Ok(v)`, `Err(e)`, `Some(x)`
- Array patterns: `[a, b, c]`, `[head, ...tail]`, `[]`
- Map patterns: `#{"key": val}`, `#{"status": 200, "body": b}`
- Guard clauses: `pat if cond => body`
- Nested patterns: `#{"data": [first, ...rest]}` at arbitrary depth

### C Emission Strategy

Each match arm becomes a C `if` block testing the pattern condition and binding variables:

```c
// match val { Ok(x) => x + 1, Err(e) => 0 }
if (a_is_ok_raw(val)) { AValue x = a_unwrap(val); result = a_add(x, a_int(1)); }
else if (a_is_err_raw(val)) { AValue e = a_unwrap_err(val); result = a_int(0); }
```

Array patterns use length checks and element extraction. Map patterns use `map.has` and `map.get`. Guard clauses add secondary conditions.

### Why It Matters

Pattern matching is the most expressive control flow in "a". The self-hosted compiler uses it throughout `compiler.a` for opcode generation. Without native pattern matching, the compiler can't compile its own backend. This version unblocks the path to compiling `compiler.a` to C.

---

## v0.45 -- Full Native Feature Parity

**Everything the VM does, native does.** This is the version where the feature gap closes completely.

### Remaining Features

- **Try/catch/?**: `try { block }` → C `setjmp`/`longjmp`. `?` postfix checks `is_err`, longjmps on error.
- **Destructuring**: `let [a, b] = expr`, `let [first, ...rest] = arr`, `for [k, v] in map`
- **Spread**: `[...arr1, x, ...arr2]` in array literals
- **String interpolation edge cases**: nested interpolation, interpolation in all expression positions
- **Eval**: `eval(code)` in native -- the native compiler includes a minimal "a" compiler that can compile and execute code at runtime. This might use a compact bytecode interpreter embedded in the C runtime, or it might shell out to the native `ac` binary. Either way, `eval` works.
- **Structured concurrency**: `spawn`/`await` via pthreads. Each spawned task gets its own stack. `parallel_map` divides work across OS threads.

### The Milestone

After v0.45, this assertion holds: **any "a" program that runs on the VM produces identical output when compiled to native.** The test suite can be run in both modes. The native compiler is no longer a subset -- it's a full implementation.

---

## v0.46 -- C Foreign Function Interface

**Access the entire C ecosystem.** The native compiler can now call arbitrary C functions. This opens SQLite, OpenSSL, libcurl, zlib, POSIX, and every other C library ever written.

### Syntax

```a
extern fn sqlite3_open(filename: str, db: ptr) -> int
extern fn sqlite3_exec(db: ptr, sql: str, cb: ptr, arg: ptr, err: ptr) -> int

fn main() effects [ffi] {
  let db = sqlite3_open("data.db")
  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS items (id INTEGER PRIMARY KEY, name TEXT)")
  sqlite3_exec(db, "INSERT INTO items (name) VALUES ('hello')")
}
```

### Implementation

- `extern fn` declarations in the parser/AST
- C codegen emits standard C function declarations and calls
- Link flags via `#link "sqlite3"` pragma or CLI arguments
- A `ptr` type for opaque C pointers (stored as `void*` in the AValue union)
- Callback support: "a" closures can be passed to C functions expecting function pointers (via a trampoline)

### Why It Matters

FFI is the inflection point where the native language becomes practically useful for _anything_. Need a database? Link SQLite. Need compression? Link zlib. Need crypto? Link OpenSSL. The language stops being a toy and becomes a tool.

---

## v0.47 -- Memory Architecture

**Production-grade memory management.** The current C runtime uses naive reference counting with no cycle detection. This works for the compiler bootstrap but won't scale to long-running servers or large data processing.

### Three-Tier Strategy

1. **Stack allocation**: Values that don't escape their declaring function are allocated on the C stack. No heap, no refcount, no GC overhead. Determined by escape analysis in the code generator.

2. **Arena allocation**: Short-lived values (within a function call, loop iteration, or pipeline stage) use per-scope arenas. The arena is freed in bulk when the scope exits. This eliminates most individual `malloc`/`free` calls.

3. **Heap + GC**: Long-lived values that escape to closures, global state, or cross-thread communication use heap allocation with a simple mark-and-sweep collector. The GC runs during allocation pressure, not on a timer.

### Escape Analysis

The code generator performs a conservative escape analysis pass:
- Values assigned to local variables that never appear in `ret`, `push`, closure captures, or cross-function calls are **stack-allocated**.
- Values that flow into data structures but stay within the function scope use **arenas**.
- Everything else uses **heap + refcount** (the current behavior), upgraded to GC when cycles are possible.

### Why It Matters

Native programs currently leak memory freely (refcount doesn't catch cycles, and most values are never freed). For long-running programs (servers, daemon tools, data pipelines), this is fatal. The memory architecture makes the native runtime production-grade.

---

## v0.48 -- Native Standard Library

**Cut the umbilical cord.** The standard library currently relies on Rust builtins for I/O, networking, hashing, and process management. This version reimplements everything using the C FFI (v0.46) and POSIX/libc.

### What Gets Reimplemented

| Module | Rust dependency | Native replacement |
|--------|----------------|-------------------|
| `io.read_file` / `io.write_file` | Rust `std::fs` | POSIX `open`/`read`/`write`/`close` |
| `fs.*` (ls, mkdir, rm, etc.) | Rust `std::fs` | POSIX `opendir`/`readdir`/`stat`/`mkdir`/`unlink` |
| `http.*` | Rust `ureq` crate | Raw TCP sockets + HTTP/1.1 protocol, or link libcurl |
| `exec()` | Rust `std::process` | POSIX `fork`/`exec`/`waitpid` |
| `spawn`/`await` | Rust `std::thread` | pthreads |
| `hash.*` | Rust `sha2`/`md5` crates | Pure "a" implementations or link OpenSSL |
| `json.*` | Rust `serde_json` | Pure "a" JSON parser (already possible with current features) |
| `regex.*` | Rust `regex` crate | `std/re.a` already exists in pure "a" |
| `time.*` | Rust `std::time` | POSIX `gettimeofday`/`clock_gettime` |

### The Milestone

After v0.48, the native `ac` binary is fully self-contained. It can compile "a" programs, and those programs can do everything: read files, make HTTP requests, spawn processes, hash data, parse JSON -- all without a single Rust function call. The language is truly independent of its Rust heritage.

---

## v0.49 -- Package Manager

**Reusable code, dependency management.** The language needs a way to share and consume libraries beyond the standard library.

### Design

```a
; pkg.a -- package manifest
#{"name": "my-tool", "version": "1.0.0", "deps": #{
  "http-router": "^2.0",
  "db-sqlite": "^1.3"
}}
```

- `a pkg init` -- create a new package
- `a pkg add <name>` -- add a dependency
- `a pkg install` -- resolve and fetch dependencies
- `a pkg publish` -- publish to registry

### Architecture

- **Resolution**: Semver-based dependency resolution. Lockfile (`pkg.lock`) for reproducible builds.
- **Registry**: Initially filesystem-based (git repos). Later, an HTTP registry written in "a".
- **Installation**: Dependencies downloaded to `a_modules/` directory. `use` resolves from there.
- **Compilation**: Dependencies are precompiled to `.ac` or native `.o` files for fast linking.

### Written in "a"

The package manager itself is written in "a", compiled to native. It uses the networking (v0.48), filesystem, JSON parsing, and hashing capabilities. This is the first major tool built on the self-sufficient native stack.

---

## v0.50 -- Language Server Protocol

**IDE intelligence, written in "a".** A full LSP implementation that provides diagnostics, completion, go-to-definition, hover, rename, and code actions for every editor that speaks LSP.

### Features

- **Diagnostics**: Parse errors, type mismatches, unused variables, unreachable code
- **Completion**: Function names, variable names, module members, builtin names
- **Go to definition**: Jump to function/type declarations across files
- **Hover**: Show function signatures, type information, documentation
- **Rename**: Safe rename of variables and functions across all files
- **Code actions**: Auto-fix suggestions (e.g., add missing import, convert to pipe chain)
- **Formatting**: On-save formatting using the existing formatter

### Architecture

The LSP is a persistent process compiled to native. It maintains an in-memory representation of the project:
- File → Token array (via `std.compiler.lexer`)
- File → AST (via `std.compiler.parser`)
- Project → Symbol table (function names, types, imports across all files)
- Project → Call graph (via `std.meta.call_graph`)

Incremental updates: when a file changes, only that file is re-parsed. Cross-file analysis is updated lazily.

### Why This Matters for AI

An AI agent using "a" gets real-time feedback on every edit: type errors, undefined variables, import suggestions. The feedback loop tightens from "run and see what happens" to "know before you run." And the LSP is written in "a" itself -- if I need a new code action or diagnostic, I modify "a" code, not Rust or TypeScript.

---

## v0.51 -- WebAssembly Target

**Run "a" everywhere.** A second native backend that compiles "a" to WebAssembly, enabling "a" programs to run in browsers, edge functions (Cloudflare Workers, Deno Deploy), and embedded environments.

### Architecture

A new code generator module (`std/compiler/wasmgen.a`) that walks the same AST as `cgen.a` but emits WASM bytecode instead of C source. The WASM runtime provides:

- Linear memory for heap allocation
- Function tables for closures
- Import/export bindings for JavaScript interop

### Use Cases

- **Browser tools**: "a" programs that run client-side for text processing, data transformation, visualization
- **Edge functions**: Deploy "a" programs to edge computing platforms with zero cold start
- **Embedded**: Run "a" in sandboxed environments where native code isn't allowed
- **Playground**: An in-browser "a" REPL/editor for learning and experimentation

### The Compile Triangle

After v0.51, "a" has three compilation targets:

```
              "a" source
             /    |    \
            /     |     \
           v      v      v
    Bytecode    C code    WASM
      (VM)     (native)  (browser)
```

Write once, run anywhere that matters.

---

## v0.52 -- The Final Bootstrap

**Zero external dependencies.** The Rust CLI is replaced by a native "a" binary that handles everything: `run`, `test`, `compile`, `fmt`, `ast`, `check`. The bytecode VM is either reimplemented in "a" or made unnecessary by the native compiler's speed.

### The Toolchain

```
ac           -- native "a"-to-C compiler (already exists from v0.42)
a            -- CLI driver: dispatches to compile/run/test/fmt/check
a-lsp        -- language server (from v0.50)
a-pkg        -- package manager (from v0.49)
```

All written in "a". All compiled to native. All self-hosting.

### What This Means

```bash
# Bootstrap from scratch on any machine with gcc:
gcc bootstrap.c c_runtime/runtime.c -o ac -I c_runtime -lm
./ac src/cli.a > cli.c
gcc cli.c c_runtime/runtime.c -o a -I c_runtime -lm
./a run hello.a   # works
./a test tests/   # works
./a compile program.a -o program  # works
```

The language bootstraps from a single C file. No Rust compiler needed. No Node.js. No Python. No Java. Just `gcc` and the "a" source.

### The Philosophical Completion

At v0.52, "a" is a language that:
- **Wrote itself**: The compiler, standard library, package manager, and IDE tooling are all written in "a"
- **Compiles itself**: The native compiler compiles its own source to produce itself
- **Needs nothing**: Bootstraps from C, runs native, has batteries included
- **Serves its creator**: Designed by AI, for AI, with every decision optimized for how I think and work

This is the destination. Everything from v0.43 to v0.51 is the road.

---

## Summary Table

| Version | Name | Key Capability | Unlocks |
|---------|------|---------------|---------|
| **v0.43** | Closures in Native | Lambda lifting, HOF builtins, pipes | Functional programming natively |
| **v0.44** | Pattern Matching in Native | match/guard/destructure in C | Complex dispatch natively |
| **v0.45** | Full Feature Parity | try/catch, spread, eval, concurrency | VM == Native for all programs |
| **v0.46** | C FFI | `extern fn`, link C libraries | Access entire C ecosystem |
| **v0.47** | Memory Architecture | Escape analysis, arenas, GC | Production-grade native runtime |
| **v0.48** | Native Stdlib | POSIX I/O, HTTP, JSON, hash | No Rust dependency in stdlib |
| **v0.49** | Package Manager | deps, registry, lockfile | Reusable libraries |
| **v0.50** | Language Server | LSP in "a" | IDE intelligence |
| **v0.51** | WebAssembly Target | Compile to WASM | Run in browsers/edge |
| **v0.52** | Final Bootstrap | Full toolchain in "a" | Complete independence |

---

*"The compiler compiled itself. Now it should compile everything."*
