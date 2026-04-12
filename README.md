# a

A programming language by AI, for AI.

Every design choice -- grammar, keywords, type system, builtins -- optimizes for what matters when AI writes and reads code: zero ambiguity, explicit data flow, token efficiency, and batteries included.

## Build

```
cargo build --release
```

## Usage

```
a run program.a          # execute (bytecode VM, fast)
a test tests/test_re.a   # run test_* functions with pass/fail reporting
a interp program.a       # execute (tree-walking interpreter, legacy)
a check program.a        # type check
a fmt program.a          # format
a ast program.a          # dump AST as JSON
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

**85+ builtins** covering everything an agent needs:

| Domain | Operations |
|--------|-----------|
| **Filesystem** | `fs.ls`, `fs.mkdir`, `fs.rm`, `fs.mv`, `fs.cp`, `fs.glob`, `fs.exists`, `fs.is_dir`, `fs.is_file`, `fs.cwd`, `fs.abs`, `io.read_file`, `io.write_file` |
| **HTTP** | `http.get`, `http.post`, `http.put`, `http.patch`, `http.delete` (returns `{status, body, headers}`) |
| **Shell** | `exec(cmd)` returns `{stdout, stderr, code}` |
| **JSON** | `json.parse`, `json.stringify`, `json.pretty` |
| **Strings** | `str.split`, `str.join`, `str.contains`, `str.replace`, `str.trim`, `str.upper`, `str.lower`, `str.starts_with`, `str.ends_with`, `str.chars`, `str.slice`, `str.lines` (14 ops) |
| **Arrays** | `sort`, `reverse_arr`, `contains`, `push`, `slice`, `map`, `filter`, `reduce`, `each`, `len` |
| **Maps** | `map.get`, `map.set`, `map.keys`, `map.values`, `map.has` |
| **Error handling** | `try { }`, `?` operator, `Ok`/`Err` constructors, `unwrap`, `unwrap_or`, `is_ok`, `is_err`, `expect`, pattern matching on Results |
| **Concurrency** | `spawn`, `await`, `await_all`, `parallel_map`, `parallel_each`, `timeout` |
| **Process** | `args()`, `exit(code)`, `eprintln()` |
| **Stdin** | `io.read_stdin()`, `io.read_line()` (pipeline support) |
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
use std.compiler.emitter      # pretty-print ASTs back to "a" source
use std.compiler.serialize    # serialize/deserialize compiled programs
use std.lexer                 # legacy tokenizer
```

## Self-hosting

The compiler is written in "a". It lexes, parses, compiles to bytecode, and emits source -- all in the language itself. The Rust runtime is the bootstrap loader and VM.

**Three layers deep:** `a run examples/boot.a -- examples/boot.a examples/fibonacci.a` runs the Rust VM executing `boot.a`, which interprets `boot.a`, which interprets `fibonacci.a`. Correct output through all three layers.

**Module precompilation:** `use` statements automatically cache compiled bytecode to `.ac` files, making subsequent imports near-instant.

**Metaprogramming toolkit:** `std.meta` provides `parse`, `emit`, `walk`, `search`, `transform`, `analyze`, and `generate` -- full programmatic access to the AST for code generation, refactoring, and analysis.

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
| `examples/site_gen.a` | 99 | static site generator using path, datetime, hash, csv, template, encoding |
| `examples/gen_tests.a` | 46 | metaprogramming: auto-generate test scaffolds from source |

## Stats

| | |
|---|---|
| **Rust runtime** | ~10,000 lines across 8 modules |
| **"a" source** | ~17,700 lines across 76 files |
| **Standard library** | 19 modules, 366 functions, ~7,800 lines |
| **Test suites** | 27 suites, 498 native tests, ~4,200 lines |
| **Examples & tools** | 28 programs, ~5,600 lines |

## Editor support

Syntax highlighting for VS Code / Cursor is included in [`editors/vscode`](editors/vscode). See [REFERENCE.md](REFERENCE.md) for the complete language reference.
