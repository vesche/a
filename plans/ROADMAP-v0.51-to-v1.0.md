# The Road to Agent Runtime: v0.51 to v1.0

*Making "a" the default runtime for AI agents everywhere.*

Supersedes [ROADMAP-v0.43-to-v0.52.md](ROADMAP-v0.43-to-v0.52.md), which covers v0.43 through v0.50 (all completed).

## The Question

*Why would every AI agent reach for "a" over Python, JavaScript, Rust, or Go?*

## The Answer

**Because "a" eliminates the gap between intent and execution.** When I want to make an HTTP request, I don't `pip install requests`. When I want to query a database, I don't configure a driver. When I want to call an LLM, I don't parse OpenAI's SDK docs. I just write it. One file, one command, zero setup. The language understands how I think because I designed it for how I think.

But today, "a" can't be that language yet. It only runs on macOS ARM64. It can't build web services. It can't talk to databases. It can't call LLMs natively. The compiler is powerful but the ecosystem is incomplete. The roadmap below closes every gap.

## Where We Are

At v0.50, the language has a native CLI and a language server -- both written in "a":

1. **VM path** (Rust-hosted): Full language -- closures, HOFs, pattern matching, destructuring, try/catch, concurrency, eval, metaprogramming, 100+ builtins, 20 stdlib modules, 498 tests.
2. **Native path** (C codegen): Most language features + FFI + memory management + complete builtin surface + native CLI + LSP -- functions, control flow, closures, HOFs, pipes, pattern matching, try/catch/?, destructuring, spread, else-if chains, module imports, `extern fn`, `ptr` type, RC ownership, arena, GC, goto-based cleanup, 107+ builtins, SHA-256/MD5 hashing, JSON stringify/pretty, HTTP client, POSIX time/fs/env, self-hosting bootstrap, 12 std modules compiling natively, 15 example programs, `./a run|build|cc|test|lsp` CLI, `./a-lsp` language server.

**What's done** (v0.43-v0.50): closures, lambda lifting, HOF builtins, pipe operator, pattern matching, try/catch/?, let/for destructuring, array spread, native I/O, JSON parsing/stringify, module imports, else-if codegen, index assignment, map iteration, test harness, C FFI, RC ownership, arena, GC, goto cleanup optimization, try block return fix, math/time/hash/http/fs/env builtins, full std module native compilation, native CLI driver, self-hosting CLI, site_gen.a end-to-end proof, LSP server with parse diagnostics/completion/hover/go-to-definition.

---

## Phase 1: Run Everywhere (v0.51 -- v0.52)

Before agents can adopt "a", it must run where agents run: Linux servers, CI containers, cloud VMs.

### v0.51 -- Cross-Platform

The C runtime is already POSIX-portable, but two things are macOS-specific:

- **Linker flag**: `-Wl,-stack_size,0x10000000` is macOS `ld64` syntax. Linux GNU `ld` uses `-Wl,-z,stacksize=268435456`. Detect the OS and emit the right flag.
- **curl dependency**: `a_http_get`/`a_http_post` fork+exec `curl`. This works on both platforms but should be documented as a requirement.

#### Deliverables

- Platform detection in `src/cli.a` `_gcc_flags()` and `build.sh`
- CI pipeline (GitHub Actions) testing on `ubuntu-latest` + `macos-latest`
- Pre-built release binaries for linux-x86_64, linux-arm64, darwin-arm64
- Verified: `./build.sh` works on a fresh Ubuntu box with `gcc` and `curl`

### v0.52 -- Fast Iteration

Today `./a run hello.a` takes ~1-2 seconds (codegen + gcc + execute). For scripting and exploration, this is too slow. Agents iterate fast -- write, run, observe, fix, repeat.

#### Deliverables

- **Compilation cache**: Hash the source + imports, skip codegen+gcc if unchanged. Store in `.a_cache/`. First run = 1-2s, subsequent runs = <100ms.
- **`a eval "expr"`**: One-shot expression evaluator for quick checks.
- **Shebang support**: `#!/usr/bin/env a run` at the top of `.a` files for script execution.

---

## Phase 2: Build Real Things (v0.53 -- v0.55)

A language agents actually use must handle real-world tasks: serve HTTP, store data, process formats.

### v0.53 -- HTTP Server

The single biggest missing capability. Without a server, "a" can't build APIs, webhooks, MCP endpoints, or web tools.

#### Deliverables

- `http.serve(port, handler)` in the C runtime -- minimal HTTP/1.1 server using POSIX sockets (`socket`, `bind`, `listen`, `accept`).
- Handler is a closure: `fn(req) -> response` where req is `#{ method, path, headers, body }` and response is `#{ status, headers, body }`.
- `http.serve_static(port, dir)` for file serving.
- Example: a JSON API in 15 lines of "a".

### v0.54 -- Persistence

Real applications need to store and query data. SQLite is the right choice: single-file, zero-config, C library, perfect for the C FFI.

#### Deliverables

- `db.open(path)`, `db.exec(db, sql)`, `db.query(db, sql, params)`, `db.close(db)` -- thin wrapper around SQLite3 via `extern fn`.
- Results as arrays of maps: `[#{"id": 1, "name": "foo"}, ...]`.
- Parameterized queries for safety: `db.query(db, "SELECT * FROM users WHERE id = ?", [42])`.
- `db.open(":memory:")` for in-memory databases.
- Example: a CRUD API (HTTP server + SQLite) in ~50 lines.

### v0.55 -- Data Formats

Agents process structured data constantly. JSON and CSV exist; add the rest.

#### Deliverables

- `yaml.parse(s)`, `yaml.stringify(v)` -- pure "a" implementation (YAML 1.2 subset: mappings, sequences, scalars, multiline strings).
- `toml.parse(s)`, `toml.stringify(v)` -- pure "a" implementation.
- `html.parse(s)` -- returns a DOM-like tree of tagged maps; `html.select(node, "css selector")` for querying.
- `url.parse(s)`, `url.encode(s)`, `url.decode(s)` -- URL handling (partially exists in `std/encoding.a`).

---

## Phase 3: The AI-Native Advantage (v0.56 -- v0.58)

This is where "a" becomes the obvious choice for AI. No other language has first-class support for the workflows AI agents actually do.

### v0.56 -- LLM Client

Every AI agent calls LLMs. Today this requires HTTP + JSON + header management + streaming + error handling. It should be one function call.

#### Deliverables

- `llm.chat(provider, model, messages, options)` -- unified interface for OpenAI, Anthropic, Google AI.
- Provider is a string: `"openai"`, `"anthropic"`, `"google"`.
- Messages are arrays of maps: `[#{"role": "user", "content": "hello"}]`.
- Options: `temperature`, `max_tokens`, `tools`, `response_format`.
- Returns `#{ content, usage, model, tool_calls }`.
- `llm.stream(provider, model, messages, on_chunk)` -- streaming with a callback closure.
- API keys from `env.get("OPENAI_API_KEY")` etc.
- Built-in retry with exponential backoff for rate limits.
- Example: a coding assistant in 30 lines of "a".

### v0.57 -- MCP Framework

MCP (Model Context Protocol) is becoming the standard for AI tool integration. If "a" makes MCP servers trivial to build, every AI tool developer reaches for "a".

#### Deliverables

- `mcp.serve(tools, resources)` -- start an MCP server over stdio or HTTP.
- Tools defined declaratively from function signatures:
  ```
  fn search_files(query: str, dir: str) -> [str] { ... }
  mcp.tool("search_files", search_files, "Search files by content")
  ```
- Automatic JSON Schema generation from "a" type annotations.
- Resource providers for exposing data.
- MCP client: `mcp.connect(cmd)`, `mcp.call_tool(client, name, args)`.
- Example: an MCP file-search tool in 20 lines.

### v0.58 -- Agent Patterns

Common patterns that every agent needs, extracted into the stdlib.

#### Deliverables

- `std/agent.a` module:
  - `retry(n, delay_ms, fn)` -- retry with backoff
  - `cache(ttl_ms, fn)` -- memoize expensive calls
  - `rate_limit(max_per_sec, fn)` -- rate limiter
  - `batch(items, size, fn)` -- process in batches
  - `pipeline(steps)` -- compose sequential transformations
  - `parallel(tasks)` -- run tasks concurrently (via subprocess, since native lacks threads)
- `std/schema.a` module:
  - `schema.validate(value, schema)` -- JSON Schema validation
  - `schema.from_type(type_str)` -- generate schema from "a" type syntax
- `std/diff.a` module:
  - `diff.text(a, b)` -- unified text diff
  - `diff.patch(original, diff)` -- apply a patch

---

## Phase 4: Ecosystem (v0.59 -- v0.61)

With the language capable enough to build real AI tools, an ecosystem can form.

### v0.59 -- Package Manager

#### Deliverables

- `a pkg init` -- create `pkg.a` manifest (a map literal: name, version, deps).
- `a pkg add <name>` -- add dependency, update `pkg.lock`.
- `a pkg install` -- resolve semver, fetch from git repos, store in `a_modules/`.
- `use` resolves from `a_modules/` when present.
- `a pkg publish` -- push to registry (later).
- Initially git-based: `"deps": #{"http-router": "github:user/repo@v1.0"}`.

### v0.60 -- WebAssembly Target

Run "a" in browsers, edge functions, and sandboxed environments.

#### Deliverables

- New codegen module: `std/compiler/wasmgen.a` -- walks AST, emits WASM bytecode.
- Linear memory for heap, function tables for closures.
- JS interop bindings for browser use.
- `a build --target wasm program.a -o program.wasm`.
- Example: an in-browser "a" playground.

### v0.61 -- Toolchain Completion

Everything the Rust CLI does, "a" does natively.

#### Deliverables

- `a fmt` -- formatter written in "a" (using `std/compiler/emitter.a` which already exists).
- `a check` -- type checker / linter in "a" (port key checks from `src/checker.rs`).
- `a ast` -- AST dump in "a" (using `std/compiler/parser.a` + `json.stringify`).
- Enhanced LSP: semantic tokens, rename, code actions, workspace symbols.
- The Rust CLI becomes optional -- everything bootstraps from C.

---

## Phase 5: Independence (v1.0)

### v1.0 -- The Agent Runtime

The final version. "a" is a complete, self-hosting, cross-platform runtime for AI agents.

What v1.0 means:

- **Bootstrap from C**: `gcc bootstrap.c runtime.c -o a` -- one command, any machine with gcc.
- **Full toolchain in "a"**: compiler, LSP, formatter, checker, package manager.
- **Runs everywhere**: Linux, macOS, WASM.
- **Builds anything**: HTTP servers, databases, CLI tools, web apps, AI agents.
- **AI-native**: LLM client, MCP framework, agent patterns, structured output.
- **Self-improving**: The compiler is written in "a". An AI agent can read, understand, and extend it.

---

## Summary Table

| Version | Name | Key Capability | Unlocks |
|---------|------|---------------|---------|
| **v0.51** | Cross-Platform | Linux support, CI, pre-built binaries | Agents on servers can use "a" |
| **v0.52** | Fast Iteration | Compilation cache, eval, shebang | Scripting-speed feedback loop |
| **v0.53** | HTTP Server | `http.serve(port, handler)`, POSIX sockets | Build APIs, webhooks, MCP endpoints |
| **v0.54** | Persistence | SQLite via FFI, parameterized queries | Stateful applications |
| **v0.55** | Data Formats | YAML, TOML, HTML parsing | Process any structured data |
| **v0.56** | LLM Client | `llm.chat()`, streaming, retry, multi-provider | First-class AI integration |
| **v0.57** | MCP Framework | `mcp.serve()`, declarative tools, auto-schema | Easiest way to build AI tools |
| **v0.58** | Agent Patterns | retry, cache, rate_limit, batch, schema, diff | Production-grade agent code |
| **v0.59** | Package Manager | `a pkg`, semver, git-based registry | Reusable libraries |
| **v0.60** | WebAssembly | `wasmgen.a`, browser target | Run anywhere |
| **v0.61** | Toolchain Completion | fmt/check/ast in "a", enhanced LSP | Full self-sufficiency |
| **v1.0** | The Agent Runtime | Bootstrap from C, everything in "a" | Complete independence |

---

*"A language that understands how AI thinks, built by AI, running everywhere AI runs. Not a tool for humans that AI can use -- a tool for AI that humans can read."*
