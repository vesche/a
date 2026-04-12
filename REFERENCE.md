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
| `a run file.a [args...]` | Compile and run via bytecode VM |
| `a test file.a [-f filter]` | Run all `test_*` functions |
| `a check file.a` | Type-check without running |
| `a fmt file.a` | Format to canonical style |
| `a ast file.a` | Dump AST as JSON |
| `a compile file.a [-o out]` | Compile to `.ac` bytecode file |
| `a interp file.a` | Run via tree-walking interpreter (legacy) |

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
# Generate C from any "a" program (including multi-module)
a run std/compiler/cgen.a -- program.a > program.c

# Compile to native binary
gcc program.c c_runtime/runtime.c -o program -I c_runtime -lm -O2

# Run
./program
```

### Bootstrap (self-compilation)

```bash
# The compiler compiles itself
a run std/compiler/cgen.a -- std/compiler/cgen.a > ac.c
gcc ac.c c_runtime/runtime.c -o ac -I c_runtime -lm -O2

# Native compiler works identically
./ac program.a > program.c

# Fixed point: native compiler compiles itself, output matches
./ac std/compiler/cgen.a > ac2.c
diff ac.c ac2.c    # identical
```

### How It Works

1. The self-hosted parser (`std.compiler.parser`) produces an AST from "a" source
2. `use` declarations trigger module inlining: referenced modules are loaded, parsed, and emitted with namespace prefixes (e.g. `fn_parser_parse`, `fn_lexer_lex`)
3. Each "a" function becomes a C function returning `AValue` (a tagged union)
4. Variables are pre-declared at function scope to handle "a"'s rebinding semantics
5. 45+ builtins map directly to C runtime functions
6. The C runtime library provides the value model, reference counting, and all operations

### Supported Features

- Functions with parameters and recursion
- `use` module system with recursive inlining and deduplication
- Integer, float, boolean, void, string literals
- String interpolation
- Arrays (creation, indexing, iteration)
- Maps (creation, get/set/has, bracket indexing)
- Arithmetic and comparison operators
- `if`/`else if`/`else`, `while`, `for ... in`
- `break`, `continue`, `ret`
- `let` / `let mut` bindings with arbitrary shadowing
- Variable pre-declaration (handles C redefinition rules)
- C keyword escaping for variable names

### Not Yet Supported (Deferred)

- Closures / lambdas / higher-order functions (`map`, `filter`, `reduce`)
- Pattern matching
- `eval`, concurrency, HTTP, filesystem
- Tail call optimization

### Performance

Native executables run **~164x faster** than the bytecode VM for compute-heavy tasks (fib(35): 0.17s native vs 28s VM). The native compiler itself runs **~144x faster** than the VM-hosted code generator.
