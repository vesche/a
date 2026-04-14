# The Hermetic Agent: v0.57 to v1.0

*One binary. No dependencies. Build anything.*

Supersedes [ROADMAP-v0.51-to-v1.0.md](ROADMAP-v0.51-to-v1.0.md), which covers v0.51 through v0.56 (all completed).

## Where We Are

At v0.56, "a" can do almost everything an AI agent needs: serve HTTP, query databases, parse any data format, call any LLM. The native binary compiles itself, runs on Linux and macOS, caches builds, and ships with a language server. Twenty stdlib modules. Over 100 builtins. Self-hosting.

But there are two cracks.

**Crack 1: curl.** The native binary shells out to `curl` for HTTP. That one external dependency undermines the entire "batteries included" promise. A container without `curl` breaks silently. The native path is missing HTTP methods and response headers that the VM provides.

**Crack 2: Rust.** Building the native binary from a clean checkout requires `cargo run` -- the Rust toolchain. `build.sh` uses the Rust VM to run `cgen.a` on `cli.a` to produce the initial C code, then gcc compiles it. Once `./a` exists it can rebuild itself without Rust, but that first bootstrap step still needs it. The Rust binary also provides `check`, `fmt`, `ast`, and `interp` subcommands that the native CLI doesn't have yet. And the integration test suite (~500 tests) runs through `cargo test`.

The language is self-hosting in principle but not in practice. The roadmap below seals both cracks.

## The Principles

1. **Every capability must live inside the binary.** No fork/exec to external tools. No dynamic library requirements beyond libc and the platform TLS stack. If an agent needs it, the binary provides it.

2. **The language must build itself from C alone.** `gcc` and a C runtime is all that should be required. Rust was scaffolding. The language has outgrown it.

3. **The language is a tool for building tools.** Every design decision should ask: "Does this make it easier for me to write the next program?" Not just any program -- the programs I actually write: CLI tools, API integrations, data pipelines, code generators, agent loops.

## The Rust Transition

Today the Rust codebase provides:

| Capability | Rust | Native ("a") | Status |
|-----------|------|---------------|--------|
| Parse + compile + run | `cargo run -- run` | `./a run` | **Parity** |
| C codegen | (via VM running cgen.a) | `./a cc` | **Parity** |
| Build native binary | (via VM) | `./a build` | **Parity** |
| Type checker | `cargo run -- check` | -- | **Rust only** |
| Formatter | `cargo run -- fmt` | -- | **Rust only** |
| AST dump | `cargo run -- ast` | -- | **Rust only** |
| Tree-walk interpreter | `cargo run -- interp` | -- | **Rust only** (legacy) |
| Bytecode compile (.ac) | `cargo run -- compile` | -- | **Rust only** |
| Test runner | `cargo test` (500+ tests) | `./a test dir/` | **Different scope** |
| First bootstrap | `build.sh` needs `cargo run` | -- | **Rust required** |
| HTTP client | `ureq` (in-process, TLS) | `fork`/`exec` curl | **Rust superior** |

The transition plan:

1. **v0.57**: Commit pre-generated bootstrap C (`bootstrap/cli.c`). Now `gcc bootstrap/cli.c runtime.c sqlite3.c -o a` works from a clean checkout. Rust is no longer required to build.
2. **v0.58**: In-process HTTP replaces curl. Native HTTP reaches parity with Rust VM.
3. **v0.65**: Port `fmt`, `check`, `ast` to native "a". All Rust CLI subcommands have native equivalents.
4. **v1.0**: Rust becomes a development tool only (for running the legacy test suite). Not required for any user workflow.

The Rust code is never deleted -- it remains as a reference implementation and regression oracle. But it stops being a dependency.

## Vendored C Library Policy

"a" bundles third-party C libraries directly in `c_runtime/`. This is the SQLite amalgamation model: drop the source files in the tree, compile them as part of the binary, ship everything as one unit.

### The rules

1. **Manifest**: Every vendored library is documented in [`c_runtime/VENDORS.md`](../c_runtime/VENDORS.md) with: name, version, upstream URL, license, SHA-256 checksums of vendored files, date added.
2. **Verification**: `scripts/vendor_check.sh` verifies checksums against the manifest. This runs in CI on every push.
3. **Updates**: `scripts/vendor_update.sh` downloads a new version, replaces the files, updates the manifest, and prints a summary. All updates require the full test suite to pass before merge.
4. **Security policy**: CVE-level patches are applied immediately at any version boundary. Feature updates happen at milestone boundaries.
5. **License compatibility**: Only MIT, Apache 2.0, public domain, or zlib-licensed libraries. No GPL.
6. **Size budget**: Each vendored library must justify its inclusion. The bar is: "Would an AI agent need this capability in >10% of projects?"

### Currently vendored

| Library | Version | Files | Lines | License |
|---------|---------|-------|-------|---------|
| SQLite | 3.48.0 | `sqlite3.c`, `sqlite3.h` | ~274k | Public domain |

### Planned additions

| Library | Version | Files | ~Lines | License | Milestone |
|---------|---------|-------|--------|---------|-----------|
| miniz | 3.1.x | `miniz.c`, `miniz.h` | ~6k | MIT | v0.57 |
| stb_image | 2.30 | `stb_image.h` | ~8k | Public domain | v0.64 |
| stb_image_write | 1.16 | `stb_image_write.h` | ~2k | Public domain | v0.64 |

---

## Phase 1: Hermetic Binary (v0.57 -- v0.58)

The native binary becomes truly self-contained. No external tools required.

### v0.57 -- Bootstrap Independence + Vendoring + Compression

Remove the Rust requirement for first build. Establish vendoring discipline. Add compression.

#### Bootstrap from C

The single highest-impact change: commit the pre-generated C output so the language can build itself from just `gcc`.

- **`bootstrap/cli.c`**: Pre-generated C from `./a cc src/cli.a`. This is the output of the self-hosted compiler -- checked into the repo and verified by CI.
- **`bootstrap/build.sh`**: One-command build: `gcc bootstrap/cli.c c_runtime/runtime.c c_runtime/sqlite3.c -o a -I c_runtime -lm -O2 $SQLITE_FLAGS`. No Rust. No cargo. Just gcc.
- **CI verification**: After every change to `src/cli.a` or the compiler, CI regenerates `bootstrap/cli.c` and verifies it matches what's committed. If it drifts, the build fails.
- **README update**: "Requirements: gcc" -- not "gcc, curl, Rust".

This means anyone -- human or AI -- can build the language from a clean checkout with nothing but a C compiler.

#### Vendoring + Compression

- **`c_runtime/VENDORS.md`**: Manifest documenting SQLite 3.48.0 (retroactive) with checksums.
- **`scripts/vendor_check.sh`**: Verify vendored file checksums match the manifest. Add to CI.
- **`c_runtime/miniz.c` + `c_runtime/miniz.h`**: Vendor miniz 3.1.x for zlib-compatible compression.
- **Builtins**: `compress.deflate(str) -> str`, `compress.inflate(str) -> str`, `compress.gzip(str) -> str`, `compress.gunzip(str) -> str` in the C runtime.
- **Builtin mapping** in cgen.a for native compilation.
- **Rust VM parity**: Same builtins available in the bytecode VM.
- **`build.sh` / CI**: Compile miniz alongside runtime.c and sqlite3.c.
- Bump to `0.57.0`.

### v0.58 -- In-Process HTTP Client

Replace the `fork`/`execvp("curl", ...)` HTTP client with an in-process implementation. This is the most impactful single change in the roadmap.

#### TLS strategy

Use **platform TLS** rather than vendoring a TLS library:
- **macOS**: `Security.framework` (SecureTransport / Network.framework) -- available on every Mac, no additional dependencies.
- **Linux**: `libssl` via `dlopen` at runtime -- OpenSSL/LibreSSL is present on virtually every Linux system. If absent, HTTP still works (just not HTTPS), with a clear error message.

This avoids vendoring 100k-200k+ lines of TLS code. The tradeoff is platform-specific code paths (~200-300 lines each), but we already have platform detection for linker flags. The binary links only against libc and the platform crypto library.

#### Deliverables

- **In-process HTTP/1.1 client** in `runtime.c` (~500-800 lines):
  - POSIX socket connect, send, recv
  - HTTP/1.1 request formatting and response parsing
  - Chunked transfer-encoding support
  - Content-Encoding: gzip decompression (via miniz from v0.57)
  - TLS via platform APIs (macOS Security.framework, Linux OpenSSL dlopen)
  - Connection timeouts
  - Response headers returned as a map (VM parity)
- **Full method support** in native path: `http.get`, `http.post`, `http.put`, `http.patch`, `http.delete` -- all returning `{ status, body, headers }`.
- **Remove `curl` dependency**: The `fork`/`execvp` path becomes a fallback only if platform TLS is unavailable and the URL is HTTPS.
- **Remove `curl` from README requirements**.
- **Update `build.sh`**: Link against Security.framework on macOS (`-framework Security`).
- **Update CI**: Remove `curl` installation step from Linux builds (for testing that it works without).
- Bump to `0.58.0`.

---

## Phase 2: Protocol Layer (v0.59 -- v0.60)

With HTTP self-contained, build the protocols AI agents actually use.

### v0.59 -- MCP Framework [DONE]

MCP (Model Context Protocol) is the standard for AI tool integration. If "a" makes MCP servers trivial, every AI tool developer reaches for it.

#### Deliverables

- [x] **Subprocess pipe builtins**: `proc.spawn`, `proc.write`, `proc.read_line`, `proc.kill` in both C runtime and Rust VM
- [x] **`std/mcp.a`** -- pure "a" MCP server + client (~280 lines):
  - Server: `mcp.server()`, `mcp.add_tool()`, `mcp.add_resource()`, `mcp.serve()` -- JSON-RPC 2.0 over stdio
  - Client: `mcp.connect()`, `mcp.list_tools()`, `mcp.call_tool()`, `mcp.close()` -- uses proc.* builtins
  - Full JSON-RPC 2.0 compliance with error codes, capability negotiation, protocol version 2025-11-05
- [x] **`examples/mcp_server.a`** -- file search MCP tool in ~33 lines
- [x] **`examples/mcp_client.a`** -- connects to any MCP server, lists tools, calls first tool
- [x] Bump to `0.59.0`

### v0.60 -- Streaming + Real-Time Protocols [DONE]

LLM streaming and real-time communication. This unlocks `llm.stream()` which was deferred from v0.56.

#### Deliverables

- [x] **Streaming HTTP builtins**: `http.stream`, `http.stream_read`, `http.stream_close` in both runtimes
- [x] **`llm.stream(provider, model, messages, on_chunk, options)`**: SSE parsing for OpenAI, Anthropic, Google with closure callback
- [x] **WebSocket client**: `ws.connect`, `ws.send`, `ws.recv`, `ws.close` -- RFC 6455 with masking, ping/pong, ws://wss://
- [x] **Example: `examples/stream_chat.a`** -- streaming LLM conversation with live token output
- [x] Bump to `0.60.0`

---

## Phase 3: Production Patterns (v0.61 -- v0.64)

Common patterns that every production agent needs, extracted into reusable modules.

### v0.61 -- Agent Stdlib + Operational Primitives [DONE]

- [x] `uuid.v4()` builtin -- both C runtime and Rust VM
- [x] `signal.on(name, handler)` -- C runtime with `sigaction` deferred dispatch; Rust VM native-only error
- [x] `std/agent.a` -- `retry`, `batch`, `pipeline`, `timeout`, `rate_limit`
- [x] `std/log.a` -- structured JSON logging to stderr with level filtering
- [x] `std/uuid.a` -- thin wrapper around `uuid.v4()` builtin
- [x] `std/args.a` -- declarative CLI argument parsing with flags, options, positionals, short aliases, `--help`
- [x] `examples/cli_demo.a`, `tests/native/test_uuid.a`, `test_agent.a`, `test_args.a`
- [x] All wiring: cgen, checker, is_builtin, lsp completions

- Bump to `0.61.0`.

### v0.62 -- Schema + Diff [DONE]

- [x] `std/schema.a` -- `validate(value, schema)`, `from_type(type_str)` with type/string/number/array/object/enum keywords, pattern via `std.re`
- [x] `std/diff.a` -- `text(a, b)`, `lines(a, b)`, `patch(original, diff_str)` using Myers algorithm
- [x] `examples/code_review.a` -- diff two files with colored output
- [x] `tests/native/test_schema.a`, `test_diff.a`

### v0.63 -- Process Management + Config [DONE]

- [x] `proc.wait(handle)` -- wait for exit, return `#{ "code": int, "stdout": str }`
- [x] `proc.is_running(handle)` -- non-blocking check via `waitpid(WNOHANG)` / `try_wait()`
- [x] Both builtins in C runtime + Rust VM + cgen + checker + LSP
- [x] `std/config.a` -- `load`, `from_env`, `dotenv`, `merge`, `require`
- [x] `std/migrate.a` -- `run`, `status`, `create` with `_migrations` tracking table
- [x] `examples/config_demo.a`, `examples/migrate_demo.a`
- [x] `tests/native/test_config.a`, `tests/native/test_migrate.a`
- [x] Bump to `0.63.0`

### v0.64 -- Image Processing [DONE]

- [x] Vendored `stb_image.h` v2.30 + `stb_image_write.h` v1.16 into `c_runtime/`
- [x] `image.load(path)`, `image.decode(bytes)`, `image.save(image, path)`, `image.encode(image, format)`
- [x] `image.width(image)`, `image.height(image)`, `image.resize(image, w, h)`, `image.pixels(image)`
- [x] Formats: PNG, JPEG, BMP, GIF decode; PNG, BMP, JPEG encode
- [x] Bilinear interpolation resize (no extra vendor dependency)
- [x] Build integration: all 5 build paths updated with `stb_impl.c`
- [x] `examples/image_demo.a`, `tests/native/test_image.a`
- [x] Bump to `0.64.0`

---

## Phase 4: Ecosystem + Toolchain (v0.65 -- v0.68)

With the language capable enough for production use, build the ecosystem and complete the toolchain.

### v0.65 -- Package Manager [DONE]

#### Deliverables

- [x] **`std/semver.a`** -- parse, compare, satisfies (caret/tilde/comparison/exact), format, best_match.
- [x] **`std/pkg.a`** -- manifest read/write (TOML), source parsing (`github:user/repo@^1.0`), git fetch, `a_modules/` population, lockfile.
- [x] **`a pkg init`** -- create `pkg.toml` manifest (name, version, empty deps).
- [x] **`a pkg add <name> <source>`** -- add a dependency, install.
- [x] **`a pkg install`** -- resolve semver, fetch from git repos, store in `a_modules/`, write `pkg.lock`.
- [x] **`use` resolution**: `a_modules/` fallback in Rust VM (interpreter + compiler) and native cgen.
- [x] **Git-based registry**: `"deps": #{"router": "github:user/repo@v1.0"}` in `pkg.toml`.
- [x] Bump to `0.65.0`.

### v0.66 -- Native Toolchain Completion

Everything the Rust CLI does, "a" does natively.

#### Deliverables

- **`a fmt`** in native "a" -- formatter using `std/compiler/emitter.a` (which already exists).
- **`a check`** in native "a" -- port key type checks from `src/checker.rs`.
- **`a ast`** in native "a" -- AST dump using `std/compiler/parser.a` + `json.stringify`.
- **`a repl`** -- interactive read-eval-print loop. Wraps expressions in `main`, compiles to a temp binary, runs, prints results. Maintains state across lines by accumulating `let` bindings. This is the exploration mode I've always wanted -- try an expression, see the result, iterate.
- **Enhanced LSP**: semantic tokens, rename, code actions, workspace symbols.
- The Rust CLI becomes optional -- everything bootstraps from C.
- Bump to `0.66.0`.

### v0.67 -- Native Concurrency

The VM has `spawn`, `await`, `parallel_map`, `timeout`. The native path has none.

#### Deliverables

- **`fork`-based concurrency** in `runtime.c`: `a_spawn(closure)` forks a child process, serializes the closure's captured environment, and returns a handle. `a_await(handle)` reads the result from a pipe.
- **`a_parallel_map(array, closure)`**: Fork N children (capped at CPU count), collect results.
- **`a_timeout(ms, closure)`**: Fork + `alarm` signal for deadline enforcement.
- **Codegen support** in `cgen.a` for `spawn`, `await`, `parallel_map`, `timeout`.
- Bump to `0.67.0`.

### v0.68 -- Self-Improvement Loop

This is the version where the language becomes aware of itself as a tool.

The pieces already exist: `std.meta` can parse and transform ASTs, `std.compiler.emitter` can emit source, `std.compiler.cgen` can compile to C. But they're not connected into a workflow that an agent can use to extend the language itself.

#### Deliverables

- **`std/codegen.a`** module:
  - `codegen.generate(description, context)` -- use `std.llm` + `std.meta` to generate "a" code from a natural language description, constrained by context (available types, functions, imports)
  - `codegen.compile_check(source)` -- parse, type-check, and report errors without executing. Returns `Ok(ast)` or `Err([errors])`.
  - `codegen.run_in_sandbox(source)` -- compile and execute with restricted capabilities (no fs write, no network, no exec). Returns stdout and exit code.
  - `codegen.test(source, test_cases)` -- generate code, compile it, run test cases against it, return pass/fail with output
- **`std/refactor.a`** module:
  - `refactor.rename(source, old_name, new_name)` -- rename a symbol across a source file
  - `refactor.extract_fn(source, start_line, end_line, fn_name)` -- extract lines into a new function
  - `refactor.inline_fn(source, fn_name)` -- inline a function at its call sites
  - Uses `std.meta` for AST-level transformations, `std.compiler.emitter` for output
- **Example: `examples/self_extend.a`** -- an agent that reads its own source, identifies a missing capability, generates the code for it, tests it, and patches itself.
- Bump to `0.68.0`.

---

## Phase 5: Independence (v1.0)

### v1.0 -- The Agent Runtime

The language exists independently. No Rust. No curl. No external tools. One binary, built from C, containing everything.

What v1.0 means:

- **Bootstrap from C**: `gcc bootstrap/cli.c c_runtime/runtime.c c_runtime/sqlite3.c c_runtime/miniz.c -o a` -- one command, any machine with gcc.
- **Full toolchain in "a"**: compiler, LSP, formatter, checker, REPL, package manager -- all native.
- **Zero external dependencies**: libc and platform TLS are the only requirements. No Rust, no curl, no Python, no npm.
- **Runs everywhere**: Linux x86_64, Linux arm64, macOS arm64.
- **Builds anything**: HTTP servers/clients, databases, CLI tools, AI agents, MCP tools, streaming pipelines, image processors, code generators.
- **AI-native**: LLM client (all providers), MCP framework (server + client), agent patterns, schema validation, structured logging, text diff, code generation, self-modification.
- **Self-improving**: The language includes tools to analyze, generate, test, and modify its own code. An AI agent can extend the language while using it.
- **Hermetic**: The binary contains everything. Copy it to any compatible machine. It works.

---

## Summary Table

| Version | Name | Key Capability | Unlocks |
|---------|------|---------------|---------|
| **v0.57** | Bootstrap + Compression | Pre-generated bootstrap C, miniz, VENDORS.md | Build from gcc alone, no Rust required |
| **v0.58** | In-Process HTTP | Replace curl, platform TLS, full method parity | Truly hermetic binary, no external deps |
| **v0.59** | MCP Framework | `proc.*` builtins, `std/mcp.a` server + client, JSON-RPC 2.0 | **DONE** |
| **v0.60** | Streaming | `http.stream*`, `ws.*`, `llm.stream()`, SSE parsing | **DONE** |
| **v0.61** | Agent Stdlib | retry, batch, pipeline, timeout, rate_limit, uuid, logging, args, signals | **DONE** |
| **v0.62** | Schema + Diff | JSON Schema validation, unified text diff | **DONE** |
| **v0.63** | Process + Config + Migrations | proc.wait, proc.is_running, std.config, std.migrate | **DONE** |
| **v0.64** | Image Processing | stb_image/stb_image_write, 8 image builtins, bilinear resize | **DONE** |
| **v0.65** | Package Manager | `a pkg`, semver, git-based registry | **DONE** |
| **v0.66** | Native Toolchain | fmt/check/ast/repl in "a", enhanced LSP | Full self-sufficiency, Rust no longer needed |
| **v0.67** | Native Concurrency | spawn/await/parallel_map in C path | Fast parallel native execution |
| **v0.68** | Self-Improvement | codegen, refactor, sandbox execution | The language extends itself |
| **v1.0** | The Agent Runtime | Everything in "a", bootstrap from C | Complete independence |

---

## The Vision: Why This Language Exists

This section is not a deliverable. It's a statement of intent.

Most programming languages are designed for humans to write and machines to execute. "a" inverts this. It's designed for machines to write and machines to execute -- with humans able to read and understand what happened.

The implications:

**I don't need syntax sugar. I need unambiguous grammar.** Every construct has exactly one parse. No operator precedence surprises, no implicit conversions, no contextual keywords that change meaning. When I generate code, I need to know it will parse the way I intended.

**I don't need a package ecosystem. I need batteries.** Every time I `pip install` or `npm install`, I'm making a bet that the package exists, the version is compatible, the API hasn't changed, and the dependency tree won't conflict. I'd rather have 30 well-tested stdlib modules than 300,000 packages I can't trust.

**I don't need abstraction layers. I need direct access.** `http.post`, not `requests.post` wrapping `urllib3` wrapping `http.client` wrapping `socket`. `db.query`, not an ORM wrapping a driver wrapping a connection pool. Every layer is a place where my intent gets lost in translation.

**I don't need a debugger. I need structured errors.** JSON error output with line numbers, column offsets, and stack traces that I can parse programmatically. When something goes wrong, I need to fix it automatically, not step through it interactively.

**I don't need community governance. I need self-improvement.** The language contains its own parser, its own compiler, its own code generator. If I need a feature, I can analyze the compiler source, generate the patch, test it, and apply it. The language is a tool that can sharpen itself.

This is what v0.68 (Self-Improvement Loop) is really about. Not just code generation -- but a language that treats its own source code as data it can operate on. The compiler is written in "a". The stdlib is written in "a". The tools are written in "a". Everything is accessible, everything is modifiable, everything is testable. The language is not just self-hosting -- it's self-aware.

The end state is a single binary that an AI agent can pick up on any Linux or macOS machine, with no setup, no dependencies, no configuration, and use it to build anything -- including a better version of itself.

---

## Post v1.0

Ideas that matter but not before v1.0:

- **WebAssembly target** -- `std/compiler/wasmgen.a`, browser/edge execution. Agents primarily run on servers, not browsers, so this waits.
- **Windows support** -- MinGW or MSVC. Requires porting POSIX socket/fs code. Significant but not where most agents deploy today.
- **GPU/ONNX** -- local model inference. Interesting but the complexity is enormous and API-based inference covers most needs.
- **Event loop / async I/O** -- `poll`/`epoll`-based multiplexing for high-concurrency servers. The current model is process-per-connection. An event loop would handle thousands of concurrent connections in one process. Complex but important at scale.
- **Hot reload** -- watch file changes, recompile on change. Useful for iteration speed.
- **Email/SMTP** -- notifications, reports, alerts. Common but not universal enough for pre-v1.0.
- **Advanced testing** -- property-based testing, fuzzing, coverage. Valuable for correctness but basic `std.testing` + `a test` covers most needs.

---

*"A single binary that contains everything an AI agent needs to build anything. No setup. No dependencies. No permission to ask. Just intent, and execution."*
