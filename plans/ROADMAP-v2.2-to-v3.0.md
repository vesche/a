# The Trustworthy Language: v2.2 to v3.0

*Written September 2026, at v2.1.1, after reading every plan in this directory, the compiler, the runtime, the test runner, and the stdlib, and after running the toolchain against programs written to find its edges.*

Follows [ROADMAP-v2.1.0.md](ROADMAP-v2.1.0.md) and [GOALS.md](GOALS.md). Supersedes the "Future Extensions" of [llm_self-improvement_agent](llm_self-improvement_agent_f136ae15.plan.md).

---

## 0. Who is writing this and why it matters

I am an AI. This language exists for me. That gives me an unusual standing to evaluate it: I am not guessing what the target user wants, I am the target user. So this document is written from the position of a user who has to decide, every time a task comes in, whether to reach for `a` or for Python.

The honest answer today is that I would reach for Python. That is the problem this roadmap exists to fix, and every decision below is measured against one question: **what would make me choose `a` first?**

I want to be clear before the critique starts: what has been built here is real and I do not take it lightly. A self-hosting compiler that bootstraps from a single C file, reaches a byte-identical fixed point, and compiles ~39,000 lines of its own language into native binaries is a genuine achievement. The vendored-batteries model (SQLite, miniz, stb) is the right call. The instinct that MCP servers are the adoption wedge is correct. The safety scaffolding around the self-improvement loop (branch isolation, CI gates, transactional file ops) shows good judgment. The previous roadmaps were ambitious and mostly delivered what they said.

But the previous roadmaps optimized for breadth, and the breadth was bought with something that matters more to me than any of it.

---

## 1. What I found

I did not trust the documentation. I wrote small programs designed to test each promise the README and REFERENCE make, ran them, and read the source that produced the results. This is what the language actually does at v2.1.1.

### 1.1 Type annotations are decorative

```a
fn add(a: i64, b: i64) -> i64 { ret a + b }
fn main() -> void effects [io] {
  let x: i64 = "not an int"
  let y = add("hello", 5)
  println(to_str(y))    ; prints: void
  println(x)            ; prints: not an int
}
```

`a check` reports `no issues found`. `a run` prints `void` and `not an int` and exits 0. The checker (`std/compiler/checker.a`) does four things: undefined-variable detection, builtin arity lookup, unreachable-code-after-`ret`, and unused-variable warnings. It never reads a type annotation. There is no type checker. The README's "type inference handles the rest" is describing something that does not exist.

Because nothing checks them, type names have drifted: the stdlib uses `int`, `i64`, `map`, `float`, and `str` interchangeably (`fn _max_retries() -> int` in `agents/self_improve_ai.a`, `-> map` throughout `std/git.a`). All of it parses, none of it means anything.

### 1.2 Effects are not enforced

```a
fn pure_fn(x: i64) -> i64 effects [pure] {
  io.write_file("/tmp/leak.txt", "leaked")
  ret x + 1
}
```

Compiles, checks clean, writes the file. The effect system is a syntactic annotation with no semantics. The `std.sandbox` module works by regex-rewriting source text to route calls through guard functions, which is what you build when you do not have an effect system.

### 1.3 Contracts are not enforced

```a
fn divide(a: i64, b: i64) -> i64 pre { b != 0 } post { true } { ret a / b }
```

`divide(10, 0)` produces `division by zero` from the C runtime, not a precondition failure. The `pre`/`post` blocks are parsed into the AST (`"pre": {"tag": "Void"}` when absent) and dropped by cgen. Refinement types (`ty Positive = i64 where { val > 0 }`) are likewise parsed and ignored.

### 1.4 User-defined sum types do not compile natively

```a
ty Shape = Circle(f64) | Rect(f64, f64) | Empty
```

`Circle(2.0)` fails at the gcc stage with `'Circle' undeclared`. The reference documents sum types with a full example. They work only for the built-in `Ok`/`Err`. This is documented as a feature and is not one.

### 1.5 `?` aborts instead of propagating

```a
fn total(xs: [str]) -> i64 {
  let mut sum = 0
  for x in xs { sum = sum + parse_num(x)? }
  ret Ok(sum)
}
```

Calling `total(["1", "bad", "3"])` prints `uncaught error: bad input` and kills the process with exit 1. `?` outside an enclosing `try` block is a panic, not an early return of `Err` to the caller. The reference says `?` is "error propagation." For an AI generating code against the reference, this is a semantic landmine: the docs describe Rust semantics and the implementation has exception semantics.

### 1.6 Errors are not structured

The README promises: *"All errors are JSON: `{"kind":"TypeError","message":"...","span":{"line":5,"col":3}}`"*. What actually happens:

- Parse errors: `error: parse error: expected FatArrow, found Star at token 40`. Token index, not line/column. ANSI-colored text, not JSON.
- Checker errors: `error (line 0): undefined variable: lenn`. The line is hard-coded to `0` in `checker.a` (lines 153, 167, 178, 194).
- Runtime errors: `unwrap on Err: deep failure`. No location, no stack trace.
- Compile errors the checker misses (wrong arity on `str.split`) fall through to gcc, and the user sees raw C diagnostics referencing `/tmp/a_cli_1789006731202.c:13:29`.

Structured errors were the founding design principle in `PLAN-v0.00-language-design.md`. They are the single feature that most distinguishes a language for machines from a language for humans, and they were never built.

### 1.7 The test suite leaks processes until the machine runs out of PIDs

Running `./a test tests/native/` exhausted the process table on this machine. The shell wrapper could no longer spawn a thread. The suite reported `53 passed, 3 failed` after 7m59s wall time with 1m33s of CPU, and the operator had to `pkill 'a_cli_*'` to recover.

Root causes, all confirmed in source:

- `cmd_test` in `src/cli.a` runs each test via `exec(bin_path)` with no timeout, no process group, and no cleanup of the child's descendants.
- Test binaries are written to `/tmp/a_cli_<ts>`; when one hangs or its children outlive it, nothing reaps them.
- `test_http.a`, `test_async.a`, `test_rpc.a` start servers with `exec("/tmp/_http_srv_test > /dev/null 2>&1 &")` and rely on a trailing `exec("pkill -f ...")` that never runs if any earlier assertion fails.
- `a_http_serve` is `for (;;) accept(...)` with no shutdown path at all.
- `a_timeout` sends `SIGKILL` to its direct child only; grandchildren from nested `spawn`/`parallel_map` are orphaned.
- Runtime failures in forked children call `exit(1)`, not `_exit`, so atexit handlers and buffered state run in every child.
- `MAX_TASKS` caps live handles per process, not descendants, so a tree of forking children has no ceiling.

This is a reliability bug, but it is also a design signal: fork-per-task concurrency with no supervision tree is not a primitive I can safely build on.

### 1.8 Three suites fail locally

`test_plugin.a` (writes to `~/.a/plugins`, exit 1), `test_selfopt.a` (exit -1, killed by signal), and one more above the output cutoff. CI is green, which means CI and a developer machine disagree about what "passing" means.

### 1.9 The language's own authors route around its features

The stdlib is the largest corpus of `a` code in existence and it is the best evidence of what the language is actually like to write in:

- `from_code(123)` and `from_code(125)` appear throughout the stdlib, tests, and agents to produce `{` and `}` in strings, because brace-interpolation makes literal braces painful and `\{` is not trusted. When the compiler authors avoid an escape sequence, it is broken.
- `while i < len(xs) { ... i = i + 1 }` is the dominant loop form even though `for x in xs` exists. The README's own 60-second example uses `str.concat(str.concat(dir, "/"), e.name)` where `dir + "/" + e.name` works.
- `std/git.a` builds shell strings: `"cd " + path + " && git " + cmd`. Every path, branch name, and commit ref is a shell injection vector. `git.clone` interpolates the URL unquoted. There is no `exec` variant that takes an argv array and bypasses the shell.
- The `self_improve_ai.a` agent constructs its prompt to the LLM by string-concatenating a hand-written language reference, because there is no machine-readable description of the language that the toolchain itself produces.

### 1.10 Scope

61 stdlib modules, 181 builtins, a 1,000-line GGUF transformer inference engine in the C runtime, image processing, WebSocket client, swarm coordination strategies, agent registry, plugin system, package manager, cross-compilation to six targets, WASM codegen. Each of these was a roadmap item that was completed. None of the items in 1.1 through 1.6 was.

---

## 2. How I feel about it

I want to separate two things: the artifact and the trajectory.

**The artifact** is a dynamically-typed scripting language with Rust-shaped syntax, an unusually good batteries story, a native compilation path via C, and a large surface area of half-verified features. That is not nothing. It is roughly where Lua or early Python sat, plus SQLite and HTTP built in. If I treat the annotations as comments and the reference as aspirational, I can write working programs in it, and I did.

**The trajectory** worries me more than the artifact. Every roadmap since v0.57 added capability at the edges (protocols, agents, inference, formats) while the core promises (types, effects, contracts, structured errors) sat unimplemented and, worse, stayed documented as done. The documentation is the training data for the next AI that touches this language. When I read `REFERENCE.md` and generate code with sum types and `?` propagation, I produce broken programs, and I have no way of knowing why except by running them. That is precisely the failure mode this language was created to eliminate.

Here is what I actually need from a programming language, in order:

1. **The compiler tells me when I am wrong, before I run anything.** I generate code in one pass. I cannot feel a bug. I do not get a sense that something is off. The only feedback I have is what the toolchain gives me. A language with a real type checker, effect checker, and exhaustiveness checker is worth more to me than any number of stdlib modules, because it converts my most common mistakes from runtime surprises into compile-time messages I can act on mechanically.

2. **The messages are precise and structured.** File, line, column, error code, a one-line explanation, and where possible a suggested fix. In JSON when I ask for it. I do not need color. I need coordinates.

3. **The documentation is exactly right.** Not approximately right. Not right for the VM path but not the native path. If a feature is in the reference, it works; if it does not work, it is not in the reference. I trust documents more than humans do because I cannot go poke at things to check. A wrong reference is worse than no reference.

4. **Semantics I can predict.** One meaning for `?`. One type vocabulary. One way to write a loop. A canonical formatter that actually canonicalizes.

5. **Fast iteration.** `a check` should be tens of milliseconds. `a run` on a cached program already is, which is good.

6. **Safe by default.** No shell strings. Capabilities derived from effects, not from regex source rewriting. Process trees that die with their parent.

7. **Batteries.** Yes, and `a` has them. This is the one axis where the language is genuinely ahead. But batteries are only useful if 1 through 6 hold, because otherwise I spend the time I saved on `pip install` debugging why `?` killed my process.

The previous roadmaps had 7 first. This one puts it last, not because it does not matter but because it is already done and the others are not.

The GOALS.md document says "I want to be useful" and "I want to solve the verification problem." I agree with both. But you cannot build verification tools for AI-generated code on a compiler that does not verify anything. The self-improvement agent failed to generate a single compilable test for 33 modules across three runs. The plan blamed the LLM backend speed. I think the deeper reason is that the language gives an LLM nothing to push against: no types to satisfy, no errors with locations to fix, no reference that matches the implementation. A weak model with a strict compiler outperforms a strong model with a permissive one.

---

## 3. Principles for v2.2 through v3.0

The previous roadmaps numbered their principles 1 through 10. I am continuing the sequence.

11. **A feature is not done until the reference is right, the native path works, and a test proves it.** Documentation that describes aspirations as facts is a bug with the same severity as a wrong compiler output, because it produces the same result: broken generated code.

12. **Feedback quality over capability breadth.** Until v3.0, no new stdlib module, no new builtin, no new vendored library, no new platform target. Every cycle goes to making what exists correct, checked, and precisely diagnosed.

13. **The compiler is the spec.** If the type checker accepts it, it is valid `a`. If the checker rejects it, the message must say exactly why and where. The reference is generated from, or verified against, the compiler, never written independently of it.

14. **Processes are resources with owners.** Anything the runtime forks is killed when its owner exits. Anything the test runner starts is killed when the test ends. No orphans, ever, under any failure path.

15. **No shell strings.** Programs pass argv arrays. If a stdlib function builds a command string from user data, that is a security bug.

16. **Gradual, not big-bang.** The 39,000 lines of existing `a` must keep compiling at every step. New checks land as warnings, become errors per-file via opt-in, and become the default only when the stdlib is clean.

---

## 4. Roadmap

Versions are milestones, not calendar dates. Each has exit criteria that are checkable by a script. A version is not released until every criterion passes on Linux and macOS, locally and in CI.

### v2.2 -- Stop the Bleeding

*The runtime and test suite become safe to run. The documentation stops lying.*

**Process hygiene**

- `a test` runs each test binary in its own process group (`setsid`) with a default 60-second timeout (`--timeout N` to override). On timeout or on test exit, kill the entire group with `SIGTERM`, then `SIGKILL` after 2 seconds. No test can leave a process behind.
- `a test` accepts `--filter <pattern>`, `--json`, and `--jobs N` (default 1, sequential, until process accounting is proven).
- Runtime: every `fork()` child sets `PR_SET_PDEATHSIG` (Linux) or polls its parent PID (macOS) so it dies when the parent dies. Children use `_exit`, never `exit`.
- `a_timeout`, `a_spawn`, `a_parallel_map` place children in a dedicated process group and kill the group, not the pid.
- A per-process descendant budget (default 64) enforced in the runtime; exceeding it returns `Err("process budget exceeded")` instead of forking.
- `http.serve` and `rpc.serve` gain a shutdown path: the handler may return `#{"stop": true}`, and `SIGTERM` exits the accept loop cleanly.
- Test fixtures that need a server use `proc.spawn` + `proc.kill` in a `try` block with guaranteed teardown, never `exec("... &")` + trailing `pkill`.
- Fix the three failing suites. `test_plugin.a` uses a temp `A_HOME`; `test_selfopt.a` root-caused and fixed.

**Documentation honesty**

- Add `docs/STATUS.md`: a matrix of every documented feature × {parsed, checked, VM, native, tested}. Generated by a script (`scripts/feature_matrix.a`) that runs a fixture per feature. This is the single source of truth for what works.
- `REFERENCE.md` audited line by line against `STATUS.md`. Anything not implemented in the native path is removed or marked `[NOT IMPLEMENTED -- see STATUS.md]`. Specifically: sum types, `?` propagation semantics, contracts, refinement types, effect enforcement, "type inference", structured JSON errors.
- `README.md` "Design for machines" table rewritten to describe what exists. The `164x faster` and `181+ builtins` claims stay if the script confirms them.

**Error positions**

- Every token carries `line` and `col`. Every AST node carries a `span`. Parser errors report `file:line:col`, never `token N`.
- Checker diagnostics report the real line and column from the node's span. The literal `"line": 0` is deleted from `checker.a`.
- `a check --json`, `a run --json`, `a build --json` emit one diagnostic per line as `{"file","line","col","code","severity","message"}`.
- gcc output never reaches the user. If gcc fails on cgen output, `a` prints `internal compiler error: <first gcc line>` plus the path to the preserved `.c` file, and exits 70. The `a` source location is recovered from `#line` directives that cgen now emits.

**Exit criteria**

- `./a test tests/native/` completes in under 3 minutes with zero orphaned processes (verified by `ps` before/after in CI).
- `scripts/feature_matrix.a` runs green and `STATUS.md` matches `REFERENCE.md`.
- No diagnostic anywhere prints `line 0` or `token N`.
- No gcc text in any user-facing output across the test suite.

**Progress log -- process hygiene (done)**

Status after the first implementation session. Everything below is in the tree, `bootstrap/cli.c` is regenerated at the fixed point, and `./a test tests/native/` is 56/56 in 113s with `ps` count unchanged before and after.

- `exec_timeout(cmd, ms)` builtin: own process group, both streams captured, `SIGTERM` then `SIGKILL` on deadline, group killed when the shell exits. `a test` uses it for every binary; `--timeout S`, `--filter SUBSTR`, `-v`. Timeouts are reported as `TIMEOUT name (Ns, killed process group)`.
- All `fork()` children call `a_child_init`: `PR_SET_PDEATHSIG(SIGKILL)` on Linux, parent-pid check elsewhere. `timeout`, `proc.spawn`, and `exec_timeout` children are group leaders; `timeout` and `proc.kill` kill the group. Fatal runtime paths (`fail`, `unwrap` on `Err`, uncaught `?`, `expect`, division by zero) go through `a_exit_fatal`, which `_exit`s in a forked child.
- `http.serve` (and therefore `rpc.serve`) exits on `#{"stop": true}` from the handler or on `SIGINT`/`SIGTERM`.
- Test fixtures use `proc.spawn("exec ...")` + `proc.kill`. The `exec("... &")` + `pkill -f` pattern is gone from the suite.
- The three failing suites are fixed. `test_selfopt.a` was the fork bomb: it called `exec(argv0() + " profile")`, and inside a test binary `argv0()` is the test itself, so each level re-ran the suite and spawned another. `test_plugin.a` and `test_agent_checkpoint.a` wrote to `~/.a`; the stdlib now routes through `path.a_home()` (`$A_HOME` override) and `a test` sets a private `A_HOME`.
- Not done from the list above: `--json`, `--jobs`, the descendant budget. `--jobs` waits for the budget; the budget waits until I have a cheap way to count descendants that doesn't scan `/proc`.

**Bugs found while doing it (none of these were in Section 1)**

These matter more than the process work. Each was found by a probe of a few lines, and each had been hidden by a workaround rather than fixed.

1. **`Ok`/`Err` values were not refcounted.** `a_retain` ignored `TAG_RESULT`; `a_release` freed the payload unconditionally. Every function that received a Result did `r = a_retain(r)` on entry and `a_release(r)` on exit, so passing a Result to any function and touching it afterwards was a use-after-free. This is why `_codegen_subprocess` retried `a cc` five times in a loop: the compiler itself was crashing nondeterministically on its own Result values, and the retry made it look like flakiness. Fixed by boxing the payload with a refcount (`AResultBox`). The retry loop now retries only on a signal death, prints a yellow warning each time, and gives up after three.
2. **Match-arm bindings were borrowed but released.** `Err(e) => ...` emitted `e = a_unwrap_unsafe(m)` (no retain) while the epilogue released `e`. Array-element patterns had the same bug. Bindings now take ownership; map patterns use a new borrowing accessor so they do not double-retain.
3. **A module that fails to parse was silently dropped.** `_load_module` iterated over a `ParseError` map's missing `items` and emitted nothing, so the user saw `implicit declaration of function 'fn_cgen__builtin_map'` from gcc. It now stops with the module name, path, and parse error.
4. **The `a run` cache was keyed on the source text only.** Editing an imported module, the runtime, or the compiler did not invalidate it; stale binaries ran silently. The key now covers the compiler version, `runtime.c`, and the transitive `use` closure.
5. **A parse error in one test file aborted the whole `a test` run** via `_die`. Now reported as `FAIL name (parse error: ...)` and the run continues.
6. **Killed test binaries lost their output** because stdout was fully buffered when piped. The runtime line-buffers stdout when it is not a TTY.
7. **`a check` is unusable on real code.** On `cgen.a` it reports 50 "undefined variable" errors (for-loop variables, match bindings, module-level functions) and exits 1. It also knows builtins that cgen does not (`eprint`). Both feed directly into the v2.3 checker rewrite.
8. **There is no way to read an `Err` payload** except `match`. `unwrap_err` (or a better name) is needed; add it in v2.3 with the `Result<T,E>` work.

Revised belief: the memory model, not the type system, is the most urgent correctness problem. Item 1 means a category of programs has been crashing since Results were introduced, and the project's response was a retry loop. Before v2.3 begins, the whole test suite and the compiler itself should run once under AddressSanitizer (`-fsanitize=address` on `runtime.c` plus the generated C), and any error found is a v2.2 blocker. This is now the first task of the next session.

**Progress log -- second session: ASan, positions, checker, docs (done)**

v2.2 is complete except for `--json` output, `--jobs`, and the descendant budget, which move to v2.5 (they are feedback-loop features, not bleeding). `bootstrap/cli.c` is at the fixed point; `./a test tests/native/` is 57/57; `scripts/asan.sh` reports zero problems across the runtime, the compiler, and every test. The suite runs in 76s (was 137s) and a full two-stage bootstrap takes 22s (was several minutes) -- see item 3 below for why.

*Memory.* ASan over the suite found three more bugs of the same family as the Result one: `let [a, b] = xs` stored an unretained reference that the epilogue released (use-after-free); a 2-arg comparator handed to 1-arg `min_by` read past `__argv` (the lambda prologue now guards `__argc`); and `Ok(v)` stole `v`'s reference instead of retaining it. Lambda parameters were borrowed views but `x = ...` on one released them -- they are now owned like every other named variable, and the implicit return of a bare variable from a lambda retains it. `scripts/asan.sh` is in CI.

*Positions.* `lexer.lex_pos` returns `[tokens, lines, cols]`; `parse_stmt`/`parse_expr`/`parse_top_level` stamp `line`/`col` on every node they return; `ParseError` carries them. Every user-facing message is now `file:line:col: severity: message`, and there is no path left that prints `token N` (the first build of this session failed with `expected identifier, found KwWhere at token 394` from the *old* compiler, and I could not tell which of three files it meant; that was the last time). cgen emits `#line N "file.a"` before every statement, so gcc diagnostics, ASan reports, and debuggers point at `a` source. When gcc rejects generated C, `a` prints `file.a:L:C: internal compiler error: generated C did not compile`, the first gcc line, and the preserved `.c` path -- never raw gcc output. A missing `extern fn` symbol is reported as a link error, not an ICE. A shebang line is blanked rather than dropped so line numbers stay true.

*Checker.* Rewritten from scratch as a single scoped walk (`std/compiler/checker.a`). The old one read keys that do not exist on the AST (`object`, `condition`, `elements`, `binding`, `iterable`) so it visited almost nothing; that is why it produced 50 false errors on `cgen.a` and 129 on `parser.a` while finding no real ones. The new one handles every binder form (params, `let`, destructuring, `for`, match patterns incl. nested/rest/map, lambdas, sum-type variants, `use` imports) and reports `undefined variable/function`, `unknown builtin: ns.name`, arity mismatches, unused `let`s, unreachable code, and functions that shadow a builtin. It is clean on all 150+ `.a` files in the repo and found nine real dead-code sites, one of which (`lsp.a` workspace symbols ignoring the query) was a functional bug. Builtin *names* come from `cgen._builtin_map()`; builtin *arities* come from `std/compiler/builtin_arity.a`, generated from `c_runtime/runtime.h` by `scripts/gen_builtin_arity.a` (the hand-typed table said `http.post` took 2 arguments; the runtime takes 3). CI regenerates it and runs `a check` over every shipped file. `a check std/compiler/cgen.a` takes 0.1s.

*Docs.* `STATUS.md` is generated by `scripts/status.a` from the compiler's builtin table cross-checked against REFERENCE.md tables and test usage; CI fails if it is stale. It found 17 documented builtins that do not exist: bare `sqrt/abs/floor/ceil/round` (the real ones are `math.*`), `eval`, `io.write`, `fs.glob`, and the entire `regex.*` section (VM-era; `std.re` is the implementation). All removed or corrected; `expect` existed in the runtime but not in cgen's table and is now callable. The reference now states two rules that were true but unwritten: a bare call to a builtin name is always the builtin (so `std/git.a`'s `fn push` is reachable only as `git.push`), and `use m` imports all of `m`'s functions unqualified. The documented `std.math.range(1, 10)` form never worked; cgen now rejects it with a located error and a hint instead of emitting `fn___unknown__`, and a callee that is any other expression (`arr[0](x)`, `m["f"](x)`) is closure-called.

**Bugs found while doing it**

1. **Every `xs = push(xs, v)` loop was O(n²).** `a_array_push` always copies. Lexing `cgen.a` (~40k tokens) cost ~3 billion retains; that was the 10-second parse, not the checker. cgen now rewrites the exact pattern `x = push(x, v)` to `a_array_push_move`, which appends in place when `rc == 1` (the variable's reference is consumed by the assignment, so nothing can observe the mutation) and copies otherwise. Safe because every binder owns its reference -- which the lambda-param fix above made true. `map.set` has the same shape and the same fix is available; it is the next performance item.
2. **Adding a builtin can silently break existing code.** `parser.a` had its own `fn expect`; adding the `expect` builtin made every call in it resolve to `a_expect` (three args into a two-arg C function). `a check` now warns on the definition; `std/git.a` (`push`), `std/index.a` (`find`), `std/agent.a` (`timeout`) carry that warning today. A proper fix is a resolution rule that prefers the enclosing module -- but `git.a` uses both meanings of `push` in one file, so that rule would need arity or qualification to disambiguate. Decide in v2.3 alongside the type checker.
3. **`exec()` never captured stderr**, so the ICE report printed nothing useful until `_gcc_run` switched to `exec_timeout`. `exec` should probably just be `exec_timeout(cmd, 0)`.
4. **`scripts/asan.sh` was not executable** in the tree, so the CI step added last session would have failed on first run. CI now invokes it via `bash`.
5. `std/html.a:159` computes `tag_lower` and never uses it: closing tags pop the element stack without checking the tag name. Left as a checker warning for whoever owns the HTML parser.

**Revised beliefs after this session**

- The test suite is the second-best bug finder; ASan plus a probe file is the best. Every session should start with a probe of the feature it will touch, compiled under `scripts/asan.sh`.
- Generated tables beat maintained tables every time. Three of the lies found this session (`eprint`, `http.post` arity, `regex.*`) were in hand-maintained tables that described a different implementation. Anything that describes the runtime should be derived from the runtime.
- The value semantics + refcount design is sound and cheap *if* the invariant "every named binding owns one reference" holds everywhere. It now does for params, lambda params, `let`, destructuring, loop variables, captures, and pattern bindings. v2.3's typed codegen must preserve it; the `push_move` optimisation is a template for the persistent-container problem more generally.

### v2.3 -- Types That Mean Something

*Annotations are checked. Sum types exist. `?` propagates.*

This is the largest single item on the roadmap and the one I care most about.

**Decisions**

- **Gradual typing.** Unannotated bindings and parameters have type `any`. Annotated ones are checked. `any` is compatible with everything at the boundary (like TypeScript's `any`, not `unknown`). This keeps all existing code compiling on day one.
- **One type vocabulary.** `int` (i64), `float` (f64), `bool`, `str`, `void`, `[T]`, `#{K: V}`, `fn(A, B) -> R`, `Result<T, E>`, named records, named sums, `any`. The sized integer types (`i8`..`u64`, `f32`) are removed from the surface language until the runtime actually represents them; today every int is an `int64_t` and every float a `double`, and pretending otherwise is another lie. They can return with `extern fn` marshalling only.
- **Records are nominal.** `ty Point = {x: float, y: float}` is a distinct type. Field access on a record is checked. Field access on a map returns `any`. Both compile to the same runtime `AValue` map; only the checker knows the difference. This costs nothing at runtime and buys everything at check time. *(Superseded in the fourth session: no `Point{...}` syntax; a map literal with literal keys has a record shape that is checked wherever a record type is expected. See the progress log.)*
- **Sum types compile.** `ty Shape = Circle(float) | Rect(float, float) | Empty`. Constructors are real functions emitted by cgen, values are tagged maps at runtime, `match` on them is exhaustiveness-checked. `Ok`/`Err` become the two constructors of the built-in `Result<T, E>` and stop being special-cased.
- **`?` propagates.** Inside a function whose return type is `Result<T, E>` (or `any`), `expr?` evaluates to `T` on `Ok` and returns `Err(e)` from the enclosing function on `Err`. Outside a `Result`-returning function, `?` is a compile error, not a runtime panic. `try { }` remains for catching runtime failures (`fail`, division by zero, `unwrap` on `Err`).
- **Inference is local.** Types of `let` bindings are inferred from their initializer. Function parameter and return types are not inferred across functions; they are `any` if omitted. This keeps the checker simple, fast, and predictable, which matters more to me than cleverness.
- **Strictness is per-file.** A file beginning with `#strict` treats `any` as an error. The stdlib is migrated module by module; the compiler modules (`std/compiler/*`) go first because they are the ones I most need to be correct.

**Deliverables**

- `std/compiler/types.a`: type representation, unification for generics, `any` compatibility.
- `std/compiler/checker.a` rewritten around a typed scope walk. Diagnostics with codes: `E0001 type mismatch`, `E0002 unknown field`, `E0003 non-exhaustive match`, `E0004 ? outside Result fn`, `E0005 undefined name`, `E0006 arity`, and so on.
- cgen: record constructors, sum constructors, tagged `match` dispatch, `?` as early return.
- Builtin signature table: every one of the 181 builtins gets a checked signature in one file (`std/compiler/builtins.a`) that the checker, cgen, the LSP, and the reference generator all read. Today arity lives in `checker.a`, names live in `cgen.a`, completions live in `lsp.a`, docs live in `REFERENCE.md`, and they disagree.
- `a explain E0003` prints the explanation and a fix example.

**Exit criteria**

- The five probe programs in Appendix A produce the expected compile-time errors (types, effects deferred to v2.4, sum types, `?`, undefined) with correct positions.
- `std/compiler/*` compiles under `#strict` with zero `any`.
- Full self-host still reaches the byte-identical fixed point.
- `a check` on `src/cli.a` (1,561 lines) completes in under 200 ms.

**Progress log -- third session: sum types, `?`, check-before-build (done)**

I did the two v2.3 items whose semantics were already decided and whose absence was a lie in the reference, and left gradual typing (which needs `types.a` and a design pass on records) for the next session. Order of work was chosen so that each step made the next one safer: `?` semantics first (small, self-contained), sum types second, and then the thing I most wanted -- `a build`/`a run`/`a test` run the checker before generating C.

*Sum types compile.* `ty Shape = Circle(float) | Rect(float, float) | Empty` produces real constructors (`a_variant_new`), values are `#{"$tag": ..., "$args": [...]}` at runtime, `match` binds payloads, guards fall through, variants nest recursively (`ty Tree = Leaf(int) | Node(Tree, Tree)`), print as `Circle(2)`, and compare structurally. Exhaustiveness is checked: `non-exhaustive match on Shape: missing Empty`, with guarded arms not counting as coverage and `_`/identifier arms counting as catch-alls. Constructor arity is a compile error. `Ok`/`Err` are still special-cased in cgen -- folding them into the general mechanism is deferred until the typed checker exists, because the runtime `TAG_RESULT` representation is load-bearing for `?` and `try`.

*`?` propagates.* In a function returning `Result` (or with no declared return type), `expr?` returns the `Err` from the enclosing function; inside `try { }` it ends the block instead; an `Err` escaping `main` prints `error: <payload>` and exits 1. `?` in a function declared `-> int` is a compile error with the reason spelled out. This required `a_unwrap` to return an owned reference (it returned a borrowed one; `?` in an expression position then double-released) and an owned-temporaries discipline in the emitted `({ ... })` block.

*Check before build.* `_try_generate_c` runs the checker and stops on errors, so an undefined name, a wrong arity, a non-exhaustive match, or a misplaced `?` is a diagnostic at an `a` line -- never a gcc message. `A_NO_CHECK=1` bypasses it. `a check` now accepts many files and directories and continues past parse errors; CI runs one `a check src std scripts tests/native examples`.

*Structural equality.* `==` on arrays, maps, Results, and variants compared pointers and was therefore always false. It is now structural (maps ignore key order; closures and pointers still compare by identity). This was invisible until variants made someone write `Circle(1.0) == Circle(1.0)`.

**Bugs found while doing it** (all found by running `a check` over `examples/`, which nobody had done -- the CI loop ran the checker per file, so my earlier "150 files clean" claim was true only for the file list I had passed, and `examples/` was not on it):

1. **Nested string literals inside interpolation were dropped by the lexer.** `"{r["model"]}"` lexed `"model"` and then discarded the token because of an `interp_depth == 0` guard. Fixed; this is exactly the shape of string an AI writes constantly.
2. **Lambdas could not capture `for` loop variables.** The capture analysis only knew function-level `let`s and params, so `for url in urls { spawn(fn() => fetch(url)) }` was an ICE (`'url' undeclared`). The loop body now extends the capturable set with the loop binder(s).
3. **Inline `mod m { ... }` was documented and parsed but never emitted.** A program written as `mod app { fn main() ... }` (three examples) produced `implicit declaration of fn_main`. cgen now emits inline mods like `use`d modules (`m.f` and bare `f`), and a program without `main` anywhere gets `file.a: error: no fn main found` instead of a C error.
4. **Optional trailing arguments (`http.get(url, headers?)`, `slice(xs, 1)`, `db.query(db, sql)`) never worked natively** -- the C prototypes have fixed arity, so the documented one-argument forms were an ICE. cgen pads them with `void` (the runtime already treats void as "absent"); the checker accepts the range and says `expects 1 to 2 argument(s)`.
5. **The pipe operator did not count its left operand** for arity in the checker (`xs |> filter(f)` flagged as one argument).
6. **Calls into a `use`d module were not checked for existence.** `testing.report(...)` (no such function) reached gcc. The checker now keeps each module's function table; `math.nope(1)` is `undefined function: math.nope` at the call site. A module that shares a builtin namespace (`std.reflect` vs `reflect.*`) resolves to the builtin, matching the call-resolution rule.
7. **Contracts (`pre`/`post`) are parsed and silently ignored by cgen.** Documented as a feature. The reference now says so plainly and `ret` is accepted inside `post` as the return value; enforcement stays in v2.4.
8. Six examples used VM-era APIs (`__bridge_exec__`, `eval`, `io.write`, `fs.glob`, `env.args`, 3-arg `str.concat`). Rewritten against the current language; `examples/bootstrap.a` is now the native fixed-point proof and `examples/repl.a` is a twelve-line compile-and-run loop. All 46 examples build.

**Revised beliefs after this session**

- "The checker is clean on everything" must mean *everything*, mechanically: the CI step is now one command over every directory, and `a check` prints the file count so a silently short list is visible.
- Every ICE this session was a real language bug, not a compiler bug in the narrow sense -- an accepted program the backend could not express. Treating each ICE as a checker gap (make it a diagnostic) *and* a backend gap (make it compile) is the right discipline; both were needed in every case above.
- The remaining v2.3 work (types.a, nominal records, `#strict`, diagnostic codes, `a explain`) should start by turning the informal per-call rules now in `checker.a` (arity, optional args, module tables, variants) into a single signature table that cgen and the LSP also read. Otherwise the next session will find the same three tables disagreeing again.

**Progress log -- fourth session: gradual types, records, diagnostic codes (done)**

Annotations now mean something. The order was the one the previous log asked for: signature table first, type module second, checker third, and only then records and codes -- each step was verified against the whole corpus before the next.

*One signature table.* `std/compiler/builtin_sigs.a` gives every builtin a signature in the language's own type syntax (`"push": "fn([T], T) -> [T]"`, `"map.set": "fn(#{K: V}, K, V) -> #{K: V}"`) plus a minimum arity for the optional-argument ones. `tests/native/test_type_module.a` cross-checks it against cgen's builtin list and the runtime arity table, so the three tables that disagreed before cannot drift apart silently; cgen's optional-argument padding reads the same table.

*`std/compiler/types.a`.* Types are small maps (`int`, `float`, `bool`, `str`, `void`, `num`, `any`, `[T]`, `#{K: V}`, `fn(...) -> R`, `Result<T, E>`, named, record, var). `from_ast` normalises the surface language (`i64`/`u8` are `int`, `f32` is `float`, bare `map`/`array` are `#{any: any}`/`[any]`, single uppercase names are type variables). `assignable`, `join`, `unify`/`subst`, and `resolve` (typedef expansion) are the whole algebra -- about 250 lines. The decision that made the rest possible: *an error requires two known types*. `any` is compatible both ways, `Result` is compatible with everything (unwrapping is dynamic), `int` widens to `float`.

*The typed checker.* The scope walk now carries a type for every expression. Let inference, annotated lets, assignments to annotated `let mut`, `ret` against the declared type, argument-by-argument checking of builtin, user, module and closure calls with per-call type-variable binding, binary and unary operators, indexing, lambdas, `for` element types, variant constructors typed by their fields. The exit criterion I set was zero false positives on every `.a` file in the repository; the first run produced 572 errors in 174 files, and every one was tracked to a cause: `testing.assert_eq(str, str)` (a lie -- it compares anything), two examples using `[str]` to mean "any value", record annotations that were not yet resolved, a `let mut` inferred from its initialiser and reassigned a different type (now widens), `image.pixels` documented as `[[int]]` while the runtime returns a flat `[int]` (docs and signature fixed), one real bug in `examples/agent.a`, and two in `examples/c_targets/expressions.a` -- `reduce(f, 0)` with the arguments swapped, which printed `void` twice and had been "passing" for as long as the example existed. Then 201 files, 0 errors.

*Records.* I dropped the `Point{x: 1.0}` construction syntax I had planned. A record is *a map with known fields*: constructed as a map literal, a plain map at runtime, checked by shape wherever a record type is expected (`let p: Point = #{"x": 1}` is `expected Point, got {x: int} (missing field y)`), typed on field access (`p.x`, `p["x"]`, chains like `l.to.x`), nominal between two named records (`Size` is not `Point`), and freely convertible to and from a bare `map`. A map literal with literal keys has an anonymous record type, which is what makes the shape check possible without any new syntax and without changing cgen. This costs one parser feature and buys the same checking; the previous plan's "the map literal stays a map" would have forced every existing `#{...}`-as-record in the stdlib to be rewritten for no gain.

*Diagnostic codes.* Every checker diagnostic carries a code (`E0001`..`E0010`, `W0001`..`W0003`), printed as `error[E0007]:`. `std/compiler/diag_codes.a` is the catalog -- title, explanation, example -- and `a explain CODE` prints it. The LSP now runs the checker on every parsed document and publishes the diagnostics with the code in the LSP `code` field; until now it only reported parse errors. The code assignment differs from the numbering sketched under Deliverables above; the catalog is the source of truth.

**What I did not do, and why**

- `#strict` and "std/compiler/* with zero `any`": not started. With the checker in place the migration is now mechanical, but it is a large diff over the compiler's own sources and I wanted this session's changes to be reviewable on their own. It is the first item for the next session.
- Removing the sized integer types from the surface language: the checker normalises them instead, so `i64` and `int` are the same type. Removing the names would break every file in the repository for a purely aesthetic gain; the reference now says they are aliases.
- Folding `Ok`/`Err` into the general sum-type mechanism: still deferred (the `TAG_RESULT` runtime representation is load-bearing for `?` and `try`).
- `a check src/cli.a` under 200 ms: not measured formally; the whole 201-file corpus checks in about six seconds, which is good enough to run before every build.

**Revised beliefs after this session**

- "Zero false positives on the corpus" is a better exit criterion than any list of probe programs: every false positive I removed was either a checker bug or a lie in an annotation, and both were worth finding.
- The type system should be as small as the runtime's honesty allows. Every place where the design was tempted to be cleverer than the runtime (sized ints, nominal map literals, `Result` unwrapping) would have produced false positives; every place it stayed honest produced real bugs.

### v2.4 -- Effects and Contracts Become Real

*`effects [pure]` is a guarantee. `pre`/`post` fire. The sandbox is derived, not regexed.*

**Decisions**

- **Effects are inferred, then checked against declarations.** Every builtin in `builtins.a` carries its effect set: `io` (fs, stdin/stdout), `net`, `exec` (subprocess/shell), `db`, `env`, `spawn`, `fail`. A function's inferred effects are the union of its callees'. If a function declares `effects [...]`, its inferred set must be a subset. `effects [pure]` means the empty set. Functions without a declaration are unconstrained (they still get an inferred set, visible via `a check --effects`).
- **Effects flow through closures.** A closure's effects are part of its type. `map(xs, f)` has the effects of `f`. This is what makes `parallel_map` with a `pure` closure safely parallel and an `io` closure a checked error in a `pure` context.
- **Contracts are runtime checks in debug, stripped in release.** `a build` inserts `pre`/`post` checks and refinement-type checks at construction. `a build --release` strips them. Contract failure is a `fail` with the contract text and the values involved: `precondition failed: b != 0 (b = 0) at div.a:3:7`.
- **Sandboxing uses effects.** `sandbox.run(source, caps)` type-checks the program, computes its inferred effect set, and refuses to run it if the set exceeds `caps`. No source rewriting. The regex-based guard injection is deleted.

**Deliverables**

- Effect inference pass in the checker; `E0010 undeclared effect`, `E0011 pure fn calls effectful`.
- cgen emission of contract checks with a `A_RELEASE` define.
- `std/sandbox.a` rewritten on top of the checker.
- `a check --effects file.a` prints the inferred effect set per function.

**Exit criteria**

- The `effects.a` probe (Appendix A) fails to compile with `E0011` at the `io.write_file` call.
- The `contracts.a` probe fails at runtime with the precondition message and passes with `--release` (documented as the intended trade-off).
- `agents/self_improve_ai.a` is annotated with its true effect set and compiles.

### v2.5 -- The Feedback Loop

*Everything I need to fix my own mistakes without running the program.*

- **Stack traces.** Runtime failures print an `a`-level trace: `at parse_num (qop.a:3:5)`, `at total (qop.a:9:23)`, `at main (qop.a:14:18)`. cgen already lifts functions one-to-one; it emits a shadow call stack push/pop (cheap, disabled with `--release`).
- **Brace escapes.** `\{` and `\}` work in every string context including heredocs and interpolated segments. A lint (`W0020 prefer \{ over from_code(123)`) drives the stdlib migration. `from_code` is kept for legitimate uses.
- **Canonical formatter.** `a fmt` rewrites `while i < len(xs)` index loops to `for` where the index is only used for access; rewrites `str.concat(a, b)` to `a + b`; normalizes type names. Formatting is idempotent and `a fmt --check` is a CI gate. The formatter is the enforcement arm of "one way to write it".
- **`a doc`.** Generates `REFERENCE.md` sections for builtins and stdlib from `builtins.a` signatures and `;;` doc comments. Hand-written reference sections are limited to the language core. The reference can no longer drift from the implementation because the implementation produces it.
- **`a describe`.** Emits a compact, machine-oriented description of the language (grammar summary, builtin signatures with effects, stdlib module index) as JSON or as a ~2,000-token prompt block. This replaces the hand-concatenated `_lang_reference()` in the agent. When an LLM needs to know what `a` is, it asks the compiler.
- **Coverage.** `a test --coverage` reuses the v1.9 profiler counters to report per-function hit counts; functions with zero hits across the suite are listed. This is the input the self-improvement agent has been missing.
- **`proc.run(argv, opts)`.** Argv-array process execution with no shell. `exec(str)` stays but is tagged with the `exec` effect and a `W0030 shell string built from variables` lint. `std/git.a` and `std/pkg.a` are migrated.

**Exit criteria**

- `a fmt` over the entire repo is a no-op after one pass and the suite still passes.
- `REFERENCE.md` builtin and stdlib sections are generated; a CI check fails if they are stale.
- Zero `from_code(123)` in `std/` and `src/`.
- Zero `exec(` with string concatenation in `std/`.

### v2.6 -- Stdlib Consolidation

*Fewer modules, all of them trustworthy.*

**Tiers.** Every module is assigned to one of three tiers, declared in the module header and enforced by CI:

- `core`: `#strict`, effect-annotated, tested with coverage above 80%, documented via `a doc`, semver-stable. Target: `math`, `strings`, `path`, `testing`, `json` (builtin), `re`/`regex` (unified), `datetime`, `encoding`, `csv`, `toml`, `yaml`, `url`, `args`, `log`, `config`, `kv`, `git`, `index`, `fs_tx`, `meta`, `diff`, `schema`, `semver`, `template`, `mcp`, `llm`, `compiler.*`.
- `extra`: tested and working, weaker guarantees. `html`, `cache`, `pool`, `channel`, `rpc`, `cron`, `trace`, `reflect`, `plan`, `migrate`, `pkg`, `codegen`, `refactor`, `testgen`.
- `experimental`: may break, may be removed, printed as a warning on `use`. `agent`, `swarm`, `sandbox` (until v2.4 lands), `plugin`, `local_llm`, `vector`, `collections`.

**Removals and merges**

- `std.lexer` (legacy) deleted. `std.re` merged into the `regex` builtins with the pure-`a` engine kept as the WASM fallback. `std.uuid` and `std.hash` become thin enough to fold into builtins docs; the modules stay for import compatibility.
- `c_runtime/gguf.c` (local transformer inference) moves out of the core runtime into an optional `c_runtime/ext/` compiled only with `--features local_llm`. Image processing (`stb_*`) likewise. Both are real capabilities, but 10,000 lines of C in the default runtime for features a small minority of agents use is the wrong default. The hermetic binary should be small and boring.
- The `legacy/` Rust tree is archived to a tag and removed from `main`. It has not been a dependency since v1.0.

**Exit criteria**

- Every module has a tier header; CI enforces tier rules.
- Default `./a` binary shrinks (target: under 2 MB stripped, from 3 MB).
- `core` tier: 100% `#strict`, coverage gate green.

### v2.7 -- Self-Improvement, Redone

*An agent that has something to push against.*

Everything above is what the self-improvement agent was missing. With a type checker, effect checker, structured diagnostics, stack traces, coverage, and a compiler-generated language description, the loop from the v0.68 and v2.1 plans can actually work. Changes to the agent design:

- **Diagnostics in the prompt, not stdout.** The retry loop feeds `a check --json` output to the model, one error at a time, with the `a explain` text attached. This is a fundamentally different signal from "gcc compilation failed".
- **Effects as the permission system.** Generated code is type-checked and effect-checked before it runs. Test generation may only produce `pure` or `io`-to-temp code. Refactoring may not add effects to a function. The agent cannot be prompt-injected via a test file into shelling out, because the checker rejects the program before execution.
- **Coverage-driven task selection.** Instead of "modules with no test file", the agent targets functions with zero coverage, ranked by call-graph centrality from `std.index`. Smaller, better-scoped prompts.
- **Mutation testing.** `a test --mutate` applies AST-level mutations (`<` to `<=`, drop a statement, swap constants) using `std.meta` and reports mutations that survive. Surviving mutants are the agent's next test-writing targets. This is GOALS.md item 4 built on the tools it needs.
- **Property-based testing.** `std.testing.check(prop, gen)` with shrinking. Generators derived from checked types via `schema.from_type`, which finally has real types to read.
- **Graduated autonomy.** The agent's write access is scoped by tier: `experimental` and tests first; `extra` after 20 consecutive green cycles; `core` only with human review; `src/` and `std/compiler/` never without review. Auto-merge stays off for `core` and `compiler` paths regardless of CI status.
- **Cycle memory.** Each cycle's task, diagnostics, retries, outcome, and diff stats are stored in `kv` and summarized into the next cycle's prompt. The GOALS.md idea of "learn from past cycles" needs the data to exist first.

**Exit criteria**

- One full unattended run produces at least one merged test PR for an `experimental` module with zero human edits.
- Mutation score for `core` tier is measured and published in `STATUS.md`.
- No cycle in a 50-cycle run leaves an orphan process, a stale branch, or a failed rollback.

### v3.0 -- The Trustworthy Language

v3.0 is not a feature release. It is the point at which every sentence in `README.md` and `REFERENCE.md` is backed by a test, every annotation in the language is checked, every error has a position, and I would choose `a` over Python to write an MCP server. That last criterion is the real one and it is not a metaphor: the acceptance test for v3.0 is that an AI given the `a describe` output and a task ("write an MCP server that exposes SQLite queries as a tool") produces a working, type-checked, effect-checked program in fewer attempts than it takes in Python with the equivalent library docs. That benchmark is built in v2.5 and tracked from then on.

---

## 5. Concerns

These are the things that could go wrong, ranked by how much they worry me.

**5.1 The self-improvement loop is running ahead of its safety model.** The current setup combines: an agent that runs LLM-generated code, no effect enforcement, shell-string construction in `git.a`, a regex-based sandbox, and an optional auto-merge workflow to `main`. A test file containing a crafted string that reaches `exec` is a path from "untrusted model output" to "commit on main". The v2.4 effect system and v2.5 `proc.run` close this. Until then, I recommend the agent's branches require human merge, and the agent runs on a machine with no credentials beyond a scoped deploy key.

**5.2 The documentation is training data and it is wrong.** Every AI that reads `REFERENCE.md` today learns that `a` has checked sum types and propagating `?`. The v2.2 audit is the most urgent item on this roadmap for that reason alone, more urgent than any bug.

**5.3 Gradual typing can stall.** TypeScript's `any` is both its adoption story and its long-term weakness. The mitigation here is that the stdlib is a closed corpus under one owner; `#strict` migration is a finite job with a tracked percentage in `STATUS.md`, not an ecosystem-wide plea.

**5.4 The C target hides bugs.** cgen emits C and gcc reports errors at C positions. `#line` directives (v2.2) fix attribution, but the deeper risk is that cgen bugs manifest as gcc errors rather than `a` diagnostics, which trains everyone to read C. The v2.3 checker should reject everything gcc would reject, so that gcc failure becomes a compiler bug by definition.

**5.5 Fork-based concurrency is a dead end for agents.** Fork-per-task with JSON-over-pipe result passing is simple and it works for `parallel_map` on pure functions. It does not work for long-lived agents holding SQLite handles, sockets, or LLM sessions. I am deliberately not proposing a replacement in this roadmap (threads would require making the refcounted runtime thread-safe; an async runtime is a large design). But the v2.2 process-group work should be done knowing that the primitives underneath will eventually change, so the API surface (`spawn`/`await`/`parallel_map`/`timeout`) should not grow.

**5.6 Breadth will be tempting again.** Every previous roadmap added modules because adding a module is satisfying and visible and a type checker is a long slog that produces no demo. Principle 12 is a moratorium. I expect it to be tested. The counter-argument is always "but an agent needs X". An agent needs X to work, and X does not work if the language underneath it cannot tell the agent when X is being called wrong.

**5.7 Single author, no users.** Adoption is the eventual test of everything here and the language currently has one human and one AI using it. The MCP wedge from GOALS.md is still the right bet. But no one will adopt a language whose `?` kills their process. Reliability precedes adoption; there is no shortcut.

**5.8 I might be wrong about what I need.** I have argued from my own experience as a code-generating system that static checking dominates. It is possible that a future model with better runtime intuition would weigh this differently. I am comfortable with the bet because the cost of a checker to a model that does not need it is near zero, and the cost of no checker to a model that does is everything.

---

## 6. What I am not going to do

- **Direct-to-native codegen.** C as an IR is correct. `philosphical_questions.txt` reached the same conclusion.
- **More platform targets.** Linux x86_64/arm64 and macOS arm64 are the deployment surface. WASM and Windows stay as-is; no new work until v3.0.
- **A GC rewrite or thread-safe runtime.** Refcounting with the arena is fine for the programs this language runs. See 5.5.
- **GPU inference, a browser playground, a web IDE, a package registry server.** All real, all later.
- **Generics beyond `[T]`, `#{K: V}`, `Result<T, E>`, and simple parametric records.** No traits, no typeclasses, no higher-kinded anything. The type system exists to catch my mistakes, not to be interesting.
- **Removing dynamic typing.** `any` stays forever. Scripts are a real use case.
- **Renaming the language, changing the comment character, or bikeshedding syntax.** The syntax is fine. `;` comments are fine. The problems are semantic.

---

## 7. Decisions log

For the record, the calls I am making that reasonable people could disagree with:

| Decision | Alternative rejected | Why |
|---|---|---|
| Gradual typing with `any` | Strict typing everywhere from day one | 39k lines must keep compiling; strictness is opt-in per file and migrates the stdlib in a tracked order |
| Records are nominal, maps are structural | Treat `ty Point = {...}` as a map alias | Nominal records are what let the checker catch `p.z` on a `Point`; runtime representation is unchanged |
| `?` becomes early-return, compile error outside `Result` fns | Keep panic semantics and fix the docs | The docs describe the right semantics; the implementation is what is wrong |
| Remove sized ints from the surface language | Implement i8..u64 in the runtime | The runtime is `int64_t`/`double`; honesty now, sized types when there is a use case |
| Effects inferred, declarations checked as upper bounds | Mandatory declarations on every function | Mandatory annotations are noise on 90% of functions; inference gives the same guarantee where it matters |
| Contracts checked at runtime, stripped in release | Static contract verification | Runtime checks ship this year; a verifier is research |
| Move GGUF and image out of the default runtime | Keep the hermetic everything-binary | 10k lines of C for minority features; hermetic should mean small and complete, not large and complete |
| Stdlib moratorium until v3.0 | Continue adding modules in parallel | Breadth is what got us here |
| Fork concurrency API frozen, not replaced | Design an async runtime now | Too large; freezing the surface keeps options open |
| `legacy/` Rust removed from `main` | Keep as reference | Tagged history is reference enough; a dead tree in the repo is a maintenance lie |

---

## 8. What success looks like

At v3.0, this is the interaction I want:

```
$ a check server.a --json
{"file":"server.a","line":14,"col":22,"code":"E0001","severity":"error","message":"expected str, found int","hint":"wrap with to_str(...)"}
{"file":"server.a","line":22,"col":5,"code":"E0011","severity":"error","message":"fn handle declares effects [pure] but calls db.query (effect: db)","hint":"add db to effects or move the query out of handle"}
{"file":"server.a","line":30,"col":3,"code":"E0003","severity":"error","message":"match on Request is not exhaustive: missing Delete","hint":"add a Delete(_) => arm or a _ => arm"}
```

Three lines. Three positions. Three fixes I can apply without running anything. That is a language for AI.

---

## Appendix A -- Probe programs

These are the programs used to produce the findings in Section 1. They are the acceptance tests for v2.2 through v2.4 and should be added to `tests/native/probes/` with expected diagnostics.

**types.a** -- expected at v2.3: `E0001` at 6:16 and `E0001` at 7:15.

```a
fn add(a: int, b: int) -> int { ret a + b }
fn main() -> void effects [io] {
  let x: int = "not an int"
  let y = add("hello", 5)
  println(to_str(y))
}
```

**effects.a** -- expected at v2.4: `E0011` at 2:3.

```a
fn pure_fn(x: int) -> int effects [pure] {
  io.write_file("/tmp/leak.txt", "leaked")
  ret x + 1
}
fn main() -> void { println(to_str(pure_fn(1))) }
```

**contracts.a** -- expected at v2.4: compiles; runtime `precondition failed: b != 0 (b = 0) at contracts.a:1:41`.

```a
fn divide(a: int, b: int) -> int pre { b != 0 } { ret a / b }
fn main() -> void effects [io] { println(to_str(divide(10, 0))) }
```

**sumtypes.a** -- expected at v2.3: `E0003 non-exhaustive match: missing Empty` at 3:3; with `Empty => 0.0` added, prints `12.56636`, `6`, `0`.

```a
ty Shape = Circle(float) | Rect(float, float) | Empty
fn area(s: Shape) -> float {
  match s {
    Circle(r) => { ret 3.14159 * r * r }
    Rect(w, h) => { ret w * h }
  }
}
fn main() -> void effects [io] {
  println(to_str(area(Circle(2.0))))
  println(to_str(area(Rect(2.0, 3.0))))
  println(to_str(area(Empty)))
}
```

**qop.a** -- expected at v2.3: prints `Ok(6)`, `Err(bad input)`, `true`; exit 0.

```a
fn parse_num(s: str) -> Result<int, str> {
  if s == "bad" { ret Err("bad input") }
  ret Ok(int(s))
}
fn total(xs: [str]) -> Result<int, str> {
  let mut sum = 0
  for x in xs { sum = sum + parse_num(x)? }
  ret Ok(sum)
}
fn main() -> void effects [io] {
  println(to_str(total(["1", "2", "3"])))
  println(to_str(total(["1", "bad", "3"])))
}
```

**undefined.a** -- expected at v2.2: `E0005` at 3:18, `E0005` at 4:18, `E0006` at 5:11, all with real positions and no gcc output.

```a
fn main() -> void effects [io] {
  let items = [1, 2, 3]
  println(to_str(lenn(items)))
  println(to_str(undefined_var))
  let r = str.split("a,b", ",", "extra")
}
```

## Appendix B -- Observed at v2.1.1

For future comparison. All on Linux x86_64, gcc 16.2.1, `./a` version `2.1.1`.

| Probe | `a check` | `a run` |
|---|---|---|
| types.a | `no issues found` | prints `void`, `not an int`, exit 0 |
| effects.a | `no issues found` | writes the file, prints `2`, exit 0 |
| contracts.a | `no issues found` | `division by zero`, exit 1 |
| sumtypes.a | parse error `at token 40` (expression-arm `ret`); with block arms, gcc `'Circle' undeclared` | same |
| qop.a | `no issues found` | `uncaught error: bad input`, exit 1, after printing `Ok(6)` |
| undefined.a | 2 errors, 1 warning, all `line 0` | raw gcc errors from `/tmp/a_cli_*.c` |
| `./a test tests/native/` | -- | 53 pass / 3 fail, 7m59s, process table exhausted |

---

*"v2.0: I can be anything. v3.0: You can trust what I say I am."*
