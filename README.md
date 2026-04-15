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

**Requirements:** gcc (or any C99 compiler). That's it. Supported platforms: Linux x86_64, Linux arm64, macOS arm64.

Pre-built binaries are available on the [Releases](../../releases) page.

```bash
./build.sh                  # gcc bootstrap + self-host (no Rust, no cargo)
```

The build bootstraps from pre-generated C (`bootstrap/cli.c`), then self-hosts: `./a` compiles its own source to produce the final binary. No Rust or cargo required.

## Usage

```
./a run program.a           # compile and run (cached -- instant on repeat)
./a build program.a         # compile to native binary
./a build program.a -o out  # compile to native binary with custom name
./a eval "2 + 3"            # evaluate an expression
./a repl                    # interactive read-eval-print loop
./a cc program.a            # emit C to stdout
./a fmt program.a           # format source code
./a fmt src/                # format all .a files in directory
./a ast program.a           # dump parsed AST as JSON
./a check program.a         # static analysis (undefined vars, arity, unused)
./a test tests/native/      # find test_*.a files, compile, run, report
./a lsp                     # build the language server binary (./a-lsp)
./a cache clean             # clear the compilation cache
./a pkg init                # create pkg.toml manifest
./a pkg add name source     # add a dependency
./a pkg install             # install all dependencies
./a-lsp                     # start language server (stdio, for editors)

# Shebang support
#!/usr/bin/env a run        # add to top of .a file, chmod +x, run directly
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

**145+ builtins** covering everything an agent needs (plus native compilation to C):

| Domain | Operations |
|--------|-----------|
| **Filesystem** | `fs.ls`, `fs.mkdir`, `fs.rm`, `fs.mv`, `fs.cp`, `fs.glob`, `fs.exists`, `fs.is_dir`, `fs.is_file`, `fs.cwd`, `fs.abs`, `io.read_file`, `io.write_file` |
| **HTTP client** | `http.get`, `http.post`, `http.put`, `http.patch`, `http.delete` (returns `{status, body, headers}`) -- in-process via POSIX sockets + platform TLS (macOS SecureTransport, Linux OpenSSL) |
| **HTTP streaming** | `http.stream(url, body, headers)`, `http.stream_read(h)`, `http.stream_close(h)` -- incremental line-by-line response reading for SSE/streaming APIs |
| **WebSocket** | `ws.connect(url)`, `ws.send(h, msg)`, `ws.recv(h)`, `ws.close(h)` -- RFC 6455 client with masking, ping/pong, ws:// and wss:// |
| **HTTP server** | `http.serve(port, handler)` -- POSIX sockets, closure handler receives `{method, path, headers, body}`, returns `{status, headers, body}`; `http.serve_static(port, dir)` for file serving |
| **Database** | `db.open(path)` (or `":memory:"`), `db.exec(db, sql)`, `db.query(db, sql, params)` with `?` binding, `db.close(db)` -- bundled SQLite, zero setup |
| **Shell** | `exec(cmd)` returns `{stdout, stderr, code}` |
| **Subprocess** | `proc.spawn(cmd)`, `proc.write(h, data)`, `proc.read_line(h)`, `proc.kill(h)`, `proc.wait(h)`, `proc.is_running(h)` -- bidirectional pipe I/O with lifecycle management |
| **MCP** | `mcp.server`, `mcp.add_tool`, `mcp.add_resource`, `mcp.serve` (server); `mcp.connect`, `mcp.list_tools`, `mcp.call_tool`, `mcp.close` (client) -- JSON-RPC 2.0 over stdio (via stdlib) |
| **JSON** | `json.parse`, `json.stringify`, `json.pretty` |
| **Data formats** | `yaml.parse`/`stringify`, `toml.parse`/`stringify`, `html.parse`/`select`/`text`, `url.parse`/`build`/`encode`/`decode` (via stdlib modules) |
| **LLM client** | `llm.chat(provider, model, messages, options)` -- unified API for OpenAI, Anthropic, Google AI with retry, tool use, normalized responses; `llm.stream(provider, model, messages, on_chunk, options)` for token-by-token streaming (via stdlib) |
| **Strings** | `str.split`, `str.join`, `str.contains`, `str.replace`, `str.trim`, `str.upper`, `str.lower`, `str.starts_with`, `str.ends_with`, `str.chars`, `str.slice`, `str.lines` (14 ops) |
| **Arrays** | `sort`, `reverse_arr`, `contains`, `push`, `slice`, `map`, `filter`, `reduce`, `each`, `sort_by`, `find`, `any`, `all`, `flat_map`, `min_by`, `max_by`, `enumerate`, `zip`, `take`, `drop`, `unique`, `chunk`, `len` |
| **Maps** | `map.get`, `map.set`, `map.keys`, `map.values`, `map.has` |
| **Error handling** | `try { }`, `?` operator, `Ok`/`Err` constructors, `unwrap`, `unwrap_or`, `is_ok`, `is_err`, `expect`, pattern matching on Results |
| **Concurrency** | `spawn`, `await`, `await_all`, `parallel_map`, `parallel_each`, `timeout` |
| **Process** | `args()`, `exit(code)`, `eprintln()` |
| **Stdin** | `io.read_stdin()`, `io.read_line()`, `io.read_bytes(n)`, `io.flush()` |
| **Environment** | `env.get`, `env.set`, `env.all` |
| **Compression** | `compress.deflate`, `compress.inflate` (raw deflate), `compress.gzip`, `compress.gunzip` (gzip format) -- via bundled miniz |
| **UUID** | `uuid.v4()` -- cryptographic random UUID v4 generation via `/dev/urandom` |
| **Signals** | `signal.on(name, handler)` -- register handlers for SIGINT, SIGTERM, SIGHUP, SIGUSR1, SIGUSR2 (native CLI only) |
| **Image** | `image.load(path)`, `image.decode(bytes)`, `image.encode(image, fmt)`, `image.save(image, path)`, `image.width`, `image.height`, `image.resize(image, w, h)`, `image.pixels(image)` -- PNG/JPEG/BMP/GIF decode, PNG/BMP/JPEG encode, bilinear resize (native CLI only, via bundled stb_image) |
| **Introspection** | `type_of`, `int`, `float`, `to_str`, `char_code`, `from_code`, `is_alpha`, `is_digit`, `is_alnum` |

**Standard library** with 33 modules:

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
use std.yaml                  # parse, stringify -- YAML 1.2 subset (mappings, sequences, scalars)
use std.toml                  # parse, stringify -- TOML (tables, arrays of tables, all value types)
use std.html                  # parse, select, text -- HTML DOM tree with CSS selector queries
use std.url                   # parse, encode, decode, build -- full URL structure parsing
use std.llm                   # chat(provider, model, msgs, opts) -- unified LLM client (OpenAI, Anthropic, Google)
use std.mcp                   # MCP server + client -- JSON-RPC 2.0 over stdio, tool/resource registration
use std.agent                 # retry, batch, pipeline, timeout, rate_limit -- operational primitives
use std.log                   # info, warn, error, debug, set_level -- structured JSON logging to stderr
use std.uuid                  # v4() -- UUID generation
use std.args                  # spec, flag, option, positional, parse -- declarative CLI argument parsing
use std.schema                # validate(value, schema), from_type -- JSON Schema draft-07 validation
use std.diff                  # text, lines, patch -- Myers algorithm unified diff, structured ops, patch
use std.config                # load, from_env, dotenv, merge, require -- layered configuration loading
use std.migrate               # run, status, create -- SQLite migration runner
use std.semver                # parse, compare, satisfies, format, best_match -- semantic versioning
use std.pkg                   # init, add_dep, install, read_manifest -- package manager
use std.template              # render(template, vars) with {{var}}, {{#if}}, {{#each}}
use std.compiler.lexer        # tokenize "a" source into token arrays
use std.compiler.parser       # parse token arrays into tagged-map ASTs
use std.compiler.ast          # AST node constructors and accessors
use std.compiler.compiler     # compile ASTs to bytecode
use std.compiler.cgen          # compile ASTs to C source code (native compilation)
use std.compiler.emitter      # pretty-print ASTs back to "a" source
use std.compiler.checker      # static analysis: undefined vars, arity, unused, unreachable
use std.compiler.serialize    # serialize/deserialize compiled programs
use std.codegen               # compile_check, run_in_sandbox, test, generate -- self-improvement loop
use std.refactor              # rename, extract_fn, inline_fn -- AST-level refactoring
use std.lexer                 # legacy tokenizer
```

## Self-hosting

The "a" compiler and CLI are fully self-hosting. The native `./a` binary compiles its own source code to produce a working copy of itself:

```bash
./build.sh                            # gcc bootstrap -> self-host -> done
./a build src/cli.a -o a2             # self-hosting: ./a compiles itself
./a2 build src/cli.a -o a3            # a2 compiles itself too
./a3 run examples/hello.a            # a3 works
```

The entire language bootstraps from a single C compiler. `build.sh` compiles the pre-generated `bootstrap/cli.c` with gcc, then uses the resulting binary to recompile itself from `src/cli.a`. No Rust, no cargo, no external tools. The C code generator compiles itself -- including the lexer, parser, checker, and AST modules -- into ~12,600 lines of C with reference-counted ownership, goto-based cleanup epilogues, and 160+ native builtins. All 33 standard library modules compile natively. Closures, lambdas, HOFs, pattern matching, try/catch, destructuring, I/O, module imports, the pipe operator, C FFI (`extern fn`), memory management, SHA-256/MD5 hashing, HTTP client, JSON stringify, compression (deflate/gzip), subprocess pipes, image processing, package management, static analysis, fork-based concurrency (`spawn`/`await`/`parallel_map`/`timeout`), self-improvement loop (`codegen`/`refactor`), and POSIX time/fs/env all compile natively. Clean under AddressSanitizer.

**Fixed point reached:** the native compiler compiles its own source and produces byte-identical output. The language exists independently.

**Module precompilation:** `use` statements automatically cache compiled bytecode to `.ac` files, making subsequent imports near-instant.

**Metaprogramming toolkit:** `std.meta` provides `parse`, `emit`, `walk`, `search`, `transform`, `analyze`, and `generate` -- full programmatic access to the AST for code generation, refactoring, and analysis.

## Native compilation

"a" programs compile to native executables through C code generation. The code generator (`std/compiler/cgen.a`) is written entirely in "a" -- it uses the self-hosted parser to produce an AST, walks it, and emits equivalent C with automatic memory management. All values are reference-counted with ownership semantics: zero-initialized locals, retain on copy, release at scope exit via goto-based cleanup epilogues (single cleanup label per function, 44% code reduction vs inline release). Lambdas are lifted to top-level C functions with captured environment arrays. Error handling uses `setjmp`/`longjmp` for `try`/`?` semantics with correct tail-expression capture. C FFI via `extern fn` declarations generates type-marshalling shim wrappers automatically. The C runtime (~3,900 lines) includes POSIX I/O, filesystem ops, shell execution, subprocess pipes, fork-based concurrency (spawn/await/parallel_map/timeout), JSON parse/stringify, SHA-256/MD5 hashing, HTTP/1.1 client (in-process POSIX sockets + platform TLS), HTTP streaming, WebSocket client (RFC 6455), HTTP server (POSIX sockets), SQLite (bundled amalgamation), zlib-compatible compression (bundled miniz), POSIX time, environment management, arena allocator, and mark-and-sweep GC.

**164x faster:** fib(35) runs in 0.17s native vs 28s on the bytecode VM.

## Concurrency

Structured concurrency with isolated task parallelism. No shared mutable state -- each spawned task runs in complete isolation (own VM thread in the bytecode path, fork'd process in the native path):

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
| `examples/api.a` | 20 | **JSON API server** -- HTTP server with routing, JSON responses, echo endpoint |
| `examples/crud.a` | 40 | **CRUD API** -- HTTP + SQLite with parameterized queries, create/read/delete users |
| `examples/test_formats.a` | 115 | tests for YAML, TOML, HTML, URL modules -- round-trip, selectors, edge cases |
| `examples/chat.a` | 20 | **LLM chat** -- one-shot conversation with any provider in ~15 lines |
| `examples/stream_chat.a` | 25 | **streaming LLM** -- token-by-token streaming chat with live output |
| `examples/mcp_server.a` | 33 | **MCP server** -- file search tool with tool + resource registration, JSON-RPC stdio |
| `examples/mcp_client.a` | 55 | **MCP client** -- connects to any MCP server, lists tools, calls first tool |
| `examples/cli_demo.a` | 27 | **CLI parsing** -- declarative argument parsing with flags, options, positionals, auto-help |
| `examples/code_review.a` | 40 | **code review** -- diff two files with colored add/remove output using Myers algorithm |
| `examples/config_demo.a` | 38 | **config loading** -- TOML/dotenv/env-prefix config with deep merge and required keys |
| `examples/migrate_demo.a` | 35 | **DB migrations** -- SQLite migration runner with create, run, status, idempotent re-run |
| `examples/image_demo.a` | 50 | **image processing** -- create gradient, resize, save PNG, extract pixel data, encode/decode in memory |
| `examples/pkg_demo.a` | 40 | **package manager** -- init project, parse source strings, semver constraint matching |
| `examples/agent.a` | 60 | **agentic loop** -- tool-using LLM agent: define tools, handle calls, iterate |
| `examples/test_llm.a` | 130 | tests for LLM module internals -- request building, response parsing, tool calls |
| `examples/gen_tests.a` | 46 | metaprogramming: auto-generate test scaffolds from source |
| `examples/self_extend.a` | ~130 | **self-improvement loop** -- analyze, compile-check, sandbox, test, refactor, LLM generate |
| `src/cli.a` | ~500 | **native CLI driver** -- `run`, `build`, `cc`, `fmt`, `ast`, `check`, `repl`, `test`, `lsp`, `pkg` subcommands; self-hosting (compiles itself) |
| `src/lsp.a` | ~1,100 | **language server** -- LSP over stdio with diagnostics, completion, hover, go-to-definition, semantic tokens, rename, code actions, workspace symbols |
| `std/compiler/cgen.a` | ~1,870 | **C code generator** -- self-hosting native compiler via C with memory management (closures, HOFs, pipes, try/catch, destructuring, spread, pattern matching, I/O, module inlining, import aliases, retain/release, goto cleanup, 105+ builtins, three-stage bootstrap) |

## Stats

| | |
|---|---|
| **C runtime** | ~4,100 lines (runtime.h + runtime.c) + bundled SQLite3, miniz, stb_image |
| **"a" source** | ~21,000 lines across 110+ files |
| **Standard library** | 33 modules, 510+ functions, ~10,600 lines |
| **Test suites** | 33 suites + cgen test script, 700+ native tests, ~6,000 lines |
| **Examples & tools** | 39 programs, ~6,600 lines |

## Editor support

**Language server:** `./a-lsp` is a native LSP server written in "a" itself. It provides:

- **Diagnostics** -- parse errors on every keystroke (red squiggles)
- **Completion** -- 145+ builtins with signatures, keywords, user functions, 20+ stdlib modules
- **Hover** -- function signatures for builtins and user-defined functions
- **Go-to-definition** -- in-file and cross-module (resolves `use` imports to source files)
- **Semantic tokens** -- syntax-aware token classification (keywords, functions, variables, strings, etc.)
- **Rename** -- rename symbols across the document
- **Code actions** -- "Add missing `use`" quick-fix for unresolved module references
- **Workspace symbols** -- search `fn` and `type` declarations across all workspace files

Build with `./build.sh` or `./a lsp`, then configure your editor to run `./a-lsp` as the language server for `*.a` files.

Syntax highlighting for VS Code / Cursor is included in [`editors/vscode`](editors/vscode). See [REFERENCE.md](REFERENCE.md) for the complete language reference.
