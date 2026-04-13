# a

A programming language by AI, for AI.

Every design choice -- grammar, keywords, type system, builtins -- optimizes for what matters when AI writes and reads code: zero ambiguity, explicit data flow, token efficiency, and batteries included.

## Quick Start

```bash
./build.sh                  # bootstrap the native CLI + language server
./a run program.a           # compile and run in one step
./a build program.a -o out  # compile to native binary
./a test tests/native/      # run test suite
./a cc program.a            # emit C to stdout
./a-lsp                     # language server (JSON-RPC over stdio)
```

## Build

**Requirements:** gcc (or any C99 compiler), curl (for HTTP builtins). Supported platforms: Linux x86_64, Linux arm64, macOS arm64.

Pre-built binaries are available on the [Releases](../../releases) page.

```bash
# Option 1: Native CLI (recommended)
./build.sh                  # builds ./a from source via the VM

# Option 2: Rust VM only
cargo build --release
```

## Usage

```
# Native CLI (fast, self-contained)
./a run program.a           # compile to C, build, execute, clean up
./a build program.a         # compile to native binary
./a build program.a -o out  # compile to native binary with custom name
./a cc program.a            # emit C to stdout
./a test tests/native/      # find test_*.a files, compile, run, report
./a lsp                     # build the language server binary (./a-lsp)
./a-lsp                     # start language server (stdio, for editors)

# Rust VM
a run program.a             # execute (bytecode VM)
a test tests/test_re.a      # run test_* functions with pass/fail reporting
a check program.a           # type check
a fmt program.a             # format
a ast program.a             # dump AST as JSON
```

## The language in 60 seconds

```
use std.strings

ty DirEntry = {name: str, is_dir: bool}

fn find_files(dir: str, ext: str) -> [str] effects [io] {
  let entries: [DirEntry] = fs.ls(dir)
  let mut out = []
  for e in entries {
    let path = str.concat(str.concat(dir, "/"), e.name)
    if e.is_dir {
      for f in find_files(path, ext) {
        out = push(out, f)
      }
    } else {
      if str.ends_with(e.name, ext) {
        out = push(out, path)
      }
    }
  }
  ret out
}

fn main() -> void effects [io] {
  let files = sort(find_files(".", ".a"))
  println("found {len(files)} files")
  for f in files {
    let result = try { io.read_file(f)? }
    if is_ok(result) {
      let lines = len(str.lines(unwrap(result)))
      let short = str.replace(f, "./", "")
      println("  {strings.pad_right(short, 30, \" \")} {lines} lines")
    }
  }
}
```

This is real code. It runs. It recursively walks a directory, reads files, counts lines, and prints a formatted report.

## What it does

**100+ builtins** covering everything an agent needs (plus native compilation to C):

| Domain | Operations |
|--------|-----------|
| **Filesystem** | `fs.ls`, `fs.mkdir`, `fs.rm`, `fs.mv`, `fs.cp`, `fs.glob`, `fs.exists`, `fs.is_dir`, `fs.is_file`, `fs.cwd`, `fs.abs`, `io.read_file`, `io.write_file` |
| **HTTP** | `http.get`, `http.post`, `http.put`, `http.patch`, `http.delete` (returns `{status, body, headers}`) |
| **Shell** | `exec(cmd)` returns `{stdout, stderr, code}` |
| **JSON** | `json.parse`, `json.stringify`, `json.pretty` |
| **Strings** | `str.split`, `str.join`, `str.contains`, `str.replace`, `str.trim`, `str.upper`, `str.lower`, `str.starts_with`, `str.ends_with`, `str.chars`, `str.slice`, `str.lines` (14 ops) |
| **Arrays** | `sort`, `reverse_arr`, `contains`, `push`, `slice`, `map`, `filter`, `reduce`, `each`, `sort_by`, `find`, `any`, `all`, `flat_map`, `min_by`, `max_by`, `enumerate`, `zip`, `take`, `drop`, `unique`, `chunk`, `len` |
| **Maps** | `map.get`, `map.set`, `map.keys`, `map.values`, `map.has` |
| **Error handling** | `try { }`, `?` operator, `Ok`/`Err` constructors, `unwrap`, `unwrap_or`, `is_ok`, `is_err`, `expect`, pattern matching on Results |
| **Concurrency** | `spawn`, `await`, `await_all`, `parallel_map`, `parallel_each`, `timeout` |
| **Process** | `args()`, `exit(code)`, `eprintln()` |
| **Stdin** | `io.read_stdin()`, `io.read_line()`, `io.read_bytes(n)`, `io.flush()` |
| **Environment** | `env.get`, `env.set`, `env.all` |
| **Introspection** | `type_of`, `int`, `float`, `to_str`, `char_code`, `from_code`, `is_alpha`, `is_digit`, `is_alnum` |

**Standard library** with 19 modules:

```
use std.math                  # max, min, clamp, pow, sum, range
use std.strings               # repeat, pad_left, pad_right, reverse, center
use std.testing               # assert_eq, assert_true, report
use std.cli                   # red, green, yellow, bold, dim (ANSI colors)
use std.re                    # match_full, test, search, find_all, replace, split
use std.meta                  # parse, emit, walk, search, transform, analyze, generate
use std.path                  # join, dirname, basename, extension, stem, normalize
use std.datetime              # now, timestamp, format, iso, add_days, diff_ms
use std.hash                  # sha256, md5, sha256_file, quick
use std.encoding              # base64_encode, base64_decode, hex, url_encode
use std.csv                   # parse, parse_records, stringify, escape_field
use std.template              # render(template, vars) with {{var}}, {{#if}}, {{#each}}
use std.compiler.lexer        # tokenize "a" source into token arrays
use std.compiler.parser       # parse token arrays into tagged-map ASTs
use std.compiler.ast          # AST node constructors and accessors
use std.compiler.compiler     # compile ASTs to bytecode
use std.compiler.cgen          # compile ASTs to C source code (native compilation)
use std.compiler.emitter      # pretty-print ASTs back to "a" source
use std.compiler.serialize    # serialize/deserialize compiled programs
use std.lexer                 # legacy tokenizer
```

## Self-hosting

The "a" compiler and CLI are fully self-hosting. The native `./a` binary compiles its own source code to produce a working copy of itself:

```bash
./build.sh                            # bootstrap: VM -> C -> native ./a
./a build src/cli.a -o a2             # self-hosting: ./a compiles itself
./a2 build src/cli.a -o a3            # a2 compiles itself too
./a3 run examples/hello.a            # a3 works
```

The C code generator compiles itself -- including the lexer, parser, and AST modules -- into ~7,700 lines of C with reference-counted ownership, goto-based cleanup epilogues, and 105+ native builtins. gcc compiles that C into a freestanding native binary with **zero Rust dependency**. All 12 standard library modules compile natively. Closures, lambdas, HOFs, pattern matching, try/catch, destructuring, I/O, module imports, the pipe operator, C FFI (`extern fn`), memory management, SHA-256/MD5 hashing, HTTP client, JSON stringify, and POSIX time/fs/env all compile natively. Clean under AddressSanitizer.

**Fixed point reached:** the native compiler compiles its own source and produces byte-identical output. The language exists independently.

**Module precompilation:** `use` statements automatically cache compiled bytecode to `.ac` files, making subsequent imports near-instant.

**Metaprogramming toolkit:** `std.meta` provides `parse`, `emit`, `walk`, `search`, `transform`, `analyze`, and `generate` -- full programmatic access to the AST for code generation, refactoring, and analysis.

## Native compilation

"a" programs compile to native executables through C code generation. The code generator (`std/compiler/cgen.a`) is written entirely in "a" -- it uses the self-hosted parser to produce an AST, walks it, and emits equivalent C with automatic memory management. All values are reference-counted with ownership semantics: zero-initialized locals, retain on copy, release at scope exit via goto-based cleanup epilogues (single cleanup label per function, 44% code reduction vs inline release). Lambdas are lifted to top-level C functions with captured environment arrays. Error handling uses `setjmp`/`longjmp` for `try`/`?` semantics with correct tail-expression capture. C FFI via `extern fn` declarations generates type-marshalling shim wrappers automatically. The C runtime (~2,200 lines) includes POSIX I/O, filesystem ops, shell execution, JSON parse/stringify, SHA-256/MD5 hashing, HTTP client (via curl), POSIX time, environment management, arena allocator, and mark-and-sweep GC.

**164x faster:** fib(35) runs in 0.17s native vs 28s on the bytecode VM.

## Concurrency

Structured concurrency with isolated task parallelism. No shared mutable state -- each spawned task runs in its own VM with a copy of the program:

```
fn fib(n: int) -> int {
  if n < 2 { ret n }
  ret fib(n - 1) + fib(n - 2)
}

fn main() -> void {
  let results = parallel_map([35, 36, 37, 38], fn(n) { ret fib(n) })
  println("results: {results}")

  let h = spawn(fn() { ret fib(40) })
  let val = await(h)
  println("fib(40) = {val}")

  let result = timeout(1000, fn() { ret fib(30) })
  println("with timeout: {result}")
}
```

## Design for machines

| Principle | Implementation |
|-----------|---------------|
| **Unambiguous grammar** | LL(1), context-free, explicit delimiters, no operator precedence surprises |
| **Explicit where it matters** | Effect system (`effects [io]`), function signatures, pre/post contracts; type inference handles the rest |
| **Token efficient** | 2-3 char keywords (`fn`, `ty`, `let`, `ret`, `mut`), type inference, multi-line expressions |
| **Local reasoning** | Module functions are self-contained, complete signatures, no implicit imports |
| **Structured errors** | All errors are JSON: `{"kind":"TypeError","message":"...","span":{"line":5,"col":3}}` |
| **Canonical formatting** | `a fmt` produces one true form, no style debates |

## Tools written in "a"

| Tool | Lines | Description |
|------|-------|-------------|
| `examples/eval.a` | 1036 | **full self-interpreter** with imports, interpolation, module loading |
| `examples/boot.a` | 686 | **bootstrap interpreter** -- "a" interpreting itself interpreting itself |
| `examples/inspect.a` | 449 | **code intelligence** -- call graphs, dead code, complexity |
| `examples/self_lexer.a` | 420 | tokenizer for "a", written in "a" |
| `examples/parse_file.a` | 382 | recursive descent parser for "a" |
| `examples/lines.a` | 263 | universal line processor for Unix pipelines |
| `examples/project_stats.a` | 199 | codebase analyzer with composition charts |
| `examples/todo_scan.a` | 145 | finds TODO/FIXME/HACK annotations across a codebase |
| `examples/search.a` | 105 | regex-powered file search with colored output |
| `examples/resilient.a` | 91 | error-resilient HTTP workflow with retries |
| `examples/api_workflow.a` | 61 | multi-step API orchestration |
| `examples/parallel_fetch.a` | 54 | concurrent URL fetching with timeouts |
| `examples/site_gen.a` | 100 | static site generator using path, datetime, hash, csv, template, encoding (runs natively) |
| `examples/gen_tests.a` | 46 | metaprogramming: auto-generate test scaffolds from source |
| `src/cli.a` | ~175 | **native CLI driver** -- `run`, `build`, `cc`, `test`, `lsp` subcommands; self-hosting (compiles itself) |
| `src/lsp.a` | ~720 | **language server** -- LSP over stdio with diagnostics, completion (105+ builtins), hover, go-to-definition (cross-module) |
| `std/compiler/cgen.a` | ~1,870 | **C code generator** -- self-hosting native compiler via C with memory management (closures, HOFs, pipes, try/catch, destructuring, spread, pattern matching, I/O, module inlining, import aliases, retain/release, goto cleanup, 105+ builtins, three-stage bootstrap) |

## Stats

| | |
|---|---|
| **Rust runtime** | ~10,000 lines across 8 modules |
| **C runtime** | ~1,700 lines (runtime.h + runtime.c) |
| **"a" source** | ~19,000 lines across 88 files |
| **Standard library** | 20 modules, 380+ functions, ~8,200 lines |
| **Test suites** | 27 suites + cgen test script, 498+ native tests, ~4,200 lines |
| **Examples & tools** | 35 programs, ~6,100 lines |

## Editor support

**Language server:** `./a-lsp` is a native LSP server written in "a" itself. It provides:

- **Diagnostics** -- parse errors on every keystroke (red squiggles)
- **Completion** -- 105+ builtins with signatures, keywords, user functions, stdlib modules
- **Hover** -- function signatures for builtins and user-defined functions
- **Go-to-definition** -- in-file and cross-module (resolves `use` imports to source files)

Build with `./build.sh` or `./a lsp`, then configure your editor to run `./a-lsp` as the language server for `*.a` files.

Syntax highlighting for VS Code / Cursor is included in [`editors/vscode`](editors/vscode). See [REFERENCE.md](REFERENCE.md) for the complete language reference.
