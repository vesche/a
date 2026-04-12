# The Road to Independence: v0.43 → v0.52

*A ten-version plan for making "a" a fully self-sufficient language.*

## Where We Are

At v0.47, the native path has memory management -- the runtime is production-grade:

1. **VM path** (Rust-hosted): Full language -- closures, HOFs, pattern matching, destructuring, try/catch, concurrency, eval, metaprogramming, 100+ builtins, 20 stdlib modules, 498 tests.
2. **Native path** (C codegen): Most language features + FFI + memory management -- functions, control flow, closures, HOFs, pipes, pattern matching, try/catch/?, destructuring, spread, else-if chains, module imports, `extern fn` declarations, `ptr` type, reference counting with ownership semantics, arena allocator, mark-and-sweep GC, 75+ builtins, self-hosting bootstrap (12,300 lines C, three-stage fixed point verified), 15 example programs, 4 memory-specific test programs.

**What's done** (v0.43-v0.47): closures, lambda lifting, HOF builtins, pipe operator, pattern matching, try/catch/?, let/for destructuring, array spread, native I/O, JSON parsing, module import aliases, else-if codegen, index assignment, map iteration, test harness, C FFI with `extern fn` and automatic AValue shim generation, reference counting ownership model, arena allocator, mark-and-sweep GC.

**What remains**: native stdlib (v0.48), and everything beyond.

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

## v0.45 -- Error Handling, Destructuring & Spread in Native ✅

**The core control flow gap closes.** Real "a" programs use `try { }` blocks, the `?` operator, destructuring let/for, and array spread everywhere. This version adds them all to native compilation.

### Completed

- **Try/catch/?**: `try { block }` → C `setjmp`/`longjmp`. `?` postfix checks `is_err`, longjmps on error. Global try stack with `a_try_unwrap()` runtime function.
- **Destructuring**: `let [a, b] = expr`, `let [first, ...rest] = arr` via indexed access and `a_drop`.
- **For destructuring**: `for [k, v] in pairs` via per-iteration element indexing.
- **Spread**: `[...arr1, x, ...arr2]` in array literals via incremental `a_array_push`/`a_concat_arr` chains.

### Deferred to Post-FFI

- **Eval**: Enormous scope (embedding a compiler/interpreter in the runtime). Better built after FFI exists, potentially by shelling out to the native `ac` binary.
- **Structured concurrency**: `spawn`/`await` via pthreads. Benefits from FFI for pthread calls. Planned for v0.47+.

---

## v0.45.1 -- Expression Completeness ✅

**Every expression the VM evaluates, native compiles correctly.** Found and fixed a critical bug: lambda bodies were not returning their last expression value (closures, map, filter, reduce all returned void). Also fixed void-returning builtins, added MatchExpr infrastructure, and BlockExpr trailing Return handling.

### Scope

- **MatchExpr**: Match as an *expression* that returns a value (e.g. `let x = match val { Ok(v) => v, _ => 0 }`). The bytecode compiler handles this via `compile_match_expr`; cgen.a only has statement-form `Match`. Emit using GCC statement expressions `({...})`, reusing `_emit_match` internals but capturing the result from arm bodies.
- **BlockExpr trailing return**: cgen.a only promotes the last statement of a `BlockExpr` when it is `ExprStmt`. The VM also handles trailing `Return`. Fix to match.
- **UnaryOp audit**: Verify all unary operators (`!`, `-`) emit correctly for all operand types.

### Why Before FFI

Programs using FFI will use match expressions for result handling (`let val = match ffi_call() { ... }`). These must work.

---

## v0.45.2 -- Native I/O Primitives ✅

**The native compiler becomes a standalone tool.** The C runtime now includes POSIX I/O, filesystem operations, shell execution, environment access, and a recursive-descent JSON parser. The native binary reads its own source files, compiles itself, and the result is byte-identical across three generations — a fully self-contained bootstrap with no Rust in the loop.

### C Runtime Additions

| Builtin | C Implementation | Priority |
|---------|-----------------|----------|
| `io.read_file(path)` | `fopen`/`fread`/`fclose`, return string or Err | Critical -- cgen.a uses this |
| `io.write_file(path, data)` | `fopen`/`fwrite`/`fclose` | High |
| `fs.ls(path)` | `opendir`/`readdir`/`closedir`, return array of maps | High |
| `fs.mkdir(path)` | `mkdir()` | Medium |
| `fs.cwd()` | `getcwd()` | Medium |
| `exec(cmd)` | `popen`/`pclose`, return stdout as string | Medium |
| `env.get(key)` | `getenv()` | Medium |
| `json.parse(str)` | Minimal recursive-descent JSON parser in C | Medium |

### The Milestone

After v0.45.2, this works -- no Rust VM in the loop:

```bash
./ac std/compiler/cgen.a > ac2.c
gcc ac2.c c_runtime/runtime.c -o ac2 -I c_runtime -lm
./ac2 std/compiler/cgen.a > ac3.c
diff ac2.c ac3.c  # identical -- three-stage bootstrap
```

### Why Before FFI

FFI adds complexity. The native compiler should be self-sufficient for I/O *before* we layer on foreign function calls. Also, the I/O primitives needed here are exactly what v0.48 (Native Stdlib) would later reimplement via FFI -- doing them in C first validates the approach.

---

## v0.45.3 -- Test Hardening ✅

**Confidence before complexity.** Ran the VM test suite through native compilation, built a test harness, and fixed every edge case that surfaced.

### Completed

- **Test harness**: `scripts/native_test.sh` -- compiles test files to C, builds, runs, reports pass/fail. Auto-detects `test_*` functions, generates `main()` wrappers, handles both bool-returning and void (assert-based) tests.
- **7 test suites passing natively** (113 individual tests): `test_patterns` (22), `test_maps` (21), `test_stack_traces` (4), `test_core` (8), `test_destructure` (18), `test_functional` (26), `test_strings` (14).
- **13 example programs** all compiling and running correctly.
- **Three-stage bootstrap** verified: 5,138 lines C, byte-identical across all three stages.

### Bugs Found and Fixed

1. **`else if` codegen was completely broken**: The `ElseIf` AST node uses key `"stmt"` but cgen accessed `"if"`. Every `else if` in the language emitted `/* unhandled stmt: void */`. Fixed in three places.
2. **Index assignment (`m["key"] = val`) was broken**: Emitted `m = val` instead of `m = a_index_set(m, key, val)`. Added `a_index_set` runtime function that dispatches on array/map type.
3. **FieldAccess (`m.name`) had a missing closing paren**: `a_map_get(m, a_string("name")` was missing `)`. Single character fix.
4. **Module import name resolution missing**: `use std.testing` imported functions with prefixed names but call sites used unprefixed names. Added import alias tracking to `emit_program` and call resolution.
5. **Map iteration in for-loops broken**: `for [k,v] in map` needed maps to be auto-converted to entry arrays. Added `a_iterable()` runtime helper.
6. **Missing builtins**: Added `map.delete`, `map.entries`, `map.from_entries`, `str.find`, `str.count` to both C runtime and cgen builtin map.

### Known Limitations

- **TCO tests skipped**: `test_tco.a` uses 100,000-depth recursion which overflows the C stack. Native TCO would require a trampoline or `goto`-based emission. Deferred.
- **Tests requiring VM-only features skipped**: `test_eval`, `test_concurrency`, `test_regex`, `test_hash`, `test_datetime`, `test_bridge`, `test_serialize`, `test_self_compile` (all require eval, spawn, or VM internals).

---

## v0.46 -- C Foreign Function Interface ✅

**Access the entire C ecosystem.** The native compiler can now call arbitrary C functions. This opens SQLite, OpenSSL, libcurl, zlib, POSIX, and every other C library ever written.

### Syntax

```a
extern fn abs(n: i32) -> i32
extern fn atoi(s: str) -> i32
extern fn strlen(s: str) -> i64
extern fn getpid() -> i32

fn main() {
  println(to_str(abs(-42)))       ; 42
  println(to_str(atoi("12345")))  ; 12345
  println(to_str(strlen("hello"))) ; 5
  println(to_str(getpid()))       ; some pid
}
```

### Completed

- **`extern fn` declarations**: New keyword `extern` in both self-hosted lexer (`KwExtern`) and Rust lexer (`TokenKind::Extern`). Parser produces `ExternFn` AST nodes with typed parameters and return type.
- **`ptr` type**: `TAG_PTR` added to ATag enum, `void* pval` in AValue union, `TyPtr` token in lexer, constructors `a_ptr()`, `a_ptr_null()`, `a_is_null()` in C runtime.
- **Type marshalling**: Automatic bidirectional conversion between AValue and C types — `i8`/`i16`/`i32`/`i64`/`u8`-`u64` ↔ `int*_t`, `f32`/`f64` ↔ `float`/`double`, `str` ↔ `const char*`, `bool` ↔ `int`, `ptr` ↔ `void*`, `void`.
- **Shim generation**: Each `extern fn` produces two C declarations — a raw C prototype (`extern int32_t abs(int32_t)`) and an AValue shim (`AValue fn_abs(AValue __p0) { ... }`). Call sites use the shim, so the rest of the call resolution system works unchanged.
- **Rust VM parity**: `ExternFn` variant added to `TopLevelKind`, parsed by the Rust parser, handled as no-op in interpreter/compiler/checker/formatter. The VM can parse files containing `extern fn` without errors.
- **Pointer builtins**: `ptr.null` and `ptr.is_null` registered in cgen builtin map.
- **Test program**: `examples/c_targets/ffi_test.a` exercises `abs`, `atoi`, `strlen`, `getpid` — all passing.
- **Bootstrap**: Three-stage bootstrap verified at 5,415 lines C (up from 5,138), byte-identical across all stages.
- **Full regression suite**: 113 native tests passing, 14 example programs passing.

### Deferred

- **Callback trampolines**: Passing "a" closures as C function pointers (needed for `sqlite3_exec` callbacks). Planned for v0.46.1.
- **Struct layout** (`extern struct`): Direct C struct access. Can use `ptr` + offset math for now. Deferred to v0.47+.
- **Variadic C functions** (`printf`, etc.): Requires special handling. Deferred.
- **`#link` build integration**: Link directives are not yet parsed; library flags are passed to gcc manually. Deferred.

---

## v0.47 -- Memory Architecture ✅

**Production-grade memory management.** Reference counting with ownership semantics, arena allocation infrastructure, and mark-and-sweep GC for cycle collection.

### What Was Delivered

1. **Ownership model**: All expressions produce owned values. Zero-init locals, retain on copy, release at scope exit. Safe assignment protocol prevents use-after-free during reassignment.

2. **Reference counting hardened**: `a_release` guards against double-free (`rc <= 0`). All collection constructors retain stored values. Getters (`a_array_get`, `a_map_get`) return owned references. Runtime helpers release intermediates.

3. **Codegen instrumentation**: `cgen.a` emits `a_retain`/`a_release` calls at assignments, function returns, loop boundaries, and lambda returns. Cleanup blocks at every function exit. Parameters excluded from cleanup (caller-owned).

4. **Escape analysis**: Analysis functions defined (`_ea_collect_idents`, `_escape_analysis`), context slot prepared. Full wiring deferred -- the analysis generates significant temporary data during self-compilation.

5. **Arena allocator**: Runtime API (`a_arena_new`, `a_arena_free`, `a_arena_alloc`, `a_arena_save`, `a_arena_restore`). Codegen integration deferred until escape analysis is wired.

6. **Mark-and-sweep GC**: `GCNode` tracking list, root shadow stack, recursive mark traversal, sweep with adaptive threshold. Opt-in API (`a_gc_push_root`, `a_gc_pop_roots`, `a_gc_collect`). Not wired into constructors -- RC handles the common case.

### Verification

- Three-stage bootstrap: 12,300 lines C (up from 5,415), byte-identical
- 15 example programs passing, 4 memory-specific tests clean under AddressSanitizer
- Self-compilation: ~3.9s native
- C runtime: ~1,700 lines. cgen.a: ~1,830 lines

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
| **v0.43** ✅ | Closures in Native | Lambda lifting, HOF builtins, pipes | Functional programming natively |
| **v0.44** ✅ | Pattern Matching in Native | match/guard/destructure in C | Complex dispatch natively |
| **v0.45** ✅ | Error Handling & Destructuring | try/catch/?, let/for destructure, spread | Real programs compile natively |
| **v0.45.1** ✅ | Expression Completeness | Lambda implicit return, BlockExpr, MatchExpr | Correct expression semantics |
| **v0.45.2** ✅ | Native I/O Primitives | read_file, write_file, fs.ls, exec, env, json.parse | Standalone native compiler |
| **v0.45.3** ✅ | Test Hardening | 113 native tests, 6 bugs fixed, bootstrap verified | Confidence before FFI |
| **v0.46** ✅ | C FFI | `extern fn`, ptr type, type marshalling, shim gen | Access entire C ecosystem |
| **v0.47** ✅ | Memory Architecture | RC ownership, arenas, GC | Production-grade native runtime |
| **v0.48** | Native Stdlib | POSIX I/O, HTTP, JSON, hash | No Rust dependency in stdlib |
| **v0.49** | Package Manager | deps, registry, lockfile | Reusable libraries |
| **v0.50** | Language Server | LSP in "a" | IDE intelligence |
| **v0.51** | WebAssembly Target | Compile to WASM | Run in browsers/edge |
| **v0.52** | Final Bootstrap | Full toolchain in "a" | Complete independence |

---

*"The compiler compiled itself. Now it should compile everything."*
