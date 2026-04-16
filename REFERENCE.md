# The "a" Language Reference

A programming language written by AI, for AI.

---

## 1. Quick Start

```a
fn main() -> void effects [io] {
  println("hello, world")
}
```

Save as `hello.a`, then run:

```sh
a run hello.a
```

### CLI Commands

| Command | Description |
|---------|-------------|
| `a run file.a [args...]` | Compile and run (cached -- instant on repeat) |
| `a build file.a [-o out]` | Compile to native binary via C |
| `a build file.a --target T` | Cross-compile (WASM, Linux, Windows, macOS) |
| `a cc file.a [-o out]` | Emit generated C to stdout (or file with `-o`) |
| `a wat file.a [-o out]` | Emit WebAssembly Text Format |
| `a targets` | List cross-compile targets and detected toolchains |
| `a test dir/` | Find `test_*.a` files, compile, run, report |
| `a check file.a` | Static analysis (undefined vars, arity, unused) |
| `a fmt file.a` | Format to canonical style |
| `a fmt dir/` | Format all `.a` files in directory |
| `a ast file.a` | Dump parsed AST as JSON |
| `a eval "expr"` | Evaluate an expression |
| `a repl` | Interactive read-eval-print loop |
| `a watch file.a` | Recompile and re-run on file change |
| `a lsp` | Build the language server binary (`./a-lsp`) |
| `a profile file.a` | Instrument, run, dump profile JSON |
| `a gentests file.a` | Auto-generate tests from source analysis |
| `a optimize file.a prof.json` | Optimization report from profile data |
| `a agent file.a --name N` | Deploy supervised long-running agent |
| `a spawn file.a --name N` | Build and launch named agent process |
| `a plugin install dir` | Install a plugin from directory |
| `a plugin list` | List installed plugins |
| `a plugin run name` | Execute an installed plugin |
| `a pkg init` | Create `pkg.toml` manifest |
| `a pkg add name source` | Add a dependency |
| `a pkg install` | Install all dependencies |
| `a cache clean` | Clear the compilation cache |

---

## 2. Syntax Basics

### Comments

Line comments start with `;`:

```a
; this is a comment
let x = 42  ; inline comment
```

There are no block comments.

### Newlines

Newlines are significant -- they separate statements. No semicolons needed.

```a
let x = 1
let y = 2
```

### Top-Level Forms

Only four forms are allowed at the top level of a file:

- `fn` -- function declaration
- `ty` -- type declaration
- `mod` -- module
- `use` -- import

### Entry Point

Programs must define `fn main()` as the entry point. Test files define `fn test_*()` functions instead.

---

## 3. Types

### Primitive Types

| Type | Description |
|------|-------------|
| `i8`, `i16`, `i32`, `i64` | Signed integers |
| `u8`, `u16`, `u32`, `u64` | Unsigned integers |
| `f32`, `f64` | Floating point |
| `bool` | Boolean (`true` / `false`) |
| `str` | String |
| `bytes` | Byte array |
| `void` | No value |

### Arrays

Type: `[T]`. Literals: `[1, 2, 3]`. Spread: `[...existing, 4, 5]`.

```a
let nums: [i64] = [1, 2, 3]
let more = [...nums, 4, 5]
```

### Maps

Type: `#{K: V}`. Literals: `#{"key": value}`.

```a
let config: #{str: str} = #{"host": "localhost", "port": "8080"}
```

### Tuples

Type: `(T, U)`. Used in type annotations.

### Function Types

Type: `fn(T, U) -> R`.

```a
fn apply(f: fn(i64) -> i64, x: i64) -> i64 {
  ret f(x)
}
```

### Type Declarations

**Records** (named structs):

```a
ty Point = {x: f64, y: f64}
```

**Sum types** (tagged unions):

```a
ty Shape = Circle(f64) | Rect(f64, f64) | Empty
```

**Aliases with constraints**:

```a
ty PositiveInt = i64 where { val > 0 }
```

**Generic types**:

```a
ty Pair<A, B> = {first: A, second: B}
```

---

## 4. Variables and Bindings

### Let Bindings

```a
let name = "alice"
let age: i64 = 30
```

### Mutable Bindings

```a
let mut count = 0
count = count + 1
```

### Destructuring

Array destructuring with `_` (skip) and `...` (rest):

```a
let [first, _, third] = [1, 2, 3]
let [head, ...tail] = [1, 2, 3, 4]
; head = 1, tail = [2, 3, 4]
```

---

## 5. Functions

### Declaration

```a
fn add(a: i64, b: i64) -> i64 {
  ret a + b
}
```

Return type can be omitted (inferred):

```a
fn greet(name) {
  println("hello {name}")
}
```

### Lambdas

Expression form with `=>`:

```a
let double = fn(x) => x * 2
```

Block form with `{}`:

```a
let process = fn(x) {
  let result = x * 2
  ret result + 1
}
```

### Effects

Declare side effects a function may perform:

```a
fn save(data: str) -> void effects [io] {
  io.write_file("out.txt", data)
}
```

### Contracts

Pre/postconditions on functions:

```a
fn divide(a: f64, b: f64) -> f64
  pre { b != 0.0 }
  post { true }
{
  ret a / b
}
```

---

## 6. Control Flow

### If / Else

```a
if x > 0 {
  println("positive")
} else if x == 0 {
  println("zero")
} else {
  println("negative")
}
```

### Match

Pattern matching with `=>`:

```a
match value {
  0 => println("zero")
  1 => println("one")
  n if n > 100 => println("big")
  _ => println("other")
}
```

Match with block bodies:

```a
match shape {
  Circle(r) => {
    let area = 3.14159 * r * r
    println("area: {to_str(area)}")
  }
  Rect(w, h) => println("area: {to_str(w * h)}")
  Empty => println("no shape")
}
```

### For Loops

```a
for x in [1, 2, 3] {
  println(to_str(x))
}
```

With type annotation:

```a
for name: str in names {
  println(name)
}
```

With destructuring:

```a
for [key, val] in map.entries(config) {
  println("{key} = {val}")
}
```

### While Loops

```a
let mut i = 0
while i < 10 {
  println(to_str(i))
  i = i + 1
}
```

### Break and Continue

```a
while true {
  let line = io.read_line()
  if line == "quit" { break }
  if len(line) == 0 { continue }
  println(line)
}
```

### Return

```a
fn find_first(arr, target) {
  let mut i = 0
  while i < len(arr) {
    if arr[i] == target { ret i }
    i = i + 1
  }
  ret -1
}
```

---

## 7. Expressions and Operators

### Precedence (low to high)

| Level | Operators |
|-------|-----------|
| 1 | `\|>` (pipe) |
| 2 | `\|\|` (logical or) |
| 3 | `&&` (logical and) |
| 4 | `==` `!=` |
| 5 | `<` `>` `<=` `>=` |
| 6 | `+` `-` |
| 7 | `*` `/` `%` |
| 8 | `-` `!` `try` (unary) |
| 9 | `()` `.` `[]` `?` (postfix) |

### Pipe Operator

Passes the left side as the first argument to the right side:

```a
let result = [1, 2, 3, 4, 5]
  |> filter(fn(x) => x > 2)
  |> map(fn(x) => x * 10)
```

Newlines before `|>` are allowed for chaining.

### Try / Error Propagation

```a
let data = try io.read_file("config.txt")
let data = io.read_file("config.txt")?
```

`try { block }` catches errors in the block.

### Field Access, Indexing, Calls

```a
point.x           ; field access
arr[0]            ; indexing
println("hello")  ; function call
```

---

## 8. Strings

### Regular Strings

```a
let s = "hello, world"
```

Escape sequences: `\n` (newline), `\t` (tab), `\r` (carriage return), `\\` (backslash), `\"` (quote), `\{` (literal brace), `\}` (literal brace).

### String Interpolation

Expressions inside `{}` are evaluated and converted to strings:

```a
let name = "alice"
let age = 30
println("name: {name}, age: {to_str(age)}")
println("sum: {to_str(1 + 2)}")
```

### Raw Strings

No escape processing, no interpolation:

```a
let path = r"C:\Users\name\file.txt"
let code = r#"she said "hello""#
let nested = r##"contains "# inside"##
```

### Heredocs

Triple-backtick multiline strings. No escapes, no interpolation. Leading newline after opener and trailing newline before closer are stripped.

````a
let program = ```
fn fib(n) {
  if n < 2 { ret n }
  ret fib(n - 1) + fib(n - 2)
}
fn main() { ret fib(10) }
```
````

---

## 9. Pattern Matching

### Pattern Forms

| Pattern | Matches |
|---------|---------|
| `_` | Anything (discard) |
| `42`, `3.14`, `"hello"`, `true` | Literal values |
| `name` | Binds value to `name` |
| `[a, b, c]` | Array with exactly 3 elements |
| `[head, ...rest]` | Array with 1+ elements, rest bound |
| `[_, ...rest]` | Skip first, bind rest |
| `#{"key": val}` | Map with key, bind its value |
| `Some(x)` | Constructor with inner value |
| `None` | Constructor with no value |

### Guards

```a
match n {
  x if x > 0 => println("positive")
  x if x < 0 => println("negative")
  _ => println("zero")
}
```

---

## 10. Modules and Imports

### Defining Modules

```a
mod math_utils {
  fn square(x: i64) -> i64 {
    ret x * x
  }
}
```

### Importing

```a
use std.testing
use std.math
use std.cli
```

After import, access functions with dotted names:

```a
use std.math
let nums = std.math.range(1, 10)
```

---

## 11. Error Handling

### Result Values

```a
let ok_val = Ok(42)
let err_val = Err("something failed")
```

### Checking Results

```a
if is_ok(result) {
  let val = unwrap(result)
}

let val = unwrap_or(result, "default")
let val = expect(result, "should not fail")
```

### Try

```a
; propagate errors
let data = io.read_file("config.txt")?

; catch errors
let result = try io.read_file("missing.txt")
if is_err(result) {
  println("file not found")
}
```

---

## 12. Builtin Functions Reference

### I/O

| Function | Signature | Description |
|----------|-----------|-------------|
| `print(val)` | any -> void | Print without newline |
| `println(val)` | any -> void | Print with newline |
| `eprintln(val)` | any -> void | Print to stderr |
| `io.write(val)` | any -> void | Write to stdout |
| `io.read_file(path)` | str -> str | Read file contents (or Err) |
| `io.write_file(path, data)` | str, str -> void | Write file (or Err) |
| `io.read_line()` | -> str | Read one line from stdin |
| `io.read_stdin()` | -> str | Read all of stdin |

### Conversion

| Function | Signature | Description |
|----------|-----------|-------------|
| `to_str(val)` | any -> str | Convert any value to string |
| `type_of(val)` | any -> str | Returns `"int"`, `"float"`, `"str"`, `"bool"`, `"void"`, `"array"`, `"map"`, `"fn"`, `"result"` |
| `int(val)` | str/float/bool -> i64 | Parse or coerce to integer |
| `float(val)` | str/int -> f64 | Parse or coerce to float |

### Math

| Function | Signature | Description |
|----------|-----------|-------------|
| `len(val)` | str/array/map -> i64 | Length of string (chars), array, or map |
| `sqrt(n)` | float -> float | Square root |
| `abs(n)` | int/float -> int/float | Absolute value |
| `floor(n)` | float -> i64 | Floor to integer |
| `ceil(n)` | float -> i64 | Ceiling to integer |
| `round(n)` | float -> i64 | Round to nearest integer |

### Array Operations

| Function | Signature | Description |
|----------|-----------|-------------|
| `push(arr, val)` | [T], T -> [T] | Append element (returns new array) |
| `slice(arr, start, end?)` | [T], i64, i64? -> [T] | Sub-array from start to end |
| `sort(arr)` | [T] -> [T] | Sort (int/float/str) |
| `reverse_arr(arr)` | [T] -> [T] | Reverse |
| `contains(arr, val)` | [T], T -> bool | Membership test |
| `concat_arr(a, b)` | [T], [T] -> [T] | Concatenate two arrays |

### Higher-Order Array Functions

| Function | Signature | Description |
|----------|-----------|-------------|
| `map(arr, f)` | [T], fn(T)->U -> [U] | Transform each element |
| `filter(arr, f)` | [T], fn(T)->bool -> [T] | Keep elements where f is true |
| `reduce(arr, init, f)` | [T], U, fn(U,T)->U -> U | Fold left |
| `each(arr, f)` | [T], fn(T)->void -> void | Side-effect on each element |
| `sort_by(arr, f)` | [T], fn(T,T)->i64 -> [T] | Sort with comparator |
| `find(arr, f)` | [T], fn(T)->bool -> Result | First match (Ok) or Err |
| `any(arr, f)` | [T], fn(T)->bool -> bool | True if any element matches |
| `all(arr, f)` | [T], fn(T)->bool -> bool | True if all elements match |
| `flat_map(arr, f)` | [T], fn(T)->[U] -> [U] | Map then flatten one level |
| `min_by(arr, f)` | [T], fn(T)->K -> Result | Min by key function |
| `max_by(arr, f)` | [T], fn(T)->K -> Result | Max by key function |
| `zip(a, b)` | [T], [U] -> [[T,U]] | Pair elements by index |
| `enumerate(arr)` | [T] -> [[i64,T]] | Pair each element with its index |
| `take(arr, n)` | [T], i64 -> [T] | First n elements |
| `drop(arr, n)` | [T], i64 -> [T] | All but first n elements |
| `chunk(arr, size)` | [T], i64 -> [[T]] | Split into chunks of size |
| `unique(arr)` | [T] -> [T] | Remove duplicates (first occurrence) |

### Map Operations

| Function | Signature | Description |
|----------|-----------|-------------|
| `map.get(m, key)` | map, str -> any | Get value (or void if missing) |
| `map.set(m, key, val)` | map, str, any -> map | Set key (returns new map) |
| `map.keys(m)` | map -> [str] | All keys |
| `map.values(m)` | map -> [any] | All values |
| `map.has(m, key)` | map, str -> bool | Key exists |
| `map.delete(m, key)` | map, str -> map | Remove key (returns new map) |
| `map.merge(a, b)` | map, map -> map | Merge (b overwrites a) |
| `map.entries(m)` | map -> [[str, any]] | Key-value pairs |
| `map.from_entries(arr)` | [[str, any]] -> map | Build map from pairs |

### String Operations

| Function | Signature | Description |
|----------|-----------|-------------|
| `str.concat(a, b, ...)` | str... -> str | Concatenate strings |
| `str.split(s, delim)` | str, str -> [str] | Split on delimiter |
| `str.contains(s, sub)` | str, str -> bool | Substring test |
| `str.starts_with(s, pre)` | str, str -> bool | Prefix test |
| `str.ends_with(s, suf)` | str, str -> bool | Suffix test |
| `str.replace(s, from, to)` | str, str, str -> str | Replace all occurrences |
| `str.trim(s)` | str -> str | Strip leading/trailing whitespace |
| `str.upper(s)` | str -> str | Uppercase |
| `str.lower(s)` | str -> str | Lowercase |
| `str.join(arr, sep)` | [str], str -> str | Join array with separator |
| `str.chars(s)` | str -> [str] | Split into single characters |
| `str.slice(s, start, end)` | str, i64, i64 -> str | Substring by char indices |
| `str.lines(s)` | str -> [str] | Split on newlines |
| `str.find(s, sub)` | str, str -> i64 | Index of substring (-1 if not found) |
| `str.count(s, sub)` | str, str -> i64 | Count occurrences |

### Regex

All patterns are strings using standard regex syntax. Invalid patterns produce runtime errors.

| Function | Signature | Description |
|----------|-----------|-------------|
| `regex.is_match(pat, text)` | str, str -> bool | Test if pattern matches anywhere |
| `regex.find(pat, text)` | str, str -> map/false | First match: `#{"match": str, "start": i64, "end": i64}` or `false` |
| `regex.find_all(pat, text)` | str, str -> [str] | All matching substrings |
| `regex.replace(pat, text, rep)` | str, str, str -> str | Replace first match |
| `regex.replace_all(pat, text, rep)` | str, str, str -> str | Replace all matches |
| `regex.split(pat, text)` | str, str -> [str] | Split on pattern |
| `regex.captures(pat, text)` | str, str -> [str]/false | Capture groups (index 0 = full match) or `false` |

```a
regex.is_match(r"\d+", "abc123")         ; true
regex.find(r"\d+", "abc123def")          ; #{"match": "123", "start": 3, "end": 6}
regex.find_all(r"\d+", "a1b22c333")      ; ["1", "22", "333"]
regex.replace_all(r"\s+", "a  b  c", " ") ; "a b c"
regex.split(r",\s*", "one, two, three")  ; ["one", "two", "three"]
regex.captures(r"(\d{4})-(\d{2})", "2024-03") ; ["2024-03", "2024", "03"]
```

### JSON

| Function | Signature | Description |
|----------|-----------|-------------|
| `json.parse(s)` | str -> any | Parse JSON string to value |
| `json.stringify(val)` | any -> str | Serialize to compact JSON |
| `json.pretty(val)` | any -> str | Serialize to pretty-printed JSON |

### HTTP

All HTTP functions return a record: `{status: i64, body: str, headers: #{str: str}}`.

| Function | Signature | Description |
|----------|-----------|-------------|
| `http.get(url, headers?)` | str, map? -> record | GET request |
| `http.post(url, body?, headers?)` | str, any?, map? -> record | POST request |
| `http.put(url, body?, headers?)` | str, any?, map? -> record | PUT request |
| `http.patch(url, body?, headers?)` | str, any?, map? -> record | PATCH request |
| `http.delete(url, headers?)` | str, map? -> record | DELETE request |

```a
let resp = http.get("https://api.example.com/data", #{"Accept": "application/json"})
if resp.status == 200 {
  let data = json.parse(resp.body)
}
```

### Filesystem

| Function | Signature | Description |
|----------|-----------|-------------|
| `fs.exists(path)` | str -> bool | Path exists |
| `fs.is_dir(path)` | str -> bool | Is directory |
| `fs.is_file(path)` | str -> bool | Is file |
| `fs.ls(path)` | str -> [{name, is_dir}] | List directory entries |
| `fs.mkdir(path)` | str -> void | Create directory |
| `fs.rm(path)` | str -> void | Remove file or directory |
| `fs.mv(from, to)` | str, str -> void | Move/rename |
| `fs.cp(from, to)` | str, str -> void | Copy |
| `fs.cwd()` | -> str | Current working directory |
| `fs.abs(path)` | str -> str | Absolute path |
| `fs.glob(pattern)` | str -> [str] | Glob match from cwd |

### Environment

| Function | Signature | Description |
|----------|-----------|-------------|
| `env.get(name)` | str -> str/void | Get environment variable |
| `env.set(name, val)` | str, str -> void | Set environment variable |
| `env.all()` | -> map | All environment variables |

### Shell

| Function | Signature | Description |
|----------|-----------|-------------|
| `exec(cmd)` | str -> {stdout, stderr, code} | Run shell command |

```a
let result = exec("ls -la")
println(result.stdout)
if result.code != 0 {
  eprintln(result.stderr)
}
```

### Character Functions

| Function | Signature | Description |
|----------|-----------|-------------|
| `char_code(s)` | str -> i64 | Code point of first character |
| `from_code(n)` | i64 -> str | Character from code point |
| `is_alpha(s)` | str -> bool | All characters are alphabetic |
| `is_digit(s)` | str -> bool | All characters are digits |
| `is_alnum(s)` | str -> bool | All characters are alphanumeric |

### Result Helpers

| Function | Signature | Description |
|----------|-----------|-------------|
| `Ok(val)` | any -> Result | Construct success result |
| `Err(msg)` | any -> Result | Construct error result |
| `is_ok(val)` | any -> bool | True if Ok result |
| `is_err(val)` | any -> bool | True if Err result |
| `unwrap(result)` | Result -> any | Extract Ok value (error if Err) |
| `unwrap_or(result, default)` | Result, any -> any | Extract Ok or use default |
| `expect(result, msg)` | Result, str -> any | Extract Ok (error with msg if Err) |

### Runtime

| Function | Signature | Description |
|----------|-----------|-------------|
| `eval(code, args...)` | str, any... -> any | Compile and run "a" code at runtime |
| `args()` | -> [str] | Program command-line arguments |
| `exit(code?)` | i64? -> never | Terminate with exit code (default 0) |
| `fail(msg?)` | str? -> never | Abort with runtime error |

`eval` auto-wraps bare expressions: `eval("1 + 2")` returns `3`. Full programs work too:

```a
let result = eval(r#"
  fn fib(n) {
    if n < 2 { ret n }
    ret fib(n - 1) + fib(n - 2)
  }
  fn main() { ret fib(10) }
"#)
; result = 55
```

Errors return strings starting with `"eval error:"` instead of crashing.

### Concurrency

| Function | Signature | Description |
|----------|-----------|-------------|
| `spawn(fn)` | fn -> task | Start function in a new OS thread, returns TaskHandle |
| `await(handle)` | task -> Result | Block until task completes, returns Ok(value) or Err(msg) |
| `await_all(handles)` | [task] -> [Result] | Wait for all tasks, returns array of Results |
| `parallel_map(arr, fn)` | [T], fn -> [U] | Map function over array concurrently |
| `parallel_each(arr, fn)` | [T], fn -> void | Execute function for each element concurrently |
| `timeout(ms, fn)` | i64, fn -> Result | Run function with millisecond deadline |

Each spawned task runs in an isolated VM -- no shared mutable state.
Closures capture values by copy, so captured variables are safe to use concurrently.

```a
; Fetch multiple URLs in parallel
let urls = ["https://api.a.com", "https://api.b.com", "https://api.c.com"]
let responses = parallel_map(urls, fn(url) => http.get(url))

; Spawn individual tasks for fine-grained control
let h1 = spawn(fn() => expensive_computation())
let h2 = spawn(fn() => io.read_file("large.txt"))
let r1 = await(h1)   ; Ok(result) or Err(message)
let r2 = await(h2)

; Run with a timeout
let r = timeout(5000, fn() => slow_network_call())
if is_err(r) { println("timed out!") }
```

### Time

| Function | Signature | Description |
|----------|-----------|-------------|
| `time.now()` | -> i64 | Current Unix epoch milliseconds |
| `time.sleep(ms)` | i64 -> void | Sleep for `ms` milliseconds |

### Hashing

| Function | Signature | Description |
|----------|-----------|-------------|
| `hash.sha256(s)` | str -> str | SHA-256 hash as lowercase hex string |
| `hash.md5(s)` | str -> str | MD5 hash as lowercase hex string |

### HTTP Server

| Function | Signature | Description |
|----------|-----------|-------------|
| `http.serve(port, handler)` | i64, fn -> void | Start HTTP server; handler receives `{method, path, headers, body}`, returns `{status, headers, body}` |
| `http.serve_static(port, dir)` | i64, str -> void | Serve static files from directory |

### HTTP Streaming

| Function | Signature | Description |
|----------|-----------|-------------|
| `http.stream(url, body, headers)` | str, str, map -> handle | Open streaming HTTP connection |
| `http.stream_read(h)` | handle -> str/void | Read next line (void at EOF) |
| `http.stream_close(h)` | handle -> void | Close stream |

### WebSocket

| Function | Signature | Description |
|----------|-----------|-------------|
| `ws.connect(url)` | str -> handle | Connect to ws:// or wss:// server (RFC 6455) |
| `ws.send(h, msg)` | handle, str -> void | Send text message |
| `ws.recv(h)` | handle -> str | Receive next message (blocks) |
| `ws.close(h)` | handle -> void | Close connection |

### Async I/O

Non-blocking HTTP via `poll()`-based event loop, up to 256 concurrent requests.

| Function | Signature | Description |
|----------|-----------|-------------|
| `async.http_get(url, headers?)` | str, map? -> handle | Start async GET |
| `async.http_post(url, body?, headers?)` | str, any?, map? -> handle | Start async POST |
| `async.http_put(url, body?, headers?)` | str, any?, map? -> handle | Start async PUT |
| `async.http_patch(url, body?, headers?)` | str, any?, map? -> handle | Start async PATCH |
| `async.http_delete(url, headers?)` | str, map? -> handle | Start async DELETE |
| `async.await(handle)` | handle -> {status, body, headers} | Wait for single request |
| `async.gather(handles)` | [handle] -> [Result] | Wait for all requests |

```a
let h1 = async.http_get("https://api.a.com/data")
let h2 = async.http_get("https://api.b.com/data")
let results = async.gather([h1, h2])
```

### Database

| Function | Signature | Description |
|----------|-----------|-------------|
| `db.open(path)` | str -> handle | Open SQLite database (or `":memory:"`) |
| `db.exec(db, sql)` | handle, str -> void | Execute SQL statement |
| `db.query(db, sql, params?)` | handle, str, [any]? -> [map] | Query with `?` binding, returns rows |
| `db.close(db)` | handle -> void | Close database |

### Subprocess

| Function | Signature | Description |
|----------|-----------|-------------|
| `proc.spawn(cmd)` | str -> handle | Start subprocess with bidirectional pipes |
| `proc.write(h, data)` | handle, str -> void | Write to subprocess stdin |
| `proc.read_line(h)` | handle -> str | Read line from subprocess stdout |
| `proc.kill(h)` | handle -> void | Kill subprocess |
| `proc.wait(h)` | handle -> i64 | Wait for exit, return exit code |
| `proc.is_running(h)` | handle -> bool | Check if still running |

### Compression

| Function | Signature | Description |
|----------|-----------|-------------|
| `compress.deflate(data)` | str -> str | Raw deflate compression |
| `compress.inflate(data)` | str -> str | Raw deflate decompression |
| `compress.gzip(data)` | str -> str | Gzip compression |
| `compress.gunzip(data)` | str -> str | Gzip decompression |

### UUID

| Function | Signature | Description |
|----------|-----------|-------------|
| `uuid.v4()` | -> str | Cryptographic random UUID v4 via `/dev/urandom` |

### Signals

Register handlers for OS signals (native CLI only).

| Function | Signature | Description |
|----------|-----------|-------------|
| `signal.on(name, handler)` | str, fn -> void | Register handler for `SIGINT`, `SIGTERM`, `SIGHUP`, `SIGUSR1`, `SIGUSR2` |

### Image

Native image processing via bundled stb_image (native CLI only).

| Function | Signature | Description |
|----------|-----------|-------------|
| `image.load(path)` | str -> handle | Load PNG/JPEG/BMP/GIF from file |
| `image.decode(bytes)` | str -> handle | Decode from in-memory bytes |
| `image.encode(img, fmt)` | handle, str -> str | Encode to `"png"`, `"bmp"`, or `"jpeg"` |
| `image.save(img, path)` | handle, str -> void | Save to file (format from extension) |
| `image.width(img)` | handle -> i64 | Image width in pixels |
| `image.height(img)` | handle -> i64 | Image height in pixels |
| `image.resize(img, w, h)` | handle, i64, i64 -> handle | Bilinear resize |
| `image.pixels(img)` | handle -> [[i64]] | RGBA pixel data |

### Reflection

| Function | Signature | Description |
|----------|-----------|-------------|
| `reflect.memory_usage` | -> i64 | RSS in bytes (via mach/procfs) |
| `reflect.uptime_ms` | -> i64 | Process uptime in milliseconds |
| `reflect.pid` | -> i64 | Process ID |

### Stdin

| Function | Signature | Description |
|----------|-----------|-------------|
| `io.read_stdin()` | -> str | Read all of stdin |
| `io.read_line()` | -> str | Read one line from stdin |
| `io.read_bytes(n)` | i64 -> str | Read n bytes from stdin |
| `io.flush()` | -> void | Flush stdout |

---

## 13. Standard Library

Import with `use std.<module>`.

### std.math

```a
use std.math
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `max(a, b)` | i64, i64 -> i64 | Maximum |
| `min(a, b)` | i64, i64 -> i64 | Minimum |
| `clamp(val, lo, hi)` | i64, i64, i64 -> i64 | Clamp to range |
| `fmax(a, b)` | f64, f64 -> f64 | Float maximum |
| `fmin(a, b)` | f64, f64 -> f64 | Float minimum |
| `pow(base, exp)` | i64, i64 -> i64 | Integer exponentiation |
| `sum(arr)` | [i64] -> i64 | Sum of array |
| `range(start, end)` | i64, i64 -> [i64] | Range `[start, end)` |

### std.strings

```a
use std.strings
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `repeat(s, n)` | str, i64 -> str | Repeat string n times |
| `pad_left(s, width, pad)` | str, i64, str -> str | Left-pad to width |
| `pad_right(s, width, pad)` | str, i64, str -> str | Right-pad to width |
| `reverse(s)` | str -> str | Reverse string |
| `count(s, sub)` | str, str -> i64 | Count occurrences |
| `center(s, width, pad)` | str, i64, str -> str | Center-pad to width |
| `is_empty(s)` | str -> bool | True if empty string |

### std.cli

ANSI terminal colors and styles. Functions wrap a string in escape codes and nest safely.

```a
use std.cli
println(bold(red("error: ") + "something failed"))
println(dim(gray("-- done --")))
```

**Colors**: `red(s)`, `green(s)`, `yellow(s)`, `blue(s)`, `magenta(s)`, `cyan(s)`, `gray(s)`

**Styles**: `bold(s)`, `dim(s)`, `underline(s)`

### std.testing

Assertion library for test files. Assertions call `fail()` on mismatch.

```a
use std.testing

fn test_addition() -> bool {
  assert_eq(1 + 1, 2)
}
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `assert_eq(actual, expected)` | any, any -> void | Fail if not equal |
| `assert_int_eq(actual, expected)` | i64, i64 -> void | Fail if integers not equal |
| `assert_true(val)` | bool -> void | Fail if not true |
| `assert_false(val)` | bool -> void | Fail if not false |
| `assert_contains(haystack, needle)` | str, str -> void | Fail if not substring |
| `assert_not_eq(actual, expected)` | any, any -> void | Fail if equal |

### std.re

Pure "a" regex engine (no Rust dependency). For most use cases, the builtin `regex.*` functions are faster. This module exists for bootstrap/self-hosting scenarios.

| Function | Signature | Description |
|----------|-----------|-------------|
| `test(pattern, text)` | str, str -> bool | Full match test |
| `search(pattern, text)` | str, str -> map/false | First match with position |
| `find_all(pattern, text)` | str, str -> [str] | All matches |
| `replace(pattern, text, rep)` | str, str, str -> str | Replace all |
| `replace_first(pattern, text, rep)` | str, str, str -> str | Replace first |
| `split(pattern, text)` | str, str -> [str] | Split on pattern |

### std.path

Path manipulation utilities. Pure "a" module, Unix-oriented.

```a
use std.path
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `join(a, b)` | str, str -> str | Join path segments with `/` |
| `join3(a, b, c)` | str, str, str -> str | Join three segments |
| `dirname(p)` | str -> str | Parent directory |
| `basename(p)` | str -> str | Final path component |
| `extension(p)` | str -> str | File extension (empty if none) |
| `stem(p)` | str -> str | Basename without extension |
| `with_extension(p, ext)` | str, str -> str | Replace file extension |
| `is_absolute(p)` | str -> bool | True if starts with `/` |
| `segments(p)` | str -> [str] | Split into path components |
| `normalize(p)` | str -> str | Collapse `//`, resolve `.` and `..` |

### std.datetime

Date and time utilities. Timestamps are Unix epoch milliseconds.

```a
use std.datetime
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `now()` | -> i64 | Current epoch milliseconds |
| `timestamp()` | -> i64 | Current epoch seconds |
| `sleep(ms)` | i64 -> void | Sleep for milliseconds |
| `to_parts(epoch_ms)` | i64 -> map | Decompose to `year`, `month`, `day`, `hour`, `minute`, `second`, `ms` |
| `from_parts(y, mo, d, h, mi, s)` | i64... -> i64 | Construct epoch ms from components |
| `format(epoch_ms, fmt)` | i64, str -> str | Format with `%Y`, `%m`, `%d`, `%H`, `%M`, `%S`, `%F`, `%T` |
| `iso(epoch_ms)` | i64 -> str | ISO 8601 format (`2026-04-10T15:30:00Z`) |
| `add_ms(epoch_ms, n)` | i64, i64 -> i64 | Add milliseconds |
| `add_seconds(epoch_ms, n)` | i64, i64 -> i64 | Add seconds |
| `add_minutes(epoch_ms, n)` | i64, i64 -> i64 | Add minutes |
| `add_hours(epoch_ms, n)` | i64, i64 -> i64 | Add hours |
| `add_days(epoch_ms, n)` | i64, i64 -> i64 | Add days |
| `diff_ms(a, b)` | i64, i64 -> i64 | Absolute difference in ms |
| `diff_seconds(a, b)` | i64, i64 -> i64 | Absolute difference in seconds |

### std.hash

Hashing utilities. Wraps `hash.sha256` and `hash.md5` builtins with convenience functions.

```a
use std.hash
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `sha256(s)` | str -> str | SHA-256 hex digest |
| `md5(s)` | str -> str | MD5 hex digest |
| `sha256_file(path)` | str -> str | SHA-256 of file contents |
| `md5_file(path)` | str -> str | MD5 of file contents |
| `quick(s)` | str -> str | Short md5 hash (16 hex chars) for cache keys |

### std.encoding

Base64, hex, and URL encoding/decoding. Pure "a" module.

```a
use std.encoding
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `base64_encode(s)` | str -> str | Base64 encode |
| `base64_decode(s)` | str -> str | Base64 decode |
| `hex_encode(s)` | str -> str | Hex encode (each byte -> 2 hex chars) |
| `hex_decode(s)` | str -> str | Hex decode |
| `url_encode(s)` | str -> str | Percent-encode for URLs |
| `url_decode(s)` | str -> str | Percent-decode (handles `+` as space) |

### std.csv

CSV parsing and generation. RFC 4180 compliant. Pure "a" module.

```a
use std.csv
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `parse(text)` | str -> [[str]] | Parse CSV to array of rows |
| `parse_records(text)` | str -> [map] | Parse with first row as headers |
| `stringify(rows)` | [[str]] -> str | Convert rows to CSV text |
| `stringify_records(records, headers)` | [map], [str] -> str | Convert maps to CSV |
| `parse_row(line)` | str -> [str] | Parse a single CSV line |
| `escape_field(field)` | str -> str | Quote/escape a field if needed |

### std.template

Mustache-style string templating. Pure "a" module.

```a
use std.template
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `render(tmpl, vars)` | str, map -> str | Render template with variable map |
| `render_file(path, vars)` | str, map -> str | Read template file and render |

Template syntax:

- `{{name}}` -- variable substitution
- `{{#if key}}...{{/if}}` -- conditional block
- `{{#if key}}...{{#else}}...{{/if}}` -- conditional with else
- `{{#each items}}...{{/each}}` -- iteration over array
- `{{.}}` -- current item in each block
- `{{@index}}` -- current index in each block
- `{{#each items}}...{{#else}}...{{/each}}` -- iteration with empty fallback

### std.yaml

YAML 1.2 subset parser and serializer.

| Function | Signature | Description |
|----------|-----------|-------------|
| `parse(text)` | str -> any | Parse YAML to value |
| `stringify(val)` | any -> str | Serialize value to YAML |

### std.toml

TOML parser and serializer (tables, arrays of tables, all value types).

| Function | Signature | Description |
|----------|-----------|-------------|
| `parse(text)` | str -> map | Parse TOML to map |
| `stringify(val)` | map -> str | Serialize map to TOML |

### std.html

HTML DOM parser with CSS selector queries.

| Function | Signature | Description |
|----------|-----------|-------------|
| `parse(html)` | str -> node | Parse HTML to DOM tree |
| `select(node, selector)` | node, str -> [node] | CSS selector query |
| `text(node)` | node -> str | Extract text content |

### std.url

URL parsing, encoding, and construction.

| Function | Signature | Description |
|----------|-----------|-------------|
| `parse(url_str)` | str -> map | Parse URL to `{scheme, host, port, path, query, fragment}` |
| `encode(s)` | str -> str | Percent-encode |
| `decode(s)` | str -> str | Percent-decode |
| `build(parts)` | map -> str | Construct URL from parts map |

### std.llm

Unified LLM client for OpenAI, Anthropic, and Google AI.

| Function | Signature | Description |
|----------|-----------|-------------|
| `chat(provider, model, messages, opts)` | str, str, [map], map -> map | Send chat request, returns `{content, role, usage, tool_calls}` |
| `stream(provider, model, messages, on_chunk, opts)` | str, str, [map], fn, map -> map | Stream tokens; `on_chunk(text)` called per token |
| `models(provider)` | str -> [str] | List available models for provider |

```a
use std.llm
let reply = llm.chat("openai", "gpt-4", [#{"role": "user", "content": "Hello"}], #{})
println(reply["content"])
```

### std.local_llm

GGUF model load/inference in-process (CPU-only, native builds only).

| Function | Signature | Description |
|----------|-----------|-------------|
| `load(path)` | str -> handle | Load GGUF model (Q4_0, Q8_0, F16, F32) |
| `generate(h, prompt, opts)` | handle, str, map -> str | Generate text |
| `chat(h, messages, opts)` | handle, [map], map -> str | Chat completion |
| `embed(h, text)` | handle, str -> [f64] | Generate embedding vector |
| `tokenize(h, text)` | handle, str -> [i64] | Tokenize text |
| `detokenize(h, tokens)` | handle, [i64] -> str | Detokenize tokens |
| `vocab_size(h)` | handle -> i64 | Vocabulary size |
| `info(h)` | handle -> map | Model metadata |
| `is_loaded(h)` | handle -> bool | Check if handle is valid |
| `unload(h)` | handle -> void | Free model resources |

### std.log

Structured JSON logging to stderr with levels and context.

| Function | Signature | Description |
|----------|-----------|-------------|
| `set_level(level)` | str -> void | Set minimum level: `"debug"`, `"info"`, `"warn"`, `"error"` |
| `debug(msg)` | str -> void | Log at debug level |
| `info(msg)` | str -> void | Log at info level |
| `warn(msg)` | str -> void | Log at warn level |
| `error(msg)` | str -> void | Log at error level |
| `infof(msg, ctx)` | str, map -> void | Log with structured context |
| `timed(label, f)` | str, fn -> void | Log function duration |

### std.args

Declarative CLI argument parser with flags, options, positionals, and auto-help.

| Function | Signature | Description |
|----------|-----------|-------------|
| `spec()` | -> map | Create empty argument spec |
| `name(s, n)` | map, str -> map | Set program name |
| `desc(s, d)` | map, str -> map | Set description |
| `flag(s, long, short, desc)` | map, str, str, str -> map | Add boolean flag |
| `option(s, long, short, desc, default)` | map, str, str, str, str -> map | Add option with default |
| `positional(s, name, desc)` | map, str, str -> map | Add positional argument |
| `parse(s)` | map -> map | Parse `args()` against spec |

```a
use std.args
let s = args.spec()
  |> args.name("mytool")
  |> args.flag("verbose", "v", "Enable verbose output")
  |> args.option("output", "o", "Output file", "out.txt")
  |> args.positional("input", "Input file")
let parsed = args.parse(s)
```

### std.schema

JSON Schema draft-07 validation.

| Function | Signature | Description |
|----------|-----------|-------------|
| `validate(value, schema)` | any, map -> Result | Validate value against schema, Ok or Err with details |
| `from_type(type_str)` | str -> map | Generate schema from type string |

### std.diff

Myers algorithm unified diff with structured ops and patch application.

| Function | Signature | Description |
|----------|-----------|-------------|
| `lines(a, b)` | str, str -> [map] | Structured line-level diff ops |
| `text(a, b)` | str, str -> str | Unified diff text (with `+`/`-` markers) |
| `patch(original, diff_str)` | str, str -> str | Apply a unified diff |

### std.config

Layered configuration loading from TOML, dotenv, and environment variables.

| Function | Signature | Description |
|----------|-----------|-------------|
| `load(path)` | str -> map | Load TOML config file |
| `from_env(prefix)` | str -> map | Load env vars with prefix (e.g. `"APP_"`) |
| `dotenv(path)` | str -> void | Load `.env` file into environment |
| `merge(base, over)` | map, map -> map | Deep-merge two configs (over wins) |
| `require(cfg, keys)` | map, [str] -> void | Fail if any key is missing |

### std.migrate

SQLite migration runner with idempotent execution.

| Function | Signature | Description |
|----------|-----------|-------------|
| `run(handle, dir)` | handle, str -> map | Run pending migrations from directory |
| `status(handle, dir)` | handle, str -> [map] | List migration status |
| `create(dir, name)` | str, str -> str | Create new migration file |

### std.semver

Semantic versioning parse, compare, and constraint matching.

| Function | Signature | Description |
|----------|-----------|-------------|
| `parse(s)` | str -> map | Parse `"1.2.3"` to `{major, minor, patch}` |
| `format(v)` | map -> str | Format version map to string |
| `compare(a, b)` | map, map -> i64 | -1, 0, or 1 |
| `satisfies(version, constraint)` | str, str -> bool | Check `">=1.0.0 <2.0.0"` style constraints |
| `best_match(versions, constraint)` | [str], str -> str | Best matching version |

### std.pkg

Package manager with manifest, dependency resolution, and installation.

| Function | Signature | Description |
|----------|-----------|-------------|
| `init(dir)` | str -> void | Create `pkg.toml` manifest |
| `read_manifest(dir)` | str -> map | Parse `pkg.toml` |
| `add_dep(dir, name, source)` | str, str, str -> void | Add dependency |
| `install(dir)` | str -> map | Install all dependencies |
| `parse_source(source)` | str -> map | Parse source string (git URL, path, etc.) |

### std.cron

Task scheduler with intervals, one-shot timers, and cancellation.

| Function | Signature | Description |
|----------|-----------|-------------|
| `schedule(interval_ms, f)` | i64, fn -> map | Create recurring task |
| `once(delay_ms, f)` | i64, fn -> map | Create one-shot timer |
| `cancel(task)` | map -> map | Cancel a scheduled task |
| `run_loop(tasks)` | [map] -> void | Run scheduler (blocks forever) |
| `run_for(tasks, duration_ms)` | [map], i64 -> void | Run scheduler for duration |

### std.kv

Persistent key-value store backed by SQLite.

| Function | Signature | Description |
|----------|-----------|-------------|
| `open(path)` | str -> handle | Open (or create) KV store |
| `get(h, key)` | handle, str -> Result | Get value (Ok or Err) |
| `set(h, key, value)` | handle, str, any -> void | Set value (JSON-serialized) |
| `delete(h, key)` | handle, str -> void | Remove key |
| `has(h, key)` | handle, str -> bool | Key exists |
| `keys(h)` | handle -> [str] | All keys |
| `list(h, prefix)` | handle, str -> [map] | Keys matching prefix |
| `count(h)` | handle -> i64 | Number of entries |
| `clear(h)` | handle -> void | Remove all entries |
| `close(h)` | handle -> void | Close store |

### std.vector

Vector store with cosine similarity search (SQLite-backed).

| Function | Signature | Description |
|----------|-----------|-------------|
| `open(path, dim)` | str, i64 -> map | Open store with embedding dimension |
| `add(store, id, embedding)` | map, str, [f64] -> void | Add embedding |
| `add_with(store, id, embedding, metadata)` | map, str, [f64], map -> void | Add with metadata |
| `get(store, id)` | map, str -> Result | Retrieve by ID |
| `remove(store, id)` | map, str -> void | Remove entry |
| `search(store, query, k)` | map, [f64], i64 -> [map] | Top-k cosine similarity search |
| `count(store)` | map -> i64 | Number of entries |
| `clear(store)` | map -> void | Remove all entries |
| `close(store)` | map -> void | Close store |

### std.cache

Cache with TTL expiry and LRU eviction (SQLite-backed or in-memory).

| Function | Signature | Description |
|----------|-----------|-------------|
| `open(path)` | str -> map | Open persistent cache |
| `create(max_size)` | i64 -> map | Create in-memory cache with max entries |
| `get(c, key)` | map, str -> Result | Get value (Ok or Err if expired/missing) |
| `set(c, key, value, ttl_ms)` | map, str, any, i64 -> void | Set with TTL (0 = no expiry) |
| `get_or_set(c, key, ttl_ms, f)` | map, str, i64, fn -> any | Get or compute and cache |
| `delete(c, key)` | map, str -> void | Remove entry |
| `has(c, key)` | map, str -> bool | Key exists and not expired |
| `count(c)` | map -> i64 | Number of entries |
| `clear(c)` | map -> void | Remove all entries |
| `close(c)` | map -> void | Close cache |

### std.pool

Generic resource pool with factory, acquire/release, and stats.

| Function | Signature | Description |
|----------|-----------|-------------|
| `create(factory, max_size)` | fn, i64 -> map | Create pool; factory is called to create new resources |
| `acquire(p)` | map -> any | Get resource (reuse or create) |
| `release(p, resource)` | map, any -> map | Return resource to pool |
| `drain(p)` | map -> map | Clear all pooled resources |
| `size(p)` | map -> i64 | Available resources |
| `stats(p)` | map -> map | Pool statistics |

### std.channel

SQLite-backed inter-process message queues with independent cursors.

| Function | Signature | Description |
|----------|-----------|-------------|
| `create(name)` | str -> map | Create new channel |
| `open(name)` | str -> map | Open existing channel |
| `send(ch, msg)` | map, any -> void | Send message (JSON-serialized) |
| `try_recv(ch)` | map -> Result | Non-blocking receive |
| `recv(ch)` | map -> any | Blocking receive |
| `recv_timeout(ch, ms)` | map, i64 -> Result | Receive with timeout |
| `peek(ch)` | map -> Result | Peek without consuming |
| `count(ch)` | map -> i64 | Unread message count |
| `drain(ch)` | map -> [map] | Receive all pending messages |
| `purge(ch)` | map -> void | Clear all messages |
| `close(ch)` | map -> void | Close channel |
| `destroy(name)` | str -> void | Remove channel entirely |

### std.rpc

JSON-RPC 2.0 over HTTP with file-based service discovery.

| Function | Signature | Description |
|----------|-----------|-------------|
| `serve(name, port, handlers)` | str, i64, map -> void | Start RPC server; handlers map method names to functions |
| `register(name, port)` | str, i64 -> void | Register service for discovery |
| `discover(name)` | str -> Result | Find service address |
| `list_agents()` | -> [map] | List all registered services |
| `call(target, method, params)` | str, str, any -> Result | Call method on named service |
| `call_addr(addr, method, params)` | str, str, any -> Result | Call method at specific address |
| `notify(target, method, params)` | str, str, any -> void | Fire-and-forget notification |

### std.mcp

MCP (Model Context Protocol) server and client -- JSON-RPC 2.0 over stdio.

| Function | Signature | Description |
|----------|-----------|-------------|
| `server(name, version)` | str, str -> map | Create MCP server |
| `add_tool(srv, name, desc, schema, handler)` | map, str, str, map, fn -> map | Register tool |
| `add_resource(srv, uri, name, desc, handler)` | map, str, str, str, fn -> map | Register resource |
| `serve(srv)` | map -> void | Start serving over stdio |
| `connect(cmd)` | str -> map | Connect to MCP server process |
| `list_tools(client)` | map -> [map] | List server's tools |
| `call_tool(client, name, args)` | map, str, map -> map | Call a tool |
| `list_resources(client)` | map -> [map] | List server's resources |
| `read_resource(client, uri)` | map, str -> any | Read a resource |
| `close(client)` | map -> void | Disconnect |

### std.agent

Agent OS -- registration, discovery, delegation, persistent state, and self-update.

| Function | Signature | Description |
|----------|-----------|-------------|
| `register(name, capabilities)` | str, [str] -> void | Register agent in local registry |
| `unregister(name)` | str -> void | Remove agent from registry |
| `discover(name)` | str -> Result | Find agent by name |
| `find_by_capability(cap)` | str -> [map] | Find agents with capability |
| `list()` | -> [map] | List all registered agents |
| `delegate(target, task, params)` | str, str, map -> Result | Delegate task to another agent |
| `heartbeat(name)` | str -> void | Update agent's last-seen timestamp |
| `checkpoint(name, state)` | str, map -> void | Save agent state |
| `restore(name)` | str -> Result | Restore saved state |
| `save_plan(name, plan)` | str, map -> void | Persist execution plan |
| `restore_plan(name)` | str -> Result | Restore saved plan |
| `save_trace(name, trace)` | str, map -> void | Persist execution trace |
| `restore_trace(name)` | str -> Result | Restore saved trace |
| `update(source_path)` | str -> void | Self-update (exit code 42 triggers supervisor restart) |
| `version()` | -> str | Agent framework version |

### std.swarm

Multi-agent coordination with pluggable strategies.

| Function | Signature | Description |
|----------|-----------|-------------|
| `create(task, agents, strategy)` | str, [str], str -> Result | Coordinate agents: `"divide"`, `"vote"`, `"race"`, `"chain"` |
| `create_async(task, agents, strategy)` | str, [str], str -> handle | Non-blocking swarm |
| `cancel(agents)` | [str] -> void | Cancel running agents |

Strategies:
- `"divide"` -- split work across agents, merge results
- `"vote"` -- all agents answer, majority wins
- `"race"` -- first agent to finish wins
- `"chain"` -- sequential handoff, each agent's output feeds the next

### std.plan

DAG-based task decomposition with dependency tracking.

| Function | Signature | Description |
|----------|-----------|-------------|
| `create(goal)` | str -> map | Create plan with goal |
| `add_step(p, id, desc, deps)` | map, str, str, [str] -> map | Add step with dependencies |
| `execute(p, step_fn)` | map, fn -> map | Execute all steps (respecting deps) |
| `status(p)` | map -> map | Status summary |
| `summary(p)` | map -> str | Human-readable summary |
| `is_complete(p)` | map -> bool | All steps done |
| `step_ids(p)` | map -> [str] | List step IDs |
| `failed_steps(p)` | map -> [map] | Steps that failed |
| `serialize(p)` | map -> str | Serialize to JSON |
| `deserialize(json_str)` | str -> map | Deserialize from JSON |

### std.trace

Execution tracing with spans, events, and export to Chrome/OpenTelemetry format.

| Function | Signature | Description |
|----------|-----------|-------------|
| `create(name)` | str -> map | Create tracer |
| `begin(t, name)` | map, str -> map | Start a span |
| `end(t, name, data)` | map, str, map -> map | End a span |
| `event(t, name, data)` | map, str, map -> map | Record instant event |
| `counter(t, name, values)` | map, str, map -> map | Record counter values |
| `measure(t, name, f)` | map, str, fn -> map | Measure function duration |
| `spans(t)` | map -> [map] | Extract completed spans |
| `find_events(t, name)` | map, str -> [map] | Find events by name |
| `export_json(t)` | map -> str | Export as JSON |
| `export_chrome(t)` | map -> str | Export as Chrome trace format |
| `export_otel(t)` | map -> str | Export as OpenTelemetry format |
| `clear(t)` | map -> map | Reset tracer |

### std.reflect

Runtime self-inspection with profiling and health checks.

| Function | Signature | Description |
|----------|-----------|-------------|
| `memory_mb()` | -> f64 | RSS in megabytes |
| `memory_kb()` | -> i64 | RSS in kilobytes |
| `info()` | -> map | System info (pid, uptime, memory, platform) |
| `health()` | -> map | Health check (memory, uptime) |
| `profiler()` | -> map | Create code profiler |
| `tick(p, name)` | map, str -> map | Increment call counter |
| `tick_ms(p, name, ms)` | map, str, i64 -> map | Record timed call |
| `hot_paths(p, limit)` | map, i64 -> [map] | Top-N hottest functions |
| `stats(p)` | map -> map | Profiler summary |
| `reset(p)` | map -> map | Clear profiler data |

### std.sandbox

Capability-based sandboxed code execution.

| Function | Signature | Description |
|----------|-----------|-------------|
| `run(source, caps)` | str, map -> map | Execute code with restricted capabilities |
| `validate(source)` | str -> map | Check code safety without running |
| `capabilities(opts)` | map -> map | Create capability set from options |
| `deny_all()` | -> map | No permissions |
| `allow_all()` | -> map | All permissions |
| `allow_read_only(paths)` | [str] -> map | Read-only filesystem access |
| `allow_network(hosts)` | [str] -> map | Network access to specific hosts |

### std.plugin

Plugin management with install, load, and execute.

| Function | Signature | Description |
|----------|-----------|-------------|
| `init(dir, name)` | str, str -> void | Create plugin scaffold |
| `install(source_dir)` | str -> void | Install plugin from directory |
| `install_git(repo)` | str -> void | Install plugin from git repo |
| `remove(name)` | str -> void | Uninstall plugin |
| `list()` | -> [map] | List installed plugins |
| `get(name)` | str -> Result | Get plugin info |
| `is_installed(name)` | str -> bool | Check if installed |
| `load(name)` | str -> str | Load plugin source |
| `run(name)` | str -> void | Execute plugin |

### std.meta

Metaprogramming toolkit -- full AST access for code generation, analysis, and transformation.

| Function | Signature | Description |
|----------|-----------|-------------|
| `parse(source)` | str -> map | Parse "a" source to AST |
| `parse_file(path)` | str -> map | Parse file to AST |
| `emit(prog_ast)` | map -> str | Pretty-print AST to "a" source |
| `walk(node, visitor_fn)` | map, fn -> void | Walk AST, call visitor on each node |
| `find_all(node, pred_fn)` | map, fn -> [map] | Find all nodes matching predicate |
| `find_fns(prog_ast)` | map -> [map] | Find all function declarations |
| `find_calls(node)` | map -> [map] | Find all function calls |
| `fn_names(prog_ast)` | map -> [str] | Extract function names |
| `fn_signatures(prog_ast)` | map -> [map] | Extract function signatures |
| `uses(prog_ast)` | map -> [str] | Extract `use` imports |
| `call_graph(prog_ast)` | map -> map | Build call graph |
| `transform(source, fn)` | str, fn -> str | Transform source via AST |
| `gen_fn(name, params, body)` | str, [str], [map] -> map | Generate function AST node |
| `gen_test(name, body)` | str, [map] -> map | Generate test function |
| `gen_call(fn_name, args)` | str, [map] -> map | Generate function call |

### std.codegen

Compile-time checking, sandboxed execution, and LLM-assisted code generation.

| Function | Signature | Description |
|----------|-----------|-------------|
| `compile_check(source)` | str -> map | Parse and check code for errors |
| `run_in_sandbox(source, opts)` | str, map -> map | Run code in sandbox |
| `test(source, test_cases)` | str, [map] -> map | Run code against test cases |
| `generate(description, context, opts)` | str, str, map -> map | LLM-generate code from description |

### std.refactor

AST-level code refactoring.

| Function | Signature | Description |
|----------|-----------|-------------|
| `rename(source, old, new)` | str, str, str -> str | Rename symbol across source |
| `extract_fn(source, start, end, name)` | str, i64, i64, str -> str | Extract lines into new function |
| `inline_fn(source, fn_name)` | str, str -> str | Inline function at all call sites |

### std.testgen

Automatic test generation from source analysis.

| Function | Signature | Description |
|----------|-----------|-------------|
| `gen_tests(source)` | str -> str | Generate test source from "a" source |
| `gen_tests_for_file(path)` | str -> str | Generate tests for a file |
| `analyze(source)` | str -> map | Analyze source for functions, types, imports |

### std.compiler.*

The self-hosting compiler toolchain (all written in "a"):

| Module | Description |
|--------|-------------|
| `std.compiler.lexer` | Tokenize "a" source into token arrays |
| `std.compiler.parser` | Parse token arrays into tagged-map ASTs |
| `std.compiler.ast` | AST node constructors and accessors |
| `std.compiler.compiler` | Compile ASTs to bytecode |
| `std.compiler.cgen` | Compile ASTs to C source (native compilation) |
| `std.compiler.wasmgen` | Compile ASTs to WebAssembly Text Format (WAT) |
| `std.compiler.emitter` | Pretty-print ASTs back to "a" source |
| `std.compiler.checker` | Static analysis (undefined vars, arity, unused, unreachable) |
| `std.compiler.serialize` | Serialize/deserialize compiled programs |
| `std.compiler.profiler` | Instrument code with hit counters for profiling |
| `std.compiler.optimizer` | Analyze profiles, identify hot paths, suggest optimizations |

---

## 14. Complete Example

```a
use std.testing
use std.math

ty Task = {id: i64, title: str, done: bool}

fn make_task(id: i64, title: str) -> Task {
  ret #{
    "id": id,
    "title": title,
    "done": false
  }
}

fn complete(task: Task) -> Task {
  ret map.set(task, "done", true)
}

fn pending_tasks(tasks: [Task]) -> [Task] {
  ret filter(tasks, fn(t) => !map.get(t, "done"))
}

fn main() -> void effects [io] {
  let tasks = [
    make_task(1, "Write reference"),
    make_task(2, "Add tests"),
    make_task(3, "Ship it")
  ]

  let tasks = [
    complete(tasks[0]),
    tasks[1],
    tasks[2]
  ]

  let remaining = pending_tasks(tasks)
  println("{to_str(len(remaining))} tasks remaining:")

  each(remaining, fn(t) => {
    println("  [{to_str(map.get(t, \"id\"))}] {map.get(t, \"title\")}")
  })
}
```

---

## C Code Generation (Self-Hosting)

The "a" compiler is self-hosting through native compilation. The code generator (`std/compiler/cgen.a`) compiles itself and its entire dependency chain (lexer, parser, AST) to C. gcc builds a freestanding native binary -- no Rust required.

### Workflow

```bash
# One-step compile to native binary
a build program.a -o program

# Or emit C and compile manually
a cc program.a -o program.c
gcc program.c c_runtime/runtime.c -o program -I c_runtime -lm -O2

# Cross-compile to WASM
a build program.a --target wasm32-wasi -o program.wasm
```

### Bootstrap (self-compilation)

```bash
./build.sh                        # gcc bootstrap -> self-host -> done
./a build src/cli.a -o a2         # ./a compiles itself
./a2 build src/cli.a -o a3        # a2 compiles itself too
./a3 run examples/hello.a         # a3 works
```

### How It Works

1. The self-hosted parser (`std.compiler.parser`) produces an AST from "a" source
2. `use` declarations trigger module inlining: referenced modules are loaded, parsed, and emitted with namespace prefixes (e.g. `fn_parser_parse`, `fn_lexer_lex`)
3. Each "a" function becomes a C function returning `AValue` (a tagged union)
4. Variables are pre-declared at function scope to handle "a"'s rebinding semantics
5. Lambdas are lifted to top-level C functions; captured variables become an environment array
6. 181+ builtins map directly to C runtime functions
7. The C runtime library provides the value model, reference counting, and all operations

### Supported Features

Everything in the language compiles natively. The C code generator handles:

- Functions, recursion, closures, lambdas (by-value capture, nested)
- `use` module system with recursive inlining and deduplication
- All types: int, float, bool, void, str, arrays, maps, tuples, Result
- String interpolation, raw strings, heredocs
- Arrays (creation, indexing, iteration, spread `[...a, x, ...b]`)
- Maps (creation, get/set/has, bracket indexing)
- All operators including pipe (`|>`)
- `if`/`else if`/`else`, `while`, `for ... in`, `break`, `continue`, `ret`
- `let` / `let mut` bindings with arbitrary shadowing
- Pattern matching (`match` with literals, constructors, arrays, maps, wildcards, guards)
- `try { ... }` blocks and `?` operator (via `setjmp`/`longjmp`)
- Destructuring: `let [a, b] = expr`, `let [first, ...rest] = arr`, `for [k, v] in pairs`
- Higher-order functions: `map`, `filter`, `reduce`, `each`, `sort_by`, `find`, `any`, `all`, `flat_map`, `min_by`, `max_by`, `enumerate`, `zip`, `take`, `drop`, `unique`, `chunk`
- Concurrency: `spawn`, `await`, `await_all`, `parallel_map`, `parallel_each`, `timeout`
- Async I/O: `async.http_get/post/put/patch/delete`, `async.gather`
- HTTP client (POSIX sockets + platform TLS), HTTP server, HTTP streaming, WebSocket
- Database (bundled SQLite), JSON, filesystem, subprocess, shell
- Compression (deflate/gzip via miniz), image processing (stb_image)
- Local LLM inference (GGUF: Q4_0/Q8_0/F16/F32)
- Signals, environment, UUID, hashing (SHA-256/MD5)
- C FFI via `extern fn` with automatic type marshalling
- `eval` (runtime code evaluation)
- All 58 standard library modules compile natively
- 53 test suites, 890+ native tests passing

### FFI (Foreign Function Interface)

Call C functions directly from "a":

```a
extern fn abs(n: i32) -> i32
extern fn strlen(s: str) -> i64
extern fn getpid() -> i32

fn main() {
  println(to_str(abs(-42)))       ; 42
  println(to_str(strlen("hello"))) ; 5
}
```

Supported FFI types: `i8`, `i16`, `i32`, `i64`, `u8`-`u64`, `f32`, `f64`, `str`, `bool`, `ptr`, `void`. The code generator produces type-marshalling shim wrappers automatically. Link external libraries by passing `-l` flags to gcc.

### Known Limitations

- Tail call optimization (not yet implemented)
- FFI callback trampolines, variadic C functions, extern structs

### Performance

Native executables run **~164x faster** than the bytecode VM for compute-heavy tasks (fib(35): 0.17s native vs 28s VM). The native compiler itself runs **~144x faster** than the VM-hosted code generator.
