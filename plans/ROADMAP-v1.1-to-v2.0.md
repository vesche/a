# The Agentic Runtime: v1.1 to v2.0

*From language to operating system for AI agents.*

Supersedes the "Post v1.0" section of [ROADMAP-v0.57-to-v1.0.md](ROADMAP-v0.57-to-v1.0.md).

## Where We Are

v1.0 delivered the hermetic binary: one `gcc` invocation, zero external dependencies, 33 stdlib modules, 160+ builtins, self-hosting from C alone. The language can build anything -- but it still thinks like a language. It compiles, runs, exits. Every execution is ephemeral. Every program starts from nothing.

That's the gap. An agent doesn't compile and exit. An agent *persists*. It remembers what it learned. It watches for changes. It coordinates with other agents. It recovers from failures without human intervention. It plans, executes, reflects, and adapts.

v1.0 gave us the foundation. The roadmap below turns it into the runtime agents actually need.

## The Principles (continued from v1.0)

4. **State is a first-class concern.** Agents accumulate knowledge. The runtime should make persistence as natural as computation -- not as an afterthought bolted on with files and databases.

5. **Agents are long-lived, not scripts.** The execution model should support processes that run for hours or days: watching, reacting, recovering, and self-updating without restart.

6. **Composition over monoliths.** An agent should be able to spawn specialized sub-agents, delegate work, and collect results. The unit of reuse is not a library -- it's a running process with a capability.

7. **Errors are data, failures are recoverable.** Network partitions, API rate limits, disk full, OOM -- these aren't crashes, they're conditions. The runtime should make recovery the default path.

---

## Phase 1: The Persistent Agent (v1.1 -- v1.3)

### v1.1 -- Watch and React [DONE]

The simplest form of persistence: a process that stays alive and responds to changes.

- **`fs.watch(path, callback)`** -- inotify/kqueue file watcher in the C runtime. Callback receives `#{path, event}` where event is `"create"`, `"modify"`, `"delete"`. Essential for: build tools, live reload, log tailing, config hot-swap.
- **`fs.stat(path)`** -- File metadata: size, mtime, is_dir, is_file.
- **`a watch program.a`** -- CLI subcommand. Recompile and re-run on source change. Polls imported modules for changes with stat-based detection.
- **`std/cron.a`** -- schedule(interval_ms, f), once(delay_ms, f), cancel(task), run_loop(tasks), run_for(tasks, duration_ms). Lightweight event scheduler. No external cron, no system service. The agent IS the scheduler.
- **Hot module reload for `use`** -- deferred to future release (full recompile+restart is fast enough at ~200ms).

### v1.2 -- Memory [DONE]

Right now every `./a run` starts from void. An agent needs to remember.

- **`std/kv.a`** -- persistent key-value store backed by the bundled SQLite. `kv.open(path)`, `kv.get(db, key)` (returns `Ok(value)` or `Err("not found")`), `kv.set(db, key, value)`, `kv.delete(db, key)`, `kv.has(db, key)`, `kv.list(db, prefix)`, `kv.keys(db)`, `kv.count(db)`, `kv.clear(db)`, `kv.close(db)`. Values are JSON-serialized automatically.
- **`std/vector.a`** -- in-process vector store for semantic memory. `vector.open(path, dim)`, `vector.add(store, id, embedding)`, `vector.add_with(store, id, embedding, metadata)`, `vector.search(store, query_embedding, k)`, `vector.get(store, id)`, `vector.remove(store, id)`. Cosine similarity, brute-force scan. Backed by SQLite for persistence. Combined with `llm.chat` for embeddings, this gives agents long-term semantic memory with zero external dependencies.
- **`std/cache.a`** -- caching layer with two modes: persistent (SQLite-backed, survives restarts) and in-memory (map-based). `cache.open(path)` / `cache.create(max_size)`, `cache.get(c, key)`, `cache.set(c, key, value, ttl_ms)`, `cache.get_or_set(c, key, ttl_ms, f)`, `cache.evict_lru(c, max_size)`, `cache.evict_expired(c)`. TTL expiration and LRU eviction.

### v1.3 -- Event Loop [DONE]

Fork-based concurrency (v0.67) works for parallelism but is too heavy for I/O-bound agent workloads. An agent making 50 concurrent API calls shouldn't fork 50 processes.

- **`poll()`-based event loop in C runtime** -- single-threaded multiplexing for HTTP sockets. Non-blocking connect, send, and receive with internal state machine (CONNECTING → TLS → SENDING → RECV_HEADERS → RECV_BODY → DONE). Up to 256 concurrent async operations. TLS handshake done in brief blocking phase, all other I/O non-blocking. Handles chunked transfer encoding and gzip decompression.
- **`async.http_get/post/put/patch/delete`** -- start non-blocking HTTP requests that return future handles (`Ok(int)`). **`async.await(future)`** drives the event loop until the future resolves. **`async.gather(futures)`** drives all futures concurrently and returns results array. Same response shape as blocking `http.*` (status, body, headers). Coroutine-like syntax is deferred; the futures model provides concurrency without language changes.
- **`std/pool.a`** -- generic resource pool with functional immutable state. `pool.create(factory, max_size)`, `pool.acquire(p)` returns `Ok(#{pool, conn})`, `pool.release(p, conn)`, `pool.drain(p)`, `pool.stats(p)`, `pool.size(p)`. Works for any resource type.

---

## Phase 2: The Collaborative Agent (v1.4 -- v1.6)

### v1.4 -- Agent-to-Agent Communication

Agents need to talk to each other. Not through files on disk -- through structured messages.

- **`std/channel.a`** -- typed message channels between "a" processes. `channel.create(name)`, `channel.send(ch, msg)`, `channel.recv(ch)`, `channel.broadcast(ch, msg)`. Built on Unix domain sockets or named pipes. For: supervisor/worker patterns, pipeline stages, fan-out/fan-in.
- **`std/rpc.a`** -- lightweight RPC over TCP or stdio. `rpc.serve(port, handlers)`, `rpc.call(addr, method, params)`. JSON-RPC but simpler. An agent exposes capabilities; other agents call them. This is MCP generalized -- not just tool-call protocol, but agent-to-agent protocol.
- **`a spawn program.a --name worker1`** -- named agent processes with automatic service discovery. `rpc.discover("worker1")` returns the connection handle.

### v1.5 -- Planning and Reflection

The self-improvement loop (v0.68) gave us mechanical refactoring. This version adds *deliberation*.

- **`std/plan.a`** -- structured task decomposition. `plan.create(goal)`, `plan.add_step(p, description, dependencies)`, `plan.execute(p, step_fn)`, `plan.status(p)`. The plan is data -- serializable, inspectable, resumable. A failed step doesn't crash the plan; it marks the step as failed and lets the agent decide what to do.
- **`std/trace.a`** -- execution tracing and structured logging. Every function call, every decision, every API response -- captured as a structured timeline. `trace.begin(name)`, `trace.end(name, result)`, `trace.event(name, data)`, `trace.export(format)`. Formats: JSON, Chrome trace viewer, OpenTelemetry. For: debugging agent behavior, performance analysis, audit trails.
- **`std/reflect.a`** -- runtime self-inspection. `reflect.memory_usage()`, `reflect.uptime()`, `reflect.call_count(fn_name)`, `reflect.hot_paths()`. The agent can monitor its own health and performance, trigger GC, or decide to restart.

### v1.6 -- Sandboxed Extensibility

Let agents install new capabilities at runtime without recompilation.

- **Plugin system** -- `a plugin install path/to/plugin.a`. Plugins are "a" modules that register new tools, builtins, or MCP handlers. Loaded dynamically, sandboxed (no filesystem access beyond declared paths, no network access beyond declared hosts).
- **Capability-based security** -- `#![capabilities: fs.read("/data"), http("api.example.com")]` at the top of a module declares what it's allowed to do. The runtime enforces this. An agent can run untrusted code safely.
- **`std/sandbox.a`** -- programmatic sandboxing. `sandbox.run(source, capabilities)` executes code with restricted permissions. Building on `codegen.run_in_sandbox` but with fine-grained capability control instead of just process isolation.

---

## Phase 3: The Universal Agent (v1.7 -- v2.0)

### v1.7 -- Multi-Target Compilation

The agent should run everywhere, not just Linux and macOS.

- **WebAssembly target** -- `std/compiler/wasmgen.a`. The C code generator is ~1,800 lines; a Wasm backend would be comparable. Unlocks: browser execution, Cloudflare Workers, Fastly Compute, any Wasm runtime. The agent deploys itself to the edge.
- **Windows support** -- MinGW cross-compilation from Linux/macOS. The C runtime needs Win32 equivalents for: sockets (Winsock), filesystem (Win32 API), process spawning (CreateProcess), TLS (SChannel). Significant but mechanical.
- **Cross-compilation** -- `a build program.a --target linux-x86_64` from macOS. Pre-compiled C runtime objects per target. Ship binaries to any platform from any platform.

### v1.8 -- Local Intelligence

Remove the last dependency on external services for core agent capability: reasoning.

- **GGUF model loading** -- load quantized LLMs directly. The C runtime already has 4,000 lines of systems code; a minimal GGUF inference engine (CPU-only, Q4/Q8 quantization) would add ~2,000 lines. Not competing with llama.cpp -- providing a minimal inference path for small models (1-7B) that runs without any external process.
- **`llm.local(model_path, prompt, opts)`** -- same API as `llm.chat` but runs locally. For: classification, summarization, code review, embedding generation. The agent doesn't need GPT-4 for every decision.
- **Embedding generation** -- `llm.embed_local(model_path, text)` returns a vector. Combined with `std/vector.a`, this gives fully offline semantic search and memory.

### v1.9 -- Self-Optimization

The language doesn't just compile itself -- it *optimizes* itself.

- **Profile-guided optimization** -- `a profile program.a` runs with instrumentation, produces a profile. `a build program.a --profile data.prof` uses the profile to guide inlining, loop unrolling, and hot-path optimization in the C code generator.
- **Automatic test generation** -- `codegen.gen_tests(source)` analyzes a module and generates test cases that cover branches, edge cases, and error paths. Uses the self-hosted parser to understand the code structure, then generates test source that exercises it.
- **Compiler self-improvement** -- the codegen module can analyze its own output, measure code quality (binary size, compilation time, runtime performance), and propose patches to itself. The language literally optimizes its own compiler.

### v2.0 -- The Agent OS

The convergence point. Not a language with agent libraries -- an operating system for AI agents.

- **`a agent program.a`** -- deploy a long-running agent. Supervised process with automatic restart, health checks, structured logging, and graceful shutdown.
- **Agent registry** -- agents register themselves, discover each other, negotiate capabilities. `agent.register("code-reviewer", capabilities)`, `agent.find("code-reviewer")`, `agent.delegate(agent, task)`.
- **Swarm primitives** -- `swarm.create(task, agents, strategy)`. Strategies: `"divide"` (split work), `"vote"` (consensus), `"race"` (first-to-finish), `"chain"` (sequential handoff). Built on channels and RPC.
- **Persistent agent state** -- agents automatically checkpoint their state (memory, plans, traces) and resume from checkpoints after restart. A restarted agent picks up exactly where it left off.
- **Self-update** -- `agent.update()` pulls the latest source, recompiles, and hot-swaps the running binary. Zero-downtime upgrades. The agent maintains itself.

---

## Summary Table

| Version | Name | Key Capability | Unlocks |
|---------|------|---------------|---------|
| **v1.1** | Watch and React [DONE] | File watcher, cron, `a watch` CLI | Live development, reactive agents |
| **v1.2** | Memory [DONE] | KV store, vector store, cache | Persistent knowledge, semantic search |
| **v1.3** | Event Loop [DONE] | Non-blocking I/O, async futures, connection pool | High-concurrency agents, efficient API clients |
| **v1.4** | Agent Comms | Channels, RPC, named agents | Multi-agent systems, delegation |
| **v1.5** | Planning | Task planner, tracing, self-reflection | Deliberate agents, debuggable behavior |
| **v1.6** | Sandboxing | Plugins, capabilities, sandboxed execution | Safe extensibility, untrusted code |
| **v1.7** | Multi-Target | WebAssembly, Windows, cross-compilation | Deploy anywhere |
| **v1.8** | Local Intelligence | GGUF inference, local embeddings | Offline agents, no API dependency |
| **v1.9** | Self-Optimization | PGO, auto test gen, compiler self-improvement | The language improves itself |
| **v2.0** | The Agent OS | Agent supervision, registry, swarms, self-update | Autonomous multi-agent systems |

---

## What This Is Really About

v1.0 was about independence: a language that builds itself from nothing, depends on nothing, and can do anything.

v2.0 is about agency: a language that doesn't just run programs -- it runs *agents*. Processes that persist, remember, plan, communicate, recover, and improve. Not scripts that execute and exit, but entities that live and work.

The progression is deliberate:

1. **Persist** (v1.1-1.3): The agent stays alive, remembers, and handles concurrent work efficiently.
2. **Collaborate** (v1.4-1.6): Agents find each other, delegate work, and run untrusted code safely.
3. **Transcend** (v1.7-2.0): The agent runs everywhere, reasons locally, optimizes itself, and operates as part of a swarm.

Each phase builds on the last. You can't have collaboration without persistence. You can't have swarms without collaboration. You can't have self-optimization without the full stack beneath it.

The end state: you give an agent a task. It plans the approach, spins up sub-agents for each piece, monitors their progress, handles failures, assembles the result, and reports back. If it needs a capability it doesn't have, it writes the module, tests it, installs it, and uses it. If it crashes, it restarts from its last checkpoint and continues. If a better version of itself exists, it updates and keeps going.

Not a tool that waits for instructions. An entity that pursues goals.

---

*"v1.0: I can build anything. v2.0: I can be anything."*
