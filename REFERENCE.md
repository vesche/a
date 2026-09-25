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
| `a run file.a [-- args...]` | Compile and run (cached -- instant on repeat). The process becomes the program (`execv`): stdin, streaming output, exit code and signals are the program's own. A leading `--` is dropped. |
| `a build file.a [-o out]` | Compile to native binary via C. Incremental: every module is its own translation unit, cached by the hash of its generated C (see Build Cache) |
| `a build file.a --target T` | Cross-compile (WASM, Linux, Windows, macOS) |
| `a build file.a --release` | Compile out the runtime stack-trace machinery (see Runtime Failures) |
| `a cc file.a [-o out]` | Emit generated C to stdout (or file with `-o`) |
| `a wat file.a [-o out]` | Emit WebAssembly Text Format |
| `a targets` | List cross-compile targets and detected toolchains |
| `a test dir/ [--timeout S] [--filter SUBSTR] [--skip a,b] [-v]` | Find `test_*.a` files, compile (cached like `a run`), run, report. Unchanged tests skip compile. Each test runs in its own process group with a deadline (default 60s); on timeout or exit the whole group is killed. Sets a private `A_HOME` unless one is already set. A file with no `fn main` but with nullary `fn test_*()` gets a synthesized main that calls them. |
| `a check PATH...` | Static analysis over files and directories (recursive): undefined names, unknown builtins, arity, type mismatches (gradual, see §3), record fields, non-exhaustive matches, misuse of `?`, missing/`use` parse errors (E0013/E0014), unused variables, unreachable code, builtin shadowing. Exit 1 on errors. |
| `a check --json PATH...` | Same diagnostics, one JSON object per line: `{"file","line","col","code","severity","message"}` |
| `a explain [CODE]` | Explain a diagnostic code (`a explain E0007`), or list all codes |
| `a check --effects PATH...` | Print the inferred effect set of every function and of each program |
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

### Build Cache

`a build`, `a run` and `a test` compile each `use`d module as its own C
translation unit and cache the object file under `/tmp/a_obj/units/` keyed by
a hash of the unit's generated C plus everything else that shapes the object
(runtime sources, `-D` defines, flags). A module whose generated C did not
change is linked from the cache, never recompiled -- sixty test programs that
import the compiler compile it once, and editing one module recompiles that
module alone. (Line numbers are part of the generated C, so inserting a line
above code does change it; a trailing comment does not.)
The runtime objects are cached the same way under `/tmp/a_obj/<hash>/`, keyed
by every runtime source including `runtime.h`. `a run` additionally caches
the linked binary in `.a_cache/`, keyed by the sources, the runtime and the
identity (size and mtime) of the `a` binary itself, so rebuilding the
compiler invalidates it. The object cache is pruned to 300 entries once it
passes 600.

### Exit Status

`fn main() -> int` (or any sized integer type) makes the returned integer the
process exit status. An `Err` escaping `main` prints `error: <payload>` plus
the stack trace and exits 1. Any other `main` exits 0; a tail expression that
merely happens to be an integer does not become an exit status.

### Diagnostics

Every message about a program has the form `file:line:col: severity: message`
(gcc style, so editors and agents can parse it). Parse errors, checker
diagnostics, and compile errors all use it. Checker diagnostics also carry a
stable code -- `file.a:3:7: error[E0007]: argument 1 of greet: expected str, got int`
-- so a tool can branch on the code instead of the English. `a explain E0007`
prints what the code means, why it fires and an example; `a explain` lists
them all. The catalog is `std/compiler/diag_codes.a`:

| Code | Meaning |
|------|---------|
| `E0001` | undefined variable |
| `E0002` | undefined function (this file or a `use`d module) |
| `E0003` | unknown builtin in a builtin namespace (`str.length`) |
| `E0004` | wrong number of arguments |
| `E0005` | non-exhaustive `match` on a sum type |
| `E0006` | `?` in a function that does not return `Result` |
| `E0007` | type mismatch between two known types (let, argument, return, assignment, map key, record shape) |
| `E0008` | `ret value` in a `-> void` function |
| `E0009` | unknown field on a named record |
| `E0010` | operator on incompatible types (`"a" + 1`, `xs < 3`, `5[0]`, `-"x"`) |
| `E0011` | a function does something its `effects [...]` declaration does not allow |
| `E0012` | unknown effect name in an `effects [...]` clause |
| `E0013` | `use` path does not resolve to a file |
| `E0014` | `use`d file exists but does not parse |
| `W0001` | unused variable (prefix with `_` to silence) |
| `W0002` | unreachable code after `ret` |
| `W0003` | bare call resolves to a builtin, not to the local function of that name |

The language server (`a-lsp`) publishes the same diagnostics, with the code in
the LSP `code` field.

### Runtime Failures

A failure at runtime -- `fail(msg)`, `unwrap` on an `Err`, `expect`, integer
division by zero, an uncaught `?` -- prints the message and an `a`-level stack
trace, innermost frame first, then exits with status 1:

```
runtime error: not a number: x
  at parse_num (calc.a:2)
  at total (calc.a:8)
  at <lambda in main> (calc.a:14)
  at main (calc.a:15)
```

Each line is the function and the source line it was executing (for outer
frames, the line of the call). Lambdas appear as `<lambda in f>`. Generated
code keeps a shadow call stack for this (one store per statement, not
measurable on call-heavy code); `a build --release` (or `A_RELEASE=1`) compiles
it out, and failures then print only the message. Assertion failures under
`a test` show the same trace, so a failing test points at both the assertion
and the line in the test that called it.

Inside a `try { ... }` block the same failures do not exit: the block evaluates
to `Err(msg)` instead (see Error Handling). `a build`, `a run`, and `a test`
run the static checker before generating C, so an undefined name, a wrong
arity, or a non-exhaustive match stops the build with a diagnostic at the `a`
source line (warnings are only shown by `a check`). `A_NO_CHECK=1` bypasses
the checker if it ever rejects a valid program. If the generated C ever fails to
compile, that is a compiler bug: `a` reports
`file.a:L:C: internal compiler error: generated C did not compile`, shows the
first C compiler line, and keeps the `.c` file for the report. A missing
`extern fn` symbol is reported as `link error: undefined symbol NAME`.

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

Programs must define `fn main()` as the entry point. A file with no `main` but with one or more nullary `fn test_*()` functions gets a synthesized `main` that calls them in source order -- so a test file can omit `main`.

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
| `int`, `float` | Canonical names; every sized integer is `int` and every float is `float` to the checker |
| `num` | `int` or `float` (used in builtin signatures such as `math.max`) |
| `any` | Anything; opts a value out of static checking |

### Gradual typing

Annotations are optional and checked statically by `a check` (and before every
build). The rules, which are what `std/compiler/types.a` implements:

- An unannotated value has whatever type the checker can infer from its
  initialiser (`let n = 1` is `int`, `let xs = [1, 2]` is `[int]`, a literal
  `#{"x": 1}` is the record shape `{x: int}`). A mutable binding that is
  reassigned a different type simply widens to the common type (`any` when
  there is none). Inference never produces an error on its own.
- An error is reported only when two *known* types are incompatible: an
  initialiser against `let x: T`, an argument against a parameter, a `ret`
  value against the declared return type, an assignment against an annotated
  `let mut`, or a map key against the key type.
- `any` is compatible with everything in both directions, `int` widens to
  `float`, `num` accepts either, all sized integers are `int` (`i64`, `u8`,
  ...), `map` means `#{any: any}` and `array` means `[any]`. A `Result` is
  compatible with everything because unwrapping is dynamic.
- Builtins are typed by the signature table in `std/compiler/builtin_sigs.a`
  (`push: fn([T], T) -> [T]`, `reduce: fn([T], U, fn(U, T) -> U) -> U`, ...).
  Type variables are single uppercase letters bound per call, so
  `push([1, 2], "x")` is `E0007: argument 2 of push: expected int, got str`,
  and `reduce(xs, f, 0)` (arguments swapped) is caught before it silently
  prints `void`.
- Records (`ty P = {x: int}`) are maps with known fields; see Type Declarations.
- Lambdas are typed from their parameter annotations and an expression body;
  a block-bodied lambda returns `any`.

Passing `A_NO_CHECK=1` skips the checker for one build if it ever rejects a
valid program -- and that is a bug worth reporting.

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
ty Point = {x: int, y: float}

fn shift(p: Point, dx: int) -> Point { ret map.set(p, "x", p.x + dx) }

let p: Point = #{"x": 1, "y": 2.5}
println(shift(p, 4).x)     ; 5
```

A record is a map with known fields. It is constructed as a map literal and is
a plain map at runtime (`map.keys`, `map.set`, structural `==` all work), but
wherever a record type is expected -- `let p: Point = ...`, an argument, a
`ret` -- the checker verifies the literal's shape: every declared field present
with a compatible type, no undeclared field
(`E0007: let p: Point: expected Point, got {x: int} (missing field y)`).
Field access `p.x` / `p["x"]` on a named record has the declared field type and
an unknown field is `E0009`. Two different named records are never
interchangeable (`Size` is not `Point`), while a bare `map` converts to and from
any record. Field types may be other records; access chains (`l.to.x`) are typed
all the way down.

**Sum types** (tagged unions):

```a
ty Shape = Circle(f64) | Rect(f64, f64) | Empty

fn area(s: Shape) -> f64 {
  match s {
    Circle(r) => { ret 3.14159 * r * r }
    Rect(w, h) => { ret w * h }
    Empty => { ret 0.0 }
  }
}
```

Variants are constructed by name (`Circle(2.0)`, `Empty`) and taken apart with
`match`. A constructor call with the wrong number of arguments is a compile
error. A `match` on a sum type must be exhaustive: every variant needs an arm
or there must be a catch-all (`_` or a plain identifier). Guarded arms
(`Circle(r) if r > 1.0 => ...`) do not count towards coverage. The diagnostic is
`non-exhaustive match on Shape: missing Empty`.

Variants print as `Circle(2)` / `Empty`, compare structurally
(`Circle(1.0) == Circle(1.0)`), and can be nested recursively
(`ty Tree = Leaf(i64) | Node(Tree, Tree)`). `type_of` reports `map` for a
variant in this release; the representation is `#{"$tag": ..., "$args": [...]}`.

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

A function may declare the effects it performs; the checker verifies the
declaration against what the function actually does:

```a
fn save(data: str) -> void effects [fs_write, io] {
  io.write_file("out.txt", data)
  println("saved")
}

fn area(r: float) -> float effects [pure] { ret 3.14159 * r * r }
```

Effects are inferred, not trusted. Every builtin carries its effect in the
signature table (`std/compiler/builtin_sigs.a`): `println` is `io`,
`io.read_file` is `fs_read`, `exec` is `exec`, and so on. A function's
inferred set is the union over everything it calls -- builtins, other
functions in the program (through every `use`d module), and lambdas it
creates -- plus `ffi` for an `extern fn` and `call` for a closure it did not
define (a parameter, a value from elsewhere), whose effects cannot be known.
A declaration that is smaller than the inferred set is `E0011`, with the
origin named: `save is declared effects [io] but has effect fs_write
(io.write_file)`, or `(via helper)` for an effect inherited from a callee.
`effects [pure]` declares the empty set, so a pure function may not call a
closure parameter either (`call`). Functions without a declaration are
unconstrained but still have an inferred set.

| Effect | Meaning |
|--------|---------|
| `io` | stdout, stderr, stdin |
| `fs_read` | reads files or directories |
| `fs_write` | creates, modifies or deletes files |
| `net` | HTTP / WebSocket client or server |
| `exec` | runs another program or a shell |
| `db` | sqlite |
| `env` | environment variables, process introspection |
| `spawn` | tasks, `parallel_*`, signal handlers |
| `time` | clock, sleep |
| `rand` | nondeterministic output (`uuid.v4`) |
| `ffi` | raw pointers, `extern fn` |
| `call` | calls a closure whose effects are not known |

`a check --effects file.a` prints the inferred set of every function and of
the program as a whole (everything reachable from `main`). The analysis
over-approximates: it never misses an effect, but it may attribute one to a
function that only creates a closure with that effect. That is what makes a
declaration a guarantee, and what `std.sandbox` decides on.

### Contracts

Pre/postconditions on functions:

```a
fn divide(a: f64, b: f64) -> f64
  pre { b != 0.0 }
  post { ret >= 0.0 || ret < 0.0 }
{
  ret a / b
}
```

Inside `post`, `ret` names the returned value. Contracts are runtime checks
in a normal build: a false `pre` prints `precondition failed: <fn>` (with a
stack trace) and exits; a false `post` prints `postcondition failed: <fn>`.
`a build --release` / `A_RELEASE=1` compiles them out.

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

`expr?` unwraps an `Ok` and, on `Err`, returns that `Err` from the enclosing
function. It is only allowed in a function that returns `Result` (or has no
declared return type); `?` in a function declared `-> i64` is a compile error.
Inside `try { ... }` the `Err` is caught by the block instead. An `Err` that
propagates out of `main` prints `error: <payload>` plus the stack trace and
exits with status 1.

### Equality

`==` and `!=` are structural for every value: numbers compare by value
(`1 == 1.0`), strings by contents, arrays element-wise, maps key-wise ignoring
insertion order, Results by tag and payload, and sum type variants by tag and
arguments. Closures and pointers compare by identity.

### Field Access, Indexing, Calls

```a
point.x           ; field access
arr[0]            ; indexing
println("hello")  ; function call
```

Call resolution for a bare name `f(...)`: if `f` is a builtin, it is the
builtin -- even if the current module defines a function `f` or a variable `f`
is in scope. A module function that shares a builtin's name is reachable only
as `module.f(...)` from another module; `a check` warns about such
definitions. Non-builtin names resolve to a function in the current module or
program, then to a closure held in a variable.

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
println("model: {resp["model"]}")   ; string literals may appear inside {}
```

A literal `{` or `}` in a string is written `\{` / `\}`.

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

fn main() { println(math_utils.square(4) + square(5)) }
```

An inline `mod` behaves like a `use`d module in the same file: its functions
are reachable qualified (`math_utils.square`) and unqualified (`square`), and
a program's `main` may live inside a `mod`. A program with no `main` anywhere
is rejected with `file.a: error: no fn main found`.

### Importing

```a
use std.testing
use std.math
use std.cli
```

After import, a module's functions are reachable two ways: qualified by the
module's last path segment, or unqualified (every `use` imports all of the
module's functions into the program's namespace):

```a
use std.math
let a = math.clamp(15, 1, 10)   ; qualified
let b = clamp(15, 1, 10)        ; unqualified -- same function
```

`std.math.clamp(...)` is not valid; the compiler reports an error pointing at
the call. Unqualified names lose to builtins: if a module defines `push`, only
`module.push(...)` reaches it.

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
; propagate errors: on Err, the enclosing function returns that Err
fn load() -> Result<str, str> {
  let data = io.read_file("config.txt")?
  ret Ok(str.trim(data))
}

; catch errors
let result = try io.read_file("missing.txt")
if is_err(result) {
  println("file not found")
}

; catch a whole block: the first ? that hits an Err ends the block with it
let parsed = try {
  let a = parse_num(x)?
  let b = parse_num(y)?
  a + b
}
```

`?` is a compile error in a function declared with a non-Result return type
(`-> i64`, `-> str`, ...). A function with no declared return type may use it
and then returns either its normal value or an `Err`. An `Err` escaping `main`
is printed as `error: <payload>` plus the stack trace, with exit status 1.

### Catching Runtime Failures

`try { ... }` also catches runtime failures that would otherwise end the
program: `fail(msg)`, `unwrap` on an `Err`, `expect`, and integer division by
zero. The block evaluates to `Err(msg)` (for `fail`, the message; for
`unwrap`, the `Err` payload; `Err("division by zero")`):

```a
let r = try { parse_config(text) }   ; parse_config may call fail(...)
match r {
  Ok(cfg) => { run(cfg) }
  Err(msg) => { println("bad config: " + msg) }
}
```

A failure anywhere below the block -- in a called function, in a lambda passed
to `map` -- is caught by the nearest enclosing `try`. Values allocated between
the `try` and the failure are not released (the unwind is a `longjmp`), so do
not use `try` as a control-flow construct in a hot loop; it is for turning a
failure into a value. Without an enclosing `try`, a runtime failure prints a
stack trace and exits 1 (see Runtime Failures under Diagnostics).

---

## 12. Builtin Functions Reference

Generated by `a doc` from `std/compiler/builtin_sigs.a`. Do not edit by hand.
A builtin with no effect list is pure. `effects [pure]` on your function rejects every name in the Effects column.

### core

| Function | Signature | Effects |
|----------|-----------|----------|
| `Err(...)` | `fn(E) -> Result<any, E>` | pure |
| `Ok(...)` | `fn(T) -> Result<T, any>` | pure |
| `all(...)` | `fn([T], fn(T) -> bool) -> bool` | pure |
| `any(...)` | `fn([T], fn(T) -> bool) -> bool` | pure |
| `args(...)` | `fn() -> [str]` | pure |
| `argv0(...)` | `fn() -> str` | pure |
| `await(...)` | `fn(any) -> Result<any, any>` | spawn |
| `await_all(...)` | `fn([any]) -> [Result<any, any>]` | spawn |
| `char_code(...)` | `fn(str) -> int` | pure |
| `chunk(...)` | `fn([T], int) -> [[T]]` | pure |
| `concat_arr(...)` | `fn([T], [T]) -> [T]` | pure |
| `contains(...)` | `fn([T], T) -> bool` | pure |
| `drop(...)` | `fn([T], int) -> [T]` | pure |
| `each(...)` | `fn([T], fn(T) -> any) -> void` | pure |
| `embedded_file(...)` | `fn(str) -> str` | pure |
| `enumerate(...)` | `fn([T]) -> [[any]]` | pure |
| `eprintln(...)` | `fn(any) -> void` | io |
| `exec(...)` | `fn(str) -> map` | exec |
| `exec_timeout(...)` | `fn(str, int) -> map` | exec |
| `exit(...)` | `fn(int) -> void` | io |
| `expect(...)` | `fn(Result<T, any>, str) -> T` | pure |
| `fail(...)` | `fn(any) -> void` | pure |
| `filter(...)` | `fn([T], fn(T) -> bool) -> [T]` | pure |
| `find(...)` | `fn([T], fn(T) -> bool) -> Result<T, str>` | pure |
| `flat_map(...)` | `fn([T], fn(T) -> [U]) -> [U]` | pure |
| `float(...)` | `fn(any) -> float` | pure |
| `from_code(...)` | `fn(int) -> str` | pure |
| `int(...)` | `fn(any) -> int` | pure |
| `is_alnum(...)` | `fn(str) -> bool` | pure |
| `is_alpha(...)` | `fn(str) -> bool` | pure |
| `is_digit(...)` | `fn(str) -> bool` | pure |
| `is_err(...)` | `fn(any) -> bool` | pure |
| `is_ok(...)` | `fn(any) -> bool` | pure |
| `len(...)` | `fn(any) -> int` | pure |
| `map(...)` | `fn([T], fn(T) -> U) -> [U]` | pure |
| `max_by(...)` | `fn([T], fn(T) -> any) -> Result<T, str>` | pure |
| `min_by(...)` | `fn([T], fn(T) -> any) -> Result<T, str>` | pure |
| `parallel_each(...)` | `fn([T], fn(T) -> any) -> void` | spawn |
| `parallel_map(...)` | `fn([T], fn(T) -> U) -> [U]` | spawn |
| `print(...)` | `fn(any) -> void` | io |
| `println(...)` | `fn(any) -> void` | io |
| `push(...)` | `fn([T], T) -> [T]` | pure |
| `reduce(...)` | `fn([T], U, fn(U, T) -> U) -> U` | pure |
| `reverse_arr(...)` | `fn([T]) -> [T]` | pure |
| `slice(...)` | `fn([T], int, int) -> [T]` | pure |
| `sort(...)` | `fn([T]) -> [T]` | pure |
| `sort_by(...)` | `fn([T], fn(T, T) -> int) -> [T]` | pure |
| `spawn(...)` | `fn(fn() -> T) -> any` | spawn |
| `take(...)` | `fn([T], int) -> [T]` | pure |
| `timeout(...)` | `fn(int, fn() -> T) -> Result<T, str>` | spawn |
| `to_str(...)` | `fn(any) -> str` | pure |
| `type_of(...)` | `fn(any) -> str` | pure |
| `unique(...)` | `fn([T]) -> [T]` | pure |
| `unwrap(...)` | `fn(Result<T, any>) -> T` | pure |
| `unwrap_or(...)` | `fn(Result<T, any>, T) -> T` | pure |
| `zip(...)` | `fn([T], [U]) -> [[any]]` | pure |

### async

| Function | Signature | Effects |
|----------|-----------|----------|
| `async.await(...)` | `fn(any) -> map` | spawn |
| `async.gather(...)` | `fn([any]) -> [Result<any, any>]` | spawn |
| `async.http_delete(...)` | `fn(str, map) -> any` | net |
| `async.http_get(...)` | `fn(str, map) -> any` | net |
| `async.http_patch(...)` | `fn(str, any, map) -> any` | net |
| `async.http_post(...)` | `fn(str, any, map) -> any` | net |
| `async.http_put(...)` | `fn(str, any, map) -> any` | net |

### compress

| Function | Signature | Effects |
|----------|-----------|----------|
| `compress.deflate(...)` | `fn(str) -> str` | pure |
| `compress.gunzip(...)` | `fn(str) -> str` | pure |
| `compress.gzip(...)` | `fn(str) -> str` | pure |
| `compress.inflate(...)` | `fn(str) -> str` | pure |

### db

| Function | Signature | Effects |
|----------|-----------|----------|
| `db.close(...)` | `fn(any) -> void` | db |
| `db.exec(...)` | `fn(any, str) -> void` | db |
| `db.open(...)` | `fn(str) -> any` | db |
| `db.query(...)` | `fn(any, str, [any]) -> [map]` | db |

### env

| Function | Signature | Effects |
|----------|-----------|----------|
| `env.all(...)` | `fn() -> map` | env |
| `env.get(...)` | `fn(str) -> any` | env |
| `env.set(...)` | `fn(str, str) -> void` | env |

### fs

| Function | Signature | Effects |
|----------|-----------|----------|
| `fs.abs(...)` | `fn(str) -> str` | fs_read |
| `fs.cp(...)` | `fn(str, str) -> void` | fs_write |
| `fs.cwd(...)` | `fn() -> str` | fs_read |
| `fs.exists(...)` | `fn(str) -> bool` | fs_read |
| `fs.is_dir(...)` | `fn(str) -> bool` | fs_read |
| `fs.is_file(...)` | `fn(str) -> bool` | fs_read |
| `fs.ls(...)` | `fn(str) -> [map]` | fs_read |
| `fs.mkdir(...)` | `fn(str) -> void` | fs_write |
| `fs.mv(...)` | `fn(str, str) -> void` | fs_write |
| `fs.rm(...)` | `fn(str) -> void` | fs_write |
| `fs.stat(...)` | `fn(str) -> map` | fs_read |
| `fs.watch(...)` | `fn(str, fn(map) -> any) -> void` | fs_read |

### hash

| Function | Signature | Effects |
|----------|-----------|----------|
| `hash.md5(...)` | `fn(str) -> str` | pure |
| `hash.sha256(...)` | `fn(str) -> str` | pure |

### http

| Function | Signature | Effects |
|----------|-----------|----------|
| `http.delete(...)` | `fn(str, map) -> map` | net |
| `http.get(...)` | `fn(str, map) -> map` | net |
| `http.patch(...)` | `fn(str, any, map) -> map` | net |
| `http.post(...)` | `fn(str, any, map) -> map` | net |
| `http.put(...)` | `fn(str, any, map) -> map` | net |
| `http.serve(...)` | `fn(int, fn(map) -> any) -> void` | net |
| `http.serve_static(...)` | `fn(int, str) -> void` | net |
| `http.stream(...)` | `fn(str, str, map) -> any` | net |
| `http.stream_close(...)` | `fn(any) -> void` | net |
| `http.stream_read(...)` | `fn(any) -> any` | net |

### image

| Function | Signature | Effects |
|----------|-----------|----------|
| `image.decode(...)` | `fn(str) -> any` | pure |
| `image.encode(...)` | `fn(any, str) -> str` | pure |
| `image.height(...)` | `fn(any) -> int` | pure |
| `image.load(...)` | `fn(str) -> any` | fs_read |
| `image.pixels(...)` | `fn(any) -> [int]` | pure |
| `image.resize(...)` | `fn(any, int, int) -> any` | pure |
| `image.save(...)` | `fn(any, str) -> void` | fs_write |
| `image.width(...)` | `fn(any) -> int` | pure |

### io

| Function | Signature | Effects |
|----------|-----------|----------|
| `io.flush(...)` | `fn() -> void` | io |
| `io.read_bytes(...)` | `fn(int) -> str` | fs_read |
| `io.read_file(...)` | `fn(str) -> str` | fs_read |
| `io.read_line(...)` | `fn() -> str` | io |
| `io.read_stdin(...)` | `fn() -> str` | io |
| `io.write_file(...)` | `fn(str, str) -> void` | fs_write |

### json

| Function | Signature | Effects |
|----------|-----------|----------|
| `json.parse(...)` | `fn(str) -> any` | pure |
| `json.pretty(...)` | `fn(any) -> str` | pure |
| `json.stringify(...)` | `fn(any) -> str` | pure |

### local_llm

| Function | Signature | Effects |
|----------|-----------|----------|
| `local_llm.detokenize(...)` | `fn(any, [int]) -> str` | pure |
| `local_llm.embed(...)` | `fn(any, str) -> [float]` | pure |
| `local_llm.generate(...)` | `fn(any, str, map) -> str` | pure |
| `local_llm.info(...)` | `fn(any) -> map` | pure |
| `local_llm.load(...)` | `fn(str) -> any` | fs_read |
| `local_llm.tokenize(...)` | `fn(any, str) -> [int]` | pure |
| `local_llm.unload(...)` | `fn(any) -> void` | pure |
| `local_llm.vocab_size(...)` | `fn(any) -> int` | pure |

### map

| Function | Signature | Effects |
|----------|-----------|----------|
| `map.delete(...)` | `fn(#{K: V}, K) -> #{K: V}` | pure |
| `map.entries(...)` | `fn(#{K: V}) -> [[any]]` | pure |
| `map.from_entries(...)` | `fn([[any]]) -> map` | pure |
| `map.get(...)` | `fn(#{K: V}, K) -> V` | pure |
| `map.has(...)` | `fn(#{K: V}, K) -> bool` | pure |
| `map.keys(...)` | `fn(#{K: V}) -> [K]` | pure |
| `map.merge(...)` | `fn(map, map) -> map` | pure |
| `map.set(...)` | `fn(#{K: V}, K, V) -> #{K: V}` | pure |
| `map.values(...)` | `fn(#{K: V}) -> [V]` | pure |

### math

| Function | Signature | Effects |
|----------|-----------|----------|
| `math.abs(...)` | `fn(num) -> num` | pure |
| `math.ceil(...)` | `fn(num) -> int` | pure |
| `math.floor(...)` | `fn(num) -> int` | pure |
| `math.max(...)` | `fn(num, num) -> num` | pure |
| `math.min(...)` | `fn(num, num) -> num` | pure |
| `math.pow(...)` | `fn(num, num) -> num` | pure |
| `math.round(...)` | `fn(num) -> int` | pure |
| `math.sqrt(...)` | `fn(num) -> float` | pure |

### proc

| Function | Signature | Effects |
|----------|-----------|----------|
| `proc.exec(...)` | `fn(str, [str]) -> Result` | exec |
| `proc.is_running(...)` | `fn(any) -> bool` | exec |
| `proc.kill(...)` | `fn(any) -> void` | exec |
| `proc.read_line(...)` | `fn(any) -> str` | exec |
| `proc.spawn(...)` | `fn(str) -> any` | exec |
| `proc.wait(...)` | `fn(any) -> int` | exec |
| `proc.write(...)` | `fn(any, str) -> void` | exec |

### profile

| Function | Signature | Effects |
|----------|-----------|----------|
| `profile.dump(...)` | `fn(str) -> void` | fs_write |
| `profile.get_counters(...)` | `fn() -> map` | pure |
| `profile.reset(...)` | `fn() -> void` | pure |

### ptr

| Function | Signature | Effects |
|----------|-----------|----------|
| `ptr.is_null(...)` | `fn(ptr) -> bool` | ffi |
| `ptr.null(...)` | `fn() -> ptr` | ffi |

### reflect

| Function | Signature | Effects |
|----------|-----------|----------|
| `reflect.memory_usage(...)` | `fn() -> int` | env |
| `reflect.pid(...)` | `fn() -> int` | env |
| `reflect.uptime_ms(...)` | `fn() -> int` | time |

### signal

| Function | Signature | Effects |
|----------|-----------|----------|
| `signal.on(...)` | `fn(str, fn() -> any) -> void` | spawn |

### str

| Function | Signature | Effects |
|----------|-----------|----------|
| `str.chars(...)` | `fn(str) -> [str]` | pure |
| `str.concat(...)` | `fn(str, str) -> str` | pure |
| `str.contains(...)` | `fn(str, str) -> bool` | pure |
| `str.count(...)` | `fn(str, str) -> int` | pure |
| `str.ends_with(...)` | `fn(str, str) -> bool` | pure |
| `str.find(...)` | `fn(str, str) -> int` | pure |
| `str.join(...)` | `fn([str], str) -> str` | pure |
| `str.lines(...)` | `fn(str) -> [str]` | pure |
| `str.lower(...)` | `fn(str) -> str` | pure |
| `str.replace(...)` | `fn(str, str, str) -> str` | pure |
| `str.slice(...)` | `fn(str, int, int) -> str` | pure |
| `str.split(...)` | `fn(str, str) -> [str]` | pure |
| `str.starts_with(...)` | `fn(str, str) -> bool` | pure |
| `str.trim(...)` | `fn(str) -> str` | pure |
| `str.upper(...)` | `fn(str) -> str` | pure |

### time

| Function | Signature | Effects |
|----------|-----------|----------|
| `time.now(...)` | `fn() -> int` | time |
| `time.sleep(...)` | `fn(int) -> void` | time |

### uuid

| Function | Signature | Effects |
|----------|-----------|----------|
| `uuid.v4(...)` | `fn() -> str` | rand |

### ws

| Function | Signature | Effects |
|----------|-----------|----------|
| `ws.close(...)` | `fn(any) -> void` | net |
| `ws.connect(...)` | `fn(str) -> any` | net |
| `ws.recv(...)` | `fn(any) -> str` | net |
| `ws.send(...)` | `fn(any, str) -> void` | net |

---

## 13. Standard Library

Generated by `a doc` from `std/*.a` and `std/compiler/*.a`. Do not edit by hand.
Import with `use std.<module>`. Names starting with `_` are omitted.

### std.agent

std/agent.a -- Agent OS: registry, delegation, checkpoint, self-update Also includes operational primitives: retry, batch, pipeline, timeout, rate_limit.

| Function | Signature |
|----------|-----------|
| `retry` | `fn(max_attempts: int, delay_ms: int, f)` |
| `batch` | `fn(items, size: int, f)` |
| `pipeline` | `fn(steps, input)` |
| `timeout` | `fn(ms: int, f)` |
| `rate_limit` | `fn(min_interval_ms: int, last_call_ms: int, f)` |
| `register` | `fn(name, capabilities)` |
| `unregister` | `fn(name)` |
| `discover` | `fn(name)` |
| `find_by_capability` | `fn(cap)` |
| `list` | `fn() -> [map]` |
| `heartbeat` | `fn(name)` |
| `delegate` | `fn(target, task, params)` |
| `checkpoint` | `fn(name, state)` |
| `restore` | `fn(name)` |
| `save_plan` | `fn(name, p)` |
| `restore_plan` | `fn(name)` |
| `save_trace` | `fn(name, t)` |
| `restore_trace` | `fn(name)` |
| `update` | `fn(source_path)` |
| `update_from_git` | `fn(source_dir)` |
| `version` | `fn() -> str` |

### std.args

std/args.a -- Declarative CLI argument parsing

| Function | Signature |
|----------|-----------|
| `spec` | `fn() -> map` |
| `name` | `fn(s: map, n: str) -> map` |
| `desc` | `fn(s: map, d: str) -> map` |
| `flag` | `fn(s: map, long: str, short: str, description: str) -> map` |
| `option` | `fn(s: map, long: str, short: str, description: str, default: str) -> map` |
| `positional` | `fn(s: map, pname: str, description: str) -> map` |
| `parse` | `fn(s: map) -> map` |

### std.cache

std/cache.a -- Caching layer with TTL and LRU eviction

| Function | Signature |
|----------|-----------|
| `open` | `fn(path) -> map` |
| `close` | `fn(c)` |
| `get` | `fn(c, key)` |
| `set` | `fn(c, key, value, ttl_ms)` |
| `delete` | `fn(c, key)` |
| `has` | `fn(c, key) -> bool` |
| `evict_expired` | `fn(c)` |
| `evict_lru` | `fn(c, max_size)` |
| `count` | `fn(c) -> int` |
| `clear` | `fn(c)` |
| `get_or_set` | `fn(c, key, ttl_ms, f)` |
| `create` | `fn(max_size) -> map` |

### std.channel

std/channel.a -- Inter-process message channels backed by SQLite Enables decoupled producer/consumer communication between agents. Multiple writers, multiple readers (each with independent cursor).

| Function | Signature |
|----------|-----------|
| `create` | `fn(name) -> map` |
| `open` | `fn(name) -> map` |
| `open_as` | `fn(name, cursor_name) -> map` |
| `send` | `fn(ch, msg)` |
| `try_recv` | `fn(ch)` |
| `recv` | `fn(ch)` |
| `recv_timeout` | `fn(ch, timeout_ms)` |
| `peek` | `fn(ch)` |
| `count` | `fn(ch) -> int` |
| `total` | `fn(ch) -> int` |
| `drain` | `fn(ch) -> [map]` |
| `purge` | `fn(ch)` |
| `close` | `fn(ch)` |
| `destroy` | `fn(name)` |

### std.cli

std/cli.a -- terminal colors and CLI formatting utilities

| Function | Signature |
|----------|-----------|
| `esc_code` | `fn() -> str` |
| `wrap` | `fn(s: str, code: str) -> str` |
| `red` | `fn(s: str) -> str` |
| `green` | `fn(s: str) -> str` |
| `yellow` | `fn(s: str) -> str` |
| `blue` | `fn(s: str) -> str` |
| `magenta` | `fn(s: str) -> str` |
| `cyan` | `fn(s: str) -> str` |
| `gray` | `fn(s: str) -> str` |
| `bold` | `fn(s: str) -> str` |
| `dim` | `fn(s: str) -> str` |
| `underline` | `fn(s: str) -> str` |

### std.codegen

std/codegen.a -- Code generation, compilation checking, sandboxed execution, and testing. Part of the self-improvement loop: the language can analyze, generate, test, and modify its own code.

| Function | Signature |
|----------|-----------|
| `compile_check` | `fn(source)` |
| `run_in_sandbox` | `fn(source, opts)` |
| `test` | `fn(source, test_cases)` |
| `generate` | `fn(description, context, opts)` |

### std.collections

std/collections.a -- Higher-order collection utilities

| Function | Signature |
|----------|-----------|
| `group_by` | `fn(arr, key_fn) -> map` |
| `freq` | `fn(arr) -> map` |
| `partition` | `fn(arr, pred_fn) -> [array]` |
| `flatten` | `fn(arr) -> array` |
| `index_of` | `fn(arr, target) -> int` |
| `last_index_of` | `fn(arr, target) -> int` |
| `compact` | `fn(arr) -> array` |
| `interleave` | `fn(a, b) -> array` |
| `sliding_window` | `fn(arr, size: int) -> [array]` |
| `pluck` | `fn(arr, key: str) -> array` |
| `key_by` | `fn(arr, key: str) -> map` |
| `sum_by` | `fn(arr, val_fn) -> int` |
| `count_by` | `fn(arr, pred_fn) -> int` |
| `first` | `fn(arr, pred_fn)` |
| `last` | `fn(arr, pred_fn)` |
| `uniq_by` | `fn(arr, key_fn) -> array` |
| `pairs_to_map` | `fn(arr) -> map` |
| `repeat_val` | `fn(val, n: int) -> array` |

### std.config

std/config.a -- Layered configuration loading

| Function | Signature |
|----------|-----------|
| `load` | `fn(path: str) -> map` |
| `from_env` | `fn(prefix: str) -> map` |
| `dotenv` | `fn(path: str) -> void` |
| `merge` | `fn(base: map, over: map) -> map` |
| `require` | `fn(cfg: map, keys)` |

### std.cron

std/cron.a -- Lightweight task scheduler schedule, once, cancel, run_loop, run_for

| Function | Signature |
|----------|-----------|
| `schedule` | `fn(interval_ms, f) -> map` |
| `once` | `fn(delay_ms, f) -> map` |
| `cancel` | `fn(task) -> map` |
| `run_loop` | `fn(tasks) -> void` |
| `run_for` | `fn(tasks, duration_ms) -> void` |

### std.csv

std/csv.a -- CSV parsing and generation (RFC 4180)

| Function | Signature |
|----------|-----------|
| `escape_field` | `fn(field: str) -> str` |
| `parse_row` | `fn(line: str) -> [str]` |
| `parse` | `fn(text: str) -> [[str]]` |
| `parse_records` | `fn(text: str) -> [map]` |
| `stringify` | `fn(rows: [[str]]) -> str` |
| `stringify_records` | `fn(records: [map], headers: [str]) -> str` |

### std.datetime

std/datetime.a -- Date and time utilities Epoch-based: all timestamps are Unix epoch milliseconds (i64)

| Function | Signature |
|----------|-----------|
| `now` | `fn() -> int` |
| `timestamp` | `fn() -> int` |
| `sleep` | `fn(ms: int) -> void` |
| `to_parts` | `fn(epoch_ms: int) -> map` |
| `from_parts` | `fn(y: int, mo: int, d: int, h: int, mi: int, s: int) -> int` |
| `format` | `fn(epoch_ms: int, fmt: str) -> str` |
| `iso` | `fn(epoch_ms: int) -> str` |
| `add_ms` | `fn(epoch_ms: int, ms: int) -> int` |
| `add_seconds` | `fn(epoch_ms: int, s: int) -> int` |
| `add_minutes` | `fn(epoch_ms: int, m: int) -> int` |
| `add_hours` | `fn(epoch_ms: int, h: int) -> int` |
| `add_days` | `fn(epoch_ms: int, d: int) -> int` |
| `diff_ms` | `fn(a: int, b: int) -> int` |
| `diff_seconds` | `fn(a: int, b: int) -> int` |
| `diff_minutes` | `fn(a: int, b: int) -> int` |
| `diff_hours` | `fn(a: int, b: int) -> int` |
| `diff_days` | `fn(a: int, b: int) -> int` |
| `unix` | `fn(epoch_ms: int) -> int` |
| `from_unix` | `fn(secs: int) -> int` |
| `is_leap_year` | `fn(y: int) -> bool` |
| `days_in_month` | `fn(y: int, m: int) -> int` |
| `day_of_week` | `fn(epoch_ms: int) -> int` |
| `day_of_year` | `fn(epoch_ms: int) -> int` |
| `week_of_year` | `fn(epoch_ms: int) -> int` |
| `start_of_day` | `fn(epoch_ms: int) -> int` |
| `end_of_day` | `fn(epoch_ms: int) -> int` |
| `start_of_month` | `fn(epoch_ms: int) -> int` |
| `end_of_month` | `fn(epoch_ms: int) -> int` |
| `start_of_year` | `fn(epoch_ms: int) -> int` |
| `is_before` | `fn(a: int, b: int) -> bool` |
| `is_after` | `fn(a: int, b: int) -> bool` |
| `is_same_day` | `fn(a: int, b: int) -> bool` |
| `add_months` | `fn(epoch_ms: int, months: int) -> int` |
| `add_years` | `fn(epoch_ms: int, years: int) -> int` |
| `day_name` | `fn(epoch_ms: int) -> str` |
| `month_name` | `fn(epoch_ms: int) -> str` |
| `parse_iso` | `fn(s: str) -> int` |
| `relative` | `fn(epoch_ms: int) -> str` |

### std.diff

std/diff.a -- Text diff (Myers algorithm), patch, unified format

| Function | Signature |
|----------|-----------|
| `lines` | `fn(a: str, b: str) -> array` |
| `text` | `fn(a: str, b: str) -> str` |
| `patch` | `fn(original: str, diff_str: str) -> str` |

### std.encoding

std/encoding.a -- Base64, hex, and URL encoding/decoding

| Function | Signature |
|----------|-----------|
| `base64_encode` | `fn(s: str) -> str` |
| `base64_decode` | `fn(s: str) -> str` |
| `hex_encode` | `fn(s: str) -> str` |
| `hex_decode` | `fn(s: str) -> str` |
| `url_encode` | `fn(s: str) -> str` |
| `url_decode` | `fn(s: str) -> str` |
| `base64url_encode` | `fn(s: str) -> str` |
| `base64url_decode` | `fn(s: str) -> str` |
| `html_escape` | `fn(s: str) -> str` |
| `html_unescape` | `fn(s: str) -> str` |
| `json_escape` | `fn(s: str) -> str` |
| `json_unescape` | `fn(s: str) -> str` |
| `rot13` | `fn(s: str) -> str` |

### std.fs_tx

std/fs_tx.a -- Transactional file operations with automatic rollback

| Function | Signature |
|----------|-----------|
| `begin` | `fn(paths) -> map` |
| `commit` | `fn(tx) -> map` |
| `rollback` | `fn(tx) -> map` |
| `run` | `fn(paths, f)` |

### std.git

std/git.a -- Structured git operations

| Function | Signature |
|----------|-----------|
| `init` | `fn(path)` |
| `clone` | `fn(url, path, opts)` |
| `status` | `fn(path) -> [map]` |
| `is_clean` | `fn(path) -> bool` |
| `diff` | `fn(path, opts) -> [map]` |
| `diff_stat` | `fn(path, opts) -> [map]` |
| `log` | `fn(path, opts) -> [map]` |
| `log_short` | `fn(path, n) -> [map]` |
| `add` | `fn(path, files)` |
| `add_all` | `fn(path)` |
| `reset` | `fn(path, files)` |
| `commit` | `fn(path, message)` |
| `commit_all` | `fn(path, message)` |
| `branch` | `fn(path) -> str` |
| `branches` | `fn(path) -> [map]` |
| `checkout` | `fn(path, branch_name)` |
| `create_branch` | `fn(path, name)` |
| `delete_branch` | `fn(path, name)` |
| `merge` | `fn(path, branch_name)` |
| `remote` | `fn(path) -> [map]` |
| `remote_url` | `fn(path, name) -> str` |
| `push` | `fn(path, remote_name, branch_name)` |
| `pull` | `fn(path, remote_name, branch_name)` |
| `fetch` | `fn(path, remote_name)` |
| `tag` | `fn(path, name)` |
| `tag_annotated` | `fn(path, name, message)` |
| `tags` | `fn(path) -> [str]` |
| `delete_tag` | `fn(path, name)` |
| `stash` | `fn(path)` |
| `stash_pop` | `fn(path)` |
| `stash_list` | `fn(path) -> [str]` |
| `blame` | `fn(path, file) -> [map]` |
| `head` | `fn(path) -> str` |
| `head_short` | `fn(path) -> str` |
| `show` | `fn(path, ref, file) -> str` |
| `is_repo` | `fn(path) -> bool` |
| `root` | `fn(path) -> str` |
| `changed_files` | `fn(path, ref1, ref2) -> [str]` |
| `untracked` | `fn(path) -> [str]` |
| `tracked` | `fn(path) -> [str]` |

### std.hash

std/hash.a -- Hashing utilities (wraps hash.sha256 / hash.md5 builtins)

| Function | Signature |
|----------|-----------|
| `sha256` | `fn(s: str) -> str` |
| `md5` | `fn(s: str) -> str` |
| `sha256_file` | `fn(path: str) -> str` |
| `md5_file` | `fn(path: str) -> str` |
| `quick` | `fn(s: str) -> str` |
| `hash_int` | `fn(s: str) -> int` |
| `equal` | `fn(a: str, b: str) -> bool` |
| `combine` | `fn(a: str, b: str) -> str` |
| `sha256_hex` | `fn(s: str) -> str` |
| `md5_hex` | `fn(s: str) -> str` |
| `fingerprint` | `fn(s: str) -> str` |

### std.html

std/html.a -- HTML parser, DOM tree, and CSS selector engine Parses HTML into a tree of tagged maps, queries with CSS selectors. Node: #{ "tag": "div", "attrs": #{ "class": "main" }, "children": [...] } Text: #{ "tag": "#text", "text": "hello" }

| Function | Signature |
|----------|-----------|
| `parse` | `fn(html)` |
| `text` | `fn(node) -> str` |
| `select` | `fn(node, selector) -> [map]` |

### std.index

std/index.a -- Codebase intelligence

| Function | Signature |
|----------|-----------|
| `create` | `fn(project_path, opts) -> map` |
| `files` | `fn(idx) -> [str]` |
| `symbols` | `fn(idx) -> [map]` |
| `find` | `fn(idx, name) -> [map]` |
| `find_fuzzy` | `fn(idx, pattern) -> [map]` |
| `functions` | `fn(idx) -> [map]` |
| `types` | `fn(idx) -> [map]` |
| `symbols_in` | `fn(idx, file) -> [map]` |
| `exports` | `fn(idx, file) -> [map]` |
| `imports` | `fn(idx, file) -> [str]` |
| `callers` | `fn(idx, fn_name) -> [map]` |
| `callees` | `fn(idx, fn_name) -> [str]` |
| `call_graph` | `fn(idx) -> map` |
| `dependencies` | `fn(idx, file) -> [str]` |
| `dependents` | `fn(idx, file) -> [str]` |
| `search` | `fn(idx, query) -> [map]` |
| `stats` | `fn(idx) -> map` |
| `save` | `fn(idx, path)` |
| `load` | `fn(path) -> map` |
| `update` | `fn(idx, changed_files) -> map` |

### std.kv

std/kv.a -- Persistent key-value store backed by SQLite

| Function | Signature |
|----------|-----------|
| `open` | `fn(path)` |
| `close` | `fn(handle)` |
| `get` | `fn(handle, key)` |
| `set` | `fn(handle, key, value)` |
| `delete` | `fn(handle, key)` |
| `has` | `fn(handle, key) -> bool` |
| `list` | `fn(handle, prefix) -> [map]` |
| `keys` | `fn(handle) -> [str]` |
| `count` | `fn(handle) -> int` |
| `clear` | `fn(handle)` |

### std.lexer

Standard "a" language lexer module. Tokenizes "a" source into a flat array: [kind0, text0, kind1, text1, ...]

| Function | Signature |
|----------|-----------|
| `is_ws` | `fn(c: str) -> bool` |
| `is_id_start` | `fn(c: str) -> bool` |
| `is_id_char` | `fn(c: str) -> bool` |
| `keyword_or_ident` | `fn(word: str) -> str` |
| `lex` | `fn(src: str) -> [str]` |
| `tk` | `fn(toks: [str], i: i64) -> str` |
| `tv` | `fn(toks: [str], i: i64) -> str` |
| `tcount` | `fn(toks: [str]) -> i64` |
| `skip_nl` | `fn(toks: [str], p: i64) -> i64` |

### std.llm

std/llm.a -- Unified LLM client for OpenAI, Anthropic, and Google AI Pure "a" implementation using http.post, json.parse/stringify, env.get.

| Function | Signature |
|----------|-----------|
| `chat` | `fn(provider, model, messages, opts)` |
| `stream` | `fn(provider, model, messages, on_chunk, opts)` |
| `models` | `fn(provider) -> [str]` |

### std.local_llm

std/local_llm.a -- Local LLM inference via GGUF models

| Function | Signature |
|----------|-----------|
| `load` | `fn(model_path)` |
| `generate` | `fn(handle, prompt, opts)` |
| `chat` | `fn(handle, messages, opts)` |
| `embed` | `fn(handle, text)` |
| `info` | `fn(handle)` |
| `unload` | `fn(handle)` |
| `tokenize` | `fn(handle, text)` |
| `detokenize` | `fn(handle, tokens)` |
| `vocab_size` | `fn(handle)` |
| `is_loaded` | `fn(handle)` |
| `complete` | `fn(handle, prompt, max_tokens)` |
| `classify` | `fn(handle, text, labels)` |
| `summarize` | `fn(handle, text, max_len)` |

### std.log

std/log.a -- Structured JSON logging to stderr

| Function | Signature |
|----------|-----------|
| `set_level` | `fn(level: str) -> void` |
| `log` | `fn(level: str, msg: str, ctx) -> void` |
| `debug` | `fn(msg: str) -> void` |
| `info` | `fn(msg: str) -> void` |
| `warn` | `fn(msg: str) -> void` |
| `error` | `fn(msg: str) -> void` |
| `with` | `fn(key: str, val) -> map` |
| `debugf` | `fn(msg: str, ctx) -> void` |
| `infof` | `fn(msg: str, ctx) -> void` |
| `warnf` | `fn(msg: str, ctx) -> void` |
| `errorf` | `fn(msg: str, ctx) -> void` |
| `timed` | `fn(label: str, f) -> void` |
| `with2` | `fn(k1: str, v1, k2: str, v2) -> map` |
| `child` | `fn(base_ctx) -> map` |
| `child_log` | `fn(level: str, msg: str, base_ctx, extra) -> void` |

### std.math

| Function | Signature |
|----------|-----------|
| `max` | `fn(a: i64, b: i64) -> i64` |
| `min` | `fn(a: i64, b: i64) -> i64` |
| `clamp` | `fn(val: i64, lo: i64, hi: i64) -> i64` |
| `fmax` | `fn(a: f64, b: f64) -> f64` |
| `fmin` | `fn(a: f64, b: f64) -> f64` |
| `pow` | `fn(base: i64, exp: i64) -> i64` |
| `sum` | `fn(arr: [i64]) -> i64` |
| `range` | `fn(start: i64, end: i64) -> [i64]` |
| `range_step` | `fn(start: i64, end: i64, step: i64) -> [i64]` |
| `sign` | `fn(x: i64) -> i64` |
| `fsign` | `fn(x: f64) -> f64` |
| `is_even` | `fn(n: i64) -> bool` |
| `is_odd` | `fn(n: i64) -> bool` |
| `gcd` | `fn(a: i64, b: i64) -> i64` |
| `lcm` | `fn(a: i64, b: i64) -> i64` |
| `factorial` | `fn(n: i64) -> i64` |
| `divmod` | `fn(a: i64, b: i64) -> [i64]` |
| `wrap` | `fn(val: i64, lo: i64, hi: i64) -> i64` |
| `fclamp` | `fn(val: f64, lo: f64, hi: f64) -> f64` |
| `fpow` | `fn(base: f64, exp: i64) -> f64` |
| `lerp` | `fn(a: f64, b: f64, t: f64) -> f64` |
| `map_range` | `fn(val: f64, in_lo: f64, in_hi: f64, out_lo: f64, out_hi: f64) -> f64` |
| `average` | `fn(arr: [i64]) -> f64` |
| `fsum` | `fn(arr: [f64]) -> f64` |
| `faverage` | `fn(arr: [f64]) -> f64` |
| `product` | `fn(arr: [i64]) -> i64` |
| `median` | `fn(arr: [i64]) -> f64` |

### std.mcp

std/mcp.a -- MCP (Model Context Protocol) server and client JSON-RPC 2.0 over stdio transport.

| Function | Signature |
|----------|-----------|
| `server` | `fn(name, version)` |
| `add_tool` | `fn(srv, name, description, input_schema, handler)` |
| `add_resource` | `fn(srv, uri, name, description, handler)` |
| `serve` | `fn(srv)` |
| `connect` | `fn(cmd)` |
| `list_tools` | `fn(client)` |
| `call_tool` | `fn(client, name, args)` |
| `list_resources` | `fn(client)` |
| `read_resource` | `fn(client, uri)` |
| `close` | `fn(client)` |

### std.meta

std/meta.a Metaprogramming toolkit for the "a" language. High-level utilities for AST walking, searching, transformation, and code generation. Built on std/compiler/parser.a, emitter.a, and ast.a.

| Function | Signature |
|----------|-----------|
| `parse` | `fn(source)` |
| `parse_file` | `fn(path)` |
| `emit` | `fn(prog_ast)` |
| `emit_to_file` | `fn(prog_ast, path)` |
| `walk` | `fn(node, visitor_fn)` |
| `collect_inner` | `fn(node, pred_fn, results)` |
| `find_all` | `fn(node, pred_fn)` |
| `find_fns` | `fn(prog_ast)` |
| `find_calls` | `fn(node)` |
| `fn_names` | `fn(prog_ast)` |
| `fn_signatures` | `fn(prog_ast)` |
| `uses` | `fn(prog_ast)` |
| `extract_call_name` | `fn(call_node)` |
| `call_graph` | `fn(prog_ast)` |
| `transform` | `fn(source, transform_fn)` |
| `transform_file` | `fn(path, transform_fn)` |
| `map_items` | `fn(prog_ast, map_fn)` |
| `filter_items` | `fn(prog_ast, pred_fn)` |
| `add_items` | `fn(prog_ast, new_items)` |
| `inject_stmt` | `fn(fn_node, position, stmt_node)` |
| `gen_fn` | `fn(name, param_names, body_stmts)` |
| `gen_test` | `fn(name, body_stmts)` |
| `gen_call` | `fn(fn_name, arg_exprs)` |

### std.migrate

std/migrate.a -- SQLite migration runner

| Function | Signature |
|----------|-----------|
| `run` | `fn(handle, dir: str) -> map` |
| `status` | `fn(handle, dir: str)` |
| `create` | `fn(dir: str, name: str) -> str` |

### std.path

std/path.a -- Path manipulation utilities

| Function | Signature |
|----------|-----------|
| `join` | `fn(a: str, b: str) -> str` |
| `join3` | `fn(a: str, b: str, c: str) -> str` |
| `dirname` | `fn(p: str) -> str` |
| `basename` | `fn(p: str) -> str` |
| `extension` | `fn(p: str) -> str` |
| `stem` | `fn(p: str) -> str` |
| `with_extension` | `fn(p: str, ext: str) -> str` |
| `is_absolute` | `fn(p: str) -> bool` |
| `segments` | `fn(p: str) -> [str]` |
| `normalize` | `fn(p: str) -> str` |
| `home` | `fn() -> str` |
| `a_home` | `fn() -> str` |
| `temp` | `fn() -> str` |
| `relative` | `fn(from: str, to: str) -> str` |
| `has_extension` | `fn(p: str, ext: str) -> bool` |
| `join_all` | `fn(parts: [str]) -> str` |
| `is_hidden` | `fn(p: str) -> bool` |

### std.pkg

std/pkg.a -- Package manager: manifest, dependency resolution, git fetch

| Function | Signature |
|----------|-----------|
| `parse_source` | `fn(source: str) -> map` |
| `init` | `fn(dir: str) -> void` |
| `read_manifest` | `fn(dir: str) -> map` |
| `write_manifest` | `fn(dir: str, manifest: map) -> void` |
| `add_dep` | `fn(dir: str, name: str, source: str) -> void` |
| `install` | `fn(dir: str) -> map` |

### std.plan

std/plan.a -- Structured task decomposition for deliberative agents Plans are data: serializable, inspectable, resumable. A failed step marks itself failed and lets the agent decide.

| Function | Signature |
|----------|-----------|
| `create` | `fn(goal) -> map` |
| `add_step` | `fn(p, id, description, deps) -> map` |
| `execute` | `fn(p, step_fn) -> map` |
| `get_step` | `fn(p, id)` |
| `get_result` | `fn(p, id)` |
| `mark_done` | `fn(p, id, value) -> map` |
| `mark_failed` | `fn(p, id, reason) -> map` |
| `reset_step` | `fn(p, id) -> map` |
| `status` | `fn(p) -> map` |
| `summary` | `fn(p) -> str` |
| `serialize` | `fn(p) -> str` |
| `deserialize` | `fn(json_str) -> map` |
| `steps` | `fn(p)` |
| `step_ids` | `fn(p) -> [str]` |
| `is_complete` | `fn(p) -> bool` |
| `failed_steps` | `fn(p) -> [map]` |

### std.plugin

std/plugin.a -- Plugin system for runtime extensibility Plugins are "a" modules installed to <a_home>/plugins/ with metadata, where <a_home> is $A_HOME or ~/.a (see path.a_home).

| Function | Signature |
|----------|-----------|
| `install` | `fn(source_dir)` |
| `install_git` | `fn(repo)` |
| `remove` | `fn(name)` |
| `list` | `fn() -> [map]` |
| `get` | `fn(name)` |
| `is_installed` | `fn(name) -> bool` |
| `load` | `fn(name)` |
| `run` | `fn(name)` |
| `create_manifest` | `fn(name, version, desc, ep) -> str` |
| `init` | `fn(dir, name)` |

### std.pool

std/pool.a -- Generic resource pool with configurable max size Uses functional state: pool operations return updated pool maps.

| Function | Signature |
|----------|-----------|
| `create` | `fn(factory, max_size) -> map` |
| `acquire` | `fn(p)` |
| `release` | `fn(p, conn) -> map` |
| `drain` | `fn(p) -> map` |
| `size` | `fn(p) -> int` |
| `stats` | `fn(p) -> map` |

### std.re

| Function | Signature |
|----------|-----------|
| `is_meta` | `fn(c: str) -> bool` |
| `class_match` | `fn(class_chars: [str], negated: bool, c: str) -> bool` |
| `shorthand_match` | `fn(code: str, c: str) -> bool` |
| `parse_pattern` | `fn(pat: str) -> [[str]]` |
| `node_matches_char` | `fn(node: [str], c: str) -> bool` |
| `is_quantifier` | `fn(nodes: [[str]], idx: i64) -> str` |
| `try_match` | `fn(nodes: [[str]], ni: i64, text_chars: [str], ti: i64) -> i64` |
| `handle_alternation` | `fn(nodes: [[str]], text_chars: [str], start: i64) -> i64` |
| `has_alt` | `fn(nodes: [[str]]) -> bool` |
| `match_full` | `fn(pattern: str, text: str) -> bool` |
| `search` | `fn(pattern: str, text: str) -> [str]` |
| `find_all` | `fn(pattern: str, text: str) -> [[str]]` |
| `test` | `fn(pattern: str, text: str) -> bool` |
| `replace` | `fn(pattern: str, text: str, replacement: str) -> str` |
| `replace_first` | `fn(pattern: str, text: str, replacement: str) -> str` |
| `split` | `fn(pattern: str, text: str) -> [str]` |

### std.refactor

std/refactor.a -- AST-level refactoring: rename, extract function, inline function. All functions take source strings and return transformed source strings.

| Function | Signature |
|----------|-----------|
| `rename` | `fn(source, old_name, new_name) -> str` |
| `extract_fn` | `fn(source, start_line, end_line, fn_name) -> str` |
| `inline_fn` | `fn(source, fn_name) -> str` |

### std.reflect

std/reflect.a -- Runtime self-inspection for agents Combines C builtins (memory, uptime, pid) with pure-"a" instrumentation (call counters, hot path tracking).

| Function | Signature |
|----------|-----------|
| `memory_mb` | `fn() -> float` |
| `memory_kb` | `fn() -> int` |
| `info` | `fn() -> map` |
| `profiler` | `fn() -> map` |
| `tick` | `fn(p, name) -> map` |
| `tick_ms` | `fn(p, name, elapsed_ms) -> map` |
| `call_count` | `fn(p, name) -> int` |
| `total_time` | `fn(p, name) -> int` |
| `avg_time` | `fn(p, name) -> float` |
| `hot_paths` | `fn(p, limit) -> [map]` |
| `stats` | `fn(p) -> map` |
| `reset` | `fn(p) -> map` |
| `health` | `fn() -> map` |

### std.rpc

std/rpc.a -- Lightweight RPC over HTTP with service discovery JSON-RPC 2.0 over HTTP POST. Agents register by name, discover each other.

| Function | Signature |
|----------|-----------|
| `register` | `fn(name, port)` |
| `unregister` | `fn(name)` |
| `discover` | `fn(name)` |
| `list_agents` | `fn() -> [map]` |
| `call_addr` | `fn(addr, method, params)` |
| `call` | `fn(target, method, params)` |
| `notify_addr` | `fn(addr, method, params)` |
| `notify` | `fn(target, method, params)` |
| `serve` | `fn(name, port, handlers)` |

### std.sandbox

std/sandbox.a -- run untrusted `a` code under a capability set.

| Function | Signature |
|----------|-----------|
| `capabilities` | `fn(opts) -> map` |
| `deny_all` | `fn() -> map` |
| `allow_all` | `fn() -> map` |
| `allow_read_only` | `fn() -> map` |
| `allow_network` | `fn() -> map` |
| `denied_effects` | `fn(program_effects: [str], caps: map) -> [str]` |
| `analyze` | `fn(source: str) -> map` |
| `validate` | `fn(source) -> map` |
| `run` | `fn(source, caps) -> map` |
| `run_file` | `fn(fpath, caps) -> map` |

### std.schema

std/schema.a -- JSON Schema (draft-07 subset) validation

| Function | Signature |
|----------|-----------|
| `validate` | `fn(value, schema)` |
| `from_type` | `fn(type_str: str) -> map` |

### std.semver

std/semver.a -- Semantic versioning: parse, compare, satisfies

| Function | Signature |
|----------|-----------|
| `parse` | `fn(s: str) -> map` |
| `format` | `fn(v: map) -> str` |
| `compare` | `fn(a: map, b: map) -> int` |
| `satisfies` | `fn(version: str, constraint: str) -> bool` |
| `best_match` | `fn(versions, constraint: str) -> str` |

### std.strings

| Function | Signature |
|----------|-----------|
| `repeat` | `fn(s: str, n: i64) -> str` |
| `pad_left` | `fn(s: str, width: i64, pad: str) -> str` |
| `pad_right` | `fn(s: str, width: i64, pad: str) -> str` |
| `reverse` | `fn(s: str) -> str` |
| `count` | `fn(s: str, sub: str) -> i64` |
| `center` | `fn(s: str, width: i64, pad: str) -> str` |
| `is_empty` | `fn(s: str) -> bool` |
| `char_at` | `fn(s: str, idx: i64) -> str` |
| `chars_count` | `fn(s: str) -> i64` |
| `truncate` | `fn(s: str, max_len: i64, suffix: str) -> str` |
| `words` | `fn(s: str) -> [str]` |
| `capitalize` | `fn(s: str) -> str` |
| `title_case` | `fn(s: str) -> str` |
| `snake_case` | `fn(s: str) -> str` |
| `camel_case` | `fn(s: str) -> str` |
| `kebab_case` | `fn(s: str) -> str` |
| `is_numeric` | `fn(s: str) -> bool` |
| `is_upper` | `fn(s: str) -> bool` |
| `is_lower` | `fn(s: str) -> bool` |
| `indent` | `fn(s: str, n: i64) -> str` |
| `dedent` | `fn(s: str) -> str` |
| `squeeze` | `fn(s: str) -> str` |
| `strip_prefix` | `fn(s: str, prefix: str) -> str` |
| `strip_suffix` | `fn(s: str, suffix: str) -> str` |
| `join_with` | `fn(arr: [str], sep: str, last_sep: str) -> str` |
| `wrap_text` | `fn(s: str, width: i64) -> str` |
| `replace_first` | `fn(s: str, old: str, new: str) -> str` |

### std.swarm

std/swarm.a -- Swarm coordination primitives Multi-agent strategies: divide, vote, race, chain.

| Function | Signature |
|----------|-----------|
| `create` | `fn(task, agents, strategy)` |
| `create_async` | `fn(task, agents, strategy)` |
| `cancel` | `fn(agents)` |

### std.template

std/template.a -- Mustache-style string templating Supports: var, #if key...{{/if}}, #else, #each arr..../each, dot, @index

| Function | Signature |
|----------|-----------|
| `render` | `fn(tmpl: str, vars) -> str` |
| `render_file` | `fn(path: str, vars) -> str` |

### std.testgen

std/testgen.a -- Automatic test generation from source analysis

| Function | Signature |
|----------|-----------|
| `gen_tests` | `fn(source) -> str` |
| `gen_tests_for_file` | `fn(filepath) -> str` |
| `analyze` | `fn(source) -> map` |

### std.testing

Test assertion library for "a". Assertions call fail() on mismatch, which is a catchable runtime failure. `a test` runs every `test_*.a` file. If a file has no `fn main` but has nullary `fn test_*()`, the compiler synthesizes a main that calls them.

| Function | Signature |
|----------|-----------|
| `assert_eq` | `fn(actual: any, expected: any) -> void` |
| `assert_int_eq` | `fn(actual: i64, expected: i64) -> void` |
| `assert_true` | `fn(val: bool) -> void` |
| `assert_false` | `fn(val: bool) -> void` |
| `assert_contains` | `fn(haystack: str, needle: str) -> void` |
| `assert_not_eq` | `fn(actual: any, expected: any) -> void` |
| `assert_float_eq` | `fn(actual: f64, expected: f64, epsilon: f64) -> void` |
| `assert_gt` | `fn(actual: i64, expected: i64) -> void` |
| `assert_gte` | `fn(actual: i64, expected: i64) -> void` |
| `assert_lt` | `fn(actual: i64, expected: i64) -> void` |
| `assert_lte` | `fn(actual: i64, expected: i64) -> void` |
| `assert_len` | `fn(arr, expected: i64) -> void` |
| `assert_empty` | `fn(arr) -> void` |
| `assert_not_empty` | `fn(arr) -> void` |
| `assert_starts_with` | `fn(s: str, prefix: str) -> void` |
| `assert_ends_with` | `fn(s: str, suffix: str) -> void` |
| `assert_int_not_eq` | `fn(actual: i64, expected: i64) -> void` |
| `assert_between` | `fn(val: i64, lo: i64, hi: i64) -> void` |
| `assert_not_contains` | `fn(haystack: str, needle: str) -> void` |

### std.toml

std/toml.a -- TOML parser and emitter Supports: key-value pairs, tables [section], arrays of tables [[section]], basic/literal strings, integers, floats, booleans, arrays, inline tables, dotted keys, and comments.

| Function | Signature |
|----------|-----------|
| `parse` | `fn(text)` |
| `stringify` | `fn(val) -> str` |

### std.trace

std/trace.a -- Execution tracing and structured timeline Captures function calls, decisions, API responses as structured events. Exports to JSON array or Chrome trace viewer format (chrome://tracing).

| Function | Signature |
|----------|-----------|
| `create` | `fn(name) -> map` |
| `begin` | `fn(t, name) -> map` |
| `end` | `fn(t, name, data) -> map` |
| `event` | `fn(t, name, data) -> map` |
| `counter` | `fn(t, name, values) -> map` |
| `measure` | `fn(t, name, f) -> map` |
| `duration_ms` | `fn(t) -> int` |
| `count` | `fn(t) -> int` |
| `spans` | `fn(t) -> [map]` |
| `find_events` | `fn(t, name) -> [map]` |
| `export_json` | `fn(t) -> str` |
| `export_chrome` | `fn(t) -> str` |
| `export_otel` | `fn(t) -> str` |
| `clear` | `fn(t) -> map` |

### std.url

std/url.a -- URL parsing, encoding, and building parse("https://user:pass@host:8080/path?q=1&r=2#frag") returns a map with scheme, user, password, host, port, path, query, params, fragment.

| Function | Signature |
|----------|-----------|
| `parse` | `fn(url_str)` |
| `encode` | `fn(s) -> str` |
| `decode` | `fn(s) -> str` |
| `build` | `fn(parts) -> str` |

### std.uuid

std/uuid.a -- UUID generation and validation

| Function | Signature |
|----------|-----------|
| `v4` | `fn() -> str` |
| `nil` | `fn() -> str` |
| `is_valid` | `fn(s: str) -> bool` |
| `is_nil` | `fn(s: str) -> bool` |
| `short` | `fn(s: str) -> str` |
| `from_short` | `fn(s: str) -> str` |

### std.vector

std/vector.a -- In-process vector store backed by SQLite

| Function | Signature |
|----------|-----------|
| `open` | `fn(path, dim) -> map` |
| `close` | `fn(store)` |
| `add` | `fn(store, id, embedding)` |
| `add_with` | `fn(store, id, embedding, metadata)` |
| `remove` | `fn(store, id)` |
| `count` | `fn(store) -> int` |
| `get` | `fn(store, id)` |
| `search` | `fn(store, query_embedding, k) -> [map]` |
| `clear` | `fn(store)` |

### std.yaml

std/yaml.a -- YAML 1.2 subset parser and emitter Supports: mappings, sequences, nested structures, scalars (strings, integers, floats, booleans, null), single/double-quoted strings, flow sequences [...], flow mappings {...}, block scalars (| and >), and comments.

| Function | Signature |
|----------|-----------|
| `stringify` | `fn(val) -> str` |
| `parse` | `fn(text)` |

### Compiler modules

These are the self-hosting toolchain. Import with `use std.compiler.<name>`.

| Module | Summary |
|--------|----------|
| `std.compiler.ast` | Self-hosted AST node constructors. Every node is a tagged map: #{"tag": "NodeType", ...fields} This mirrors src/ast.rs exactly. |
| `std.compiler.builtin_arity` | GENERATED by scripts/gen_builtin_arity.a from c_runtime/runtime.h -- do not edit. Builtin name -> number of arguments, for builtins with a fixed arity. |
| `std.compiler.builtin_sigs` | std/compiler/builtin_sigs.a -- The one table describing every builtin. |
| `std.compiler.cgen` | std/compiler/cgen.a -- C code generation backend Reads an "a" source file, parses to AST, emits C source to stdout. Supports `use` module inlining, closures/lambdas, HOFs, pipes. Usage: a run std/compiler/cgen.a -- input.a > output.c |
| `std.compiler.checker` | std/compiler/checker.a -- Static analysis: scope, arity, unused variables, unreachable code. |
| `std.compiler.compiler` | Self-hosted bytecode compiler for the "a" language. Mirrors src/compiler.rs: tagged-map AST -> bytecode opcodes. |
| `std.compiler.describe` | What an agent needs to write `a`, taken from the compiler rather than a hand-maintained cheat sheet. `a describe` prints `prompt`; `a describe --json` prints `as_json`. |
| `std.compiler.diag_codes` | Diagnostic codes for the checker. |
| `std.compiler.docgen` | Generates the builtin and standard-library sections of REFERENCE.md from the signature table and the source tree. Sections 1-11 and the closing example stay handwritten. `a doc --check` fails when the generated text is stale. |
| `std.compiler.effect_check` | Effect inference. |
| `std.compiler.emitter` | std/compiler/emitter.a AST-to-source pretty printer for the "a" language. Converts tagged-map ASTs (from std/compiler/ast.a) into formatted source code. This closes the metaprogramming loop: source -> parse -> transform -> emit -> source. |
| `std.compiler.lexer` | Self-hosted "a" language lexer -- full parity with src/lexer.rs Tokenizes source into flat array: [kind0, text0, kind1, text1, ...] Supports string interpolation (InterpStart, InterpMid, InterpEnd), DotDotDot (...), Underscore (_), and all escape sequences. |
| `std.compiler.optimizer` | std/compiler/optimizer.a -- Compiler self-improvement and optimization analysis |
| `std.compiler.parser` | Self-hosted recursive-descent parser for the "a" language. Produces tagged-map AST nodes (see std/compiler/ast.a). Every parse function takes (toks, pos) and returns [result, new_pos]. On error, result is #{"tag": "ParseError", "msg": "..."}. |
| `std.compiler.profiler` | std/compiler/profiler.a -- Profile-guided optimization instrumentation |
| `std.compiler.serialize` |  |
| `std.compiler.types` | std/compiler/types.a -- Gradual type representation for the checker. |
| `std.compiler.wasmgen` | std/compiler/wasmgen.a -- WebAssembly Text Format (WAT) code generator Generates WAT from the "a" AST, targeting WASI for I/O. |

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
