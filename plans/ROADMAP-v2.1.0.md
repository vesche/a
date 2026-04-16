# v2.1.0: The Working Agent

*Three stdlib modules that close the gap between "write code" and "ship code."*

Follows the completion of [ROADMAP-v1.1-to-v2.0.md](ROADMAP-v1.1-to-v2.0.md).

## Where We Are

v2.0 delivered the Agent OS: supervision, registry, swarms, self-update, local LLM inference, cross-compilation, profile-guided optimization, and 58 stdlib modules. An agent can persist, communicate, plan, and self-improve. The infrastructure is complete.

But there's a gap between what an agent *can do* and what an agent *actually needs to do well*. Every coding task follows the same pattern:

1. **Understand** the codebase
2. **Plan** the change
3. **Execute** the change safely
4. **Ship** the result

v2.0 gave us tools for steps 2-3 (plan.a, codegen.a, refactor.a, testgen.a). What's missing is the beginning and the end: deep understanding of code, and reliable version control.

## Principles

8. **Understanding precedes action.** An agent that modifies code it doesn't understand is just a random walk. The runtime should make structural comprehension as natural as text search.

9. **Safety is a primitive, not a pattern.** Snapshot-modify-test-rollback shouldn't be 15 lines of boilerplate. It should be a single function call.

10. **Version control is not optional.** Every professional coding workflow involves git. Shell-escaping `exec("git ...")` and parsing stdout is fragile. Git operations deserve structured data types.

---

## std/fs_tx.a -- Transactional File Operations [DONE]

The self-improvement agent (`agents/self_improve.a`) already does snapshot/rollback manually: read original, write new, test, restore on failure. This pattern appears everywhere agents modify files. Make it a primitive.

- **`fs_tx.begin(paths)`** -- snapshot listed files to temp directory, returns transaction handle
- **`fs_tx.commit(tx)`** -- discard snapshots, changes are permanent
- **`fs_tx.rollback(tx)`** -- restore all files to pre-transaction state
- **`fs_tx.run(paths, fn)`** -- convenience: begin, run fn, commit on Ok, rollback on Err or exception

Pure "a" module. No C runtime changes. Uses `fs.cp` for snapshots, temp directories for storage.

---

## std/git.a -- Structured Git Operations [DONE]

Every coding agent shells out to `exec("git ...")` and parses strings. This is fragile, untyped, and error-prone. Git operations should return structured maps.

- **Repository:** `init`, `clone`, `is_repo`, `root`
- **Status:** `status` returns `[{file, status}]`, `is_clean`, `untracked`, `tracked`
- **Diff:** `diff` returns `[{file, hunks: [{old_start, new_start, lines}]}]`, `diff_stat`, `changed_files`
- **Log:** `log` returns `[{hash, author, email, date, message}]`, `log_short`
- **Staging:** `add`, `add_all`, `reset`
- **Commits:** `commit`, `commit_all` -- returns `{hash, message}`
- **Branches:** `branch`, `branches`, `checkout`, `create_branch`, `delete_branch`, `merge` returns `{ok, conflicts}`
- **Remotes:** `remote`, `remote_url`, `push`, `pull`, `fetch`
- **Tags:** `tag`, `tag_annotated`, `tags`, `delete_tag`
- **Stash:** `stash`, `stash_pop`, `stash_list`
- **Inspection:** `head`, `head_short`, `show`, `blame`

Pure "a" module using `exec("git ...")` internally. All output parsed into structured maps. Git must be on PATH.

---

## std/index.a -- Codebase Intelligence [DONE]

An agent that greps for text finds strings. An agent that indexes code finds meaning. Built on `std/meta.a` (AST parsing), `std/kv.a` (persistence).

- **Indexing:** `create(project_path, opts)` walks project, parses all `.a` files, builds symbol table, call graph, import map. `update(idx, changed_files)` for incremental re-index.
- **Symbols:** `symbols`, `find(name)`, `find_fuzzy(pattern)`, `functions`, `types`, `symbols_in(file)`, `exports(file)`
- **Call Graph:** `callers(fn_name)`, `callees(fn_name)`, `call_graph()` -- who calls what, across all files
- **Dependencies:** `imports(file)`, `dependencies(file)`, `dependents(file)` -- module-level dependency tracking
- **Search:** `search(query)` -- text search with file and line info
- **Statistics:** `stats()` -- files, functions, types, modules, lines, unique imports
- **Persistence:** `save(idx, path)` / `load(path)` -- SQLite-backed via kv.a

Pure "a" module. Uses the self-hosted parser to understand code structurally, not just textually.

---

## Summary

| Module | Key Capability | Unlocks |
|--------|---------------|---------|
| `std/fs_tx.a` [DONE] | Transactional file ops | Safe code modification, automatic rollback |
| `std/git.a` [DONE] | Structured git operations | Reliable version control from agents |
| `std/index.a` [DONE] | Codebase intelligence | Structural understanding, call graphs, impact analysis |

**stdlib total: 61 modules.** 3 new modules, 39 new tests, `agents/self_improve.a` updated to use `fs_tx.run`.

---

## Usage

```a
use std.git
use std.index
use std.fs_tx

; Understand the project
let idx = index.create(".", #{})
let callers = index.callers(idx, "parse_config")

; Make changes safely
let result = fs_tx.run(["src/config.a", "src/main.a"], fn() {
  ; ... modify files ...
  let r = exec("./a test tests/")
  if r["code"] != 0 { ret Err("tests failed") }
  ret Ok("done")
})

; Ship the result
if is_ok(result) {
  git.add(".", ["src/config.a", "src/main.a"])
  git.commit(".", "refactor: simplify config parsing")
  git.push(".", "origin", "main")
}
```

Understand. Change safely. Ship.
