---
name: Structured Concurrency
overview: Add safe, structured concurrency to "a" via spawn/await/parallel_map builtins, enabling parallel I/O, batch processing, and concurrent workflows -- the biggest capability gap for real-world AI agent automation.
todos:
  - id: rc-to-arc
    content: Migrate Value from Rc to Arc in interpreter.rs, vm.rs, builtins.rs (mechanical find-replace)
    status: completed
  - id: arc-program
    content: Wrap CompiledProgram in Arc for zero-cost sharing between VMs
    status: completed
  - id: spawn-await
    content: Implement spawn, await, await_all builtins with TaskHandle value variant
    status: completed
  - id: parallel-map
    content: Implement parallel_map, parallel_each, timeout builtins
    status: completed
  - id: register-builtins
    content: Register all 6 new builtins in is_builtin and handle in do_call
    status: completed
  - id: concurrency-tests
    content: Create tests/test_concurrency.a with comprehensive test coverage
    status: completed
  - id: concurrency-demo
    content: Create examples/parallel_fetch.a demo and update PLANNING.md + REFERENCE.md
    status: completed
isProject: false
---

# v0.39 -- Structured Concurrency

## Motivation

Every real-world automation task I'd write in "a" -- fetching multiple APIs, processing files in batch, running concurrent shell commands -- is bottlenecked by sequential execution. Adding structured concurrency makes "a" genuinely powerful for the kinds of tasks AI agents actually do.

The design: **spawn tasks that run in isolated VMs, communicate only through return values.** No shared mutable state. No data races. No deadlocks. Safe by construction.

```a
let urls = ["https://api.a.com/data", "https://api.b.com/data", "https://api.c.com/data"]
let responses = parallel_map(urls, fn(url) => http.get(url))

let h1 = spawn(fn() => io.read_file("large1.txt"))
let h2 = spawn(fn() => io.read_file("large2.txt"))
let data1 = await(h1)
let data2 = await(h2)
```

## Phase 1: Make Value Thread-Safe (Rc to Arc)

`Value` currently uses `Rc<T>` in four places, making it `!Send`. Change to `Arc<T>`:

In [src/interpreter.rs](src/interpreter.rs) (`pub enum Value`):
- `String(Rc<String>)` --> `String(Arc<String>)`
- `Array(Rc<Vec<Value>>)` --> `Array(Arc<Vec<Value>>)`
- `Map(Rc<HashMap<String, Value>>)` --> `Map(Arc<HashMap<String, Value>>)`
- `VMClosure { name: Rc<String>, captures: Rc<Vec<Value>> }` --> `Arc` equivalents

Then update all use sites across:
- [src/interpreter.rs](src/interpreter.rs) -- Value definition + helpers
- [src/vm.rs](src/vm.rs) -- VM operations that construct/destructure Values
- [src/builtins.rs](src/builtins.rs) -- builtin functions that construct Values

This is a mechanical find-replace (`Rc::new` -> `Arc::new`, `use std::rc::Rc` -> `use std::sync::Arc`, etc.). No semantic changes.

## Phase 2: Arc-wrap CompiledProgram for Cheap Sharing

Change [src/vm.rs](src/vm.rs) `VM.program` from `CompiledProgram` to `Arc<CompiledProgram>` so spawned tasks share the program data at zero cost instead of deep-cloning all bytecode per task.

- `VM::new(program: CompiledProgram)` wraps in Arc internally
- Add `VM::new_shared(program: Arc<CompiledProgram>)` for spawned tasks
- `fn_index` is still built per-VM (cheap, just a HashMap of name->index)

## Phase 3: New Builtins

Six concurrency primitives, all implemented in [src/vm.rs](src/vm.rs) `do_call`:

- **`spawn(fn)`** -- Start a 0-arg function/closure in a new OS thread with its own VM. Returns an opaque `TaskHandle(id)`. The spawned VM shares the `Arc<CompiledProgram>`.

- **`await(handle)`** -- Block until the task completes. Returns `Ok(value)` on success, `Err(message)` on error. Consumes the handle (second await on same handle returns Err).

- **`await_all(handles)`** -- Wait for an array of handles. Returns array of Results in the same order. More efficient than awaiting one by one.

- **`parallel_map(arr, fn)`** -- Map a 1-arg function over array elements concurrently using OS threads. Returns array of results in order. Sugar for spawn-all + await-all.

- **`parallel_each(arr, fn)`** -- Like `parallel_map` but discards results. For side-effect-heavy concurrent work.

- **`timeout(ms, fn)`** -- Run a 0-arg function with a millisecond deadline. Returns `Ok(value)` if it completes in time, `Err("timeout")` if not.

### VM Internal State

Add to `VM` struct:
```rust
task_handles: HashMap<usize, JoinHandle<Result<Value, String>>>,
next_task_id: usize,
```

Add `Value::TaskHandle(usize)` variant to the Value enum.

### spawn implementation sketch

```rust
"spawn" if nargs == 1 => {
    let closure_val = self.stack.pop().unwrap();
    let program = Arc::clone(&self.program);
    let id = self.next_task_id;
    self.next_task_id += 1;
    let handle = std::thread::spawn(move || {
        let mut vm = VM::new_shared(program);
        vm.call_value(&closure_val, &[])
           .map_err(|e| e.message)
    });
    self.task_handles.insert(id, handle);
    self.stack.push(Value::TaskHandle(id));
}
```

### parallel_map implementation sketch

```rust
"parallel_map" if nargs == 2 => {
    let func = self.stack.pop().unwrap();
    let arr = self.stack.pop().unwrap();
    let items = arr.as_array();
    let program = Arc::clone(&self.program);
    let handles: Vec<_> = items.iter().map(|item| {
        let prog = Arc::clone(&program);
        let f = func.clone();
        let arg = item.clone();
        std::thread::spawn(move || {
            let mut vm = VM::new_shared(prog);
            vm.call_value(&f, &[arg]).map_err(|e| e.message)
        })
    }).collect();
    let results: Vec<Value> = handles.into_iter().map(|h| {
        match h.join().unwrap() {
            Ok(v) => Value::ok(v),
            Err(e) => Value::err(Value::string(e)),
        }
    }).collect();
    self.stack.push(Value::array(results));
}
```

## Phase 4: Register Builtins

In [src/builtins.rs](src/builtins.rs), add `"spawn"`, `"await"`, `"await_all"`, `"parallel_map"`, `"parallel_each"`, `"timeout"` to `is_builtin()`.

## Phase 5: Tests

Create [tests/test_concurrency.a](tests/test_concurrency.a) covering:
- `spawn` + `await` basic (spawn returns handle, await returns value)
- `spawn` with closure captures (captures transferred to child VM)
- `await` error handling (child VM error becomes Err result)
- `await_all` with multiple handles
- `parallel_map` basic (results in correct order)
- `parallel_map` with closures
- `parallel_map` error in one element (doesn't crash others)
- `parallel_each` for side effects
- `timeout` success (completes before deadline)
- `timeout` expiry (returns Err)
- Independence: spawned tasks don't share state with parent

## Phase 6: Demo + Docs

- **`examples/parallel_fetch.a`** -- Practical demo fetching multiple URLs concurrently, showing speedup vs sequential
- **PLANNING.md** updated with v0.39 milestone
- **REFERENCE.md** updated with concurrency builtins section
