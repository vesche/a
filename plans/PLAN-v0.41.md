# v0.41 -- C Code Generation

## Motivation

The self-hosted compiler can lex, parse, and compile "a" to bytecode. But that bytecode still runs on the Rust VM. The language is not truly independent.

v0.41 adds a C code generation backend. The self-hosted compiler's parser produces an AST; a new code generator walks that AST and emits C source code. Combined with a small C runtime library (~600 lines), this produces native executables via `gcc`. No Rust, no VM, no interpreter. Just machine code.

This is the first step toward full bootstrap: eventually the C code generator itself will be compiled to C, and "a" will exist without any Rust dependency.

## Architecture

```
program.a → parser (std.compiler.parser) → AST → cgen (new) → program.c
program.c + runtime.c → gcc → native binary
```

The C code generator (`std/compiler/cgen.a`) is written in "a" itself, using the existing parser and AST infrastructure. It runs on the Rust VM to produce C source, which is then compiled with any C compiler.

## C Runtime Library

A small C library providing the value model, memory management, and essential builtins. Lives in `c_runtime/runtime.h` and `c_runtime/runtime.c`.

### Value Representation

A tagged struct, ~16 bytes on 64-bit systems:

```c
typedef enum {
    TAG_INT, TAG_FLOAT, TAG_BOOL, TAG_VOID,
    TAG_STRING, TAG_ARRAY, TAG_MAP, TAG_RESULT
} ATag;

typedef struct AValue {
    ATag tag;
    union {
        int64_t ival;
        double fval;
        int bval;
        AString* sval;
        AArray* aval;
        AMap* mval;
        struct { int is_ok; struct AValue* inner; } rval;
    };
} AValue;
```

### Heap Types (Reference Counted)

```c
typedef struct AString {
    int rc;
    int len;
    char data[];   // flexible array member
} AString;

typedef struct AArray {
    int rc;
    int len;
    int cap;
    AValue* items;
} AArray;

typedef struct AMap {
    int rc;
    int len;
    int cap;
    char** keys;
    AValue* vals;
} AMap;
```

### Memory Management

Reference counting with `retain(AValue*)` and `release(AValue*)`. Simple and predictable -- matches the `Arc` model in the Rust runtime. No garbage collector needed for v0.41.

- `retain` increments refcount on heap types (string, array, map)
- `release` decrements and frees when refcount hits zero
- Stack values (int, float, bool, void) have no heap allocation

### Runtime Functions (~600 lines total)

**Value constructors:**
- `a_int(i64)`, `a_float(f64)`, `a_bool(int)`, `a_void()`, `a_string(char*)`, `a_string_len(char*, int)`

**String operations:**
- `a_str_concat(AValue, AValue)` -- concatenate two strings
- `a_str_eq(AValue, AValue)` -- string equality
- `a_to_str(AValue)` -- convert any value to string representation
- `a_str_len(AValue)` -- string length
- `a_concat_n(int n, ...)` -- concatenate N values as strings (for interpolation)

**Array operations:**
- `a_array_new(int n, ...)` -- create array from N values
- `a_array_get(AValue, AValue)` -- index into array
- `a_array_push(AValue, AValue)` -- push (returns new array)
- `a_array_len(AValue)` -- array length
- `a_array_slice(AValue, AValue, AValue)` -- slice

**Map operations:**
- `a_map_new(int n, ...)` -- create map from N key-value pairs
- `a_map_get(AValue, AValue)` -- get by key
- `a_map_set(AValue, AValue, AValue)` -- set (returns new map)
- `a_map_has(AValue, AValue)` -- key existence check
- `a_map_keys(AValue)` -- array of keys
- `a_map_merge(AValue, AValue)` -- merge two maps

**Arithmetic and comparison:**
- `a_add`, `a_sub`, `a_mul`, `a_div`, `a_mod`, `a_neg` -- numeric ops with int/float promotion
- `a_eq`, `a_neq`, `a_lt`, `a_gt`, `a_lteq`, `a_gteq` -- comparison
- `a_not` -- boolean negation

**I/O:**
- `a_println(AValue)`, `a_print(AValue)`, `a_eprintln(AValue)`

**Result:**
- `a_ok(AValue)`, `a_err(AValue)`, `a_is_ok(AValue)`, `a_is_err(AValue)`, `a_unwrap(AValue)`

**Utility:**
- `a_type_of(AValue)` -- returns string tag name
- `a_args(int argc, char** argv)` -- program arguments as array
- `a_fail(AValue)` -- abort with error message

## C Code Generator (`std/compiler/cgen.a`)

Written in "a", ~400-500 lines. Imports `std.compiler.parser` and `std.compiler.ast`.

### How AST Maps to C

**Program structure:**

```a
fn foo(x: int, y: int) -> int {
  ret x + y
}
fn main() {
  println(foo(1, 2))
}
```

Becomes:

```c
#include "runtime.h"

AValue fn_foo(AValue x, AValue y) {
    return a_add(x, y);
}

AValue fn_main(void) {
    a_println(fn_foo(a_int(1), a_int(2)));
    return a_void();
}

int main(int argc, char** argv) {
    g_argc = argc; g_argv = argv;
    fn_main();
    return 0;
}
```

### Mapping rules

| "a" construct | C output |
|---------------|----------|
| `fn name(params) { body }` | `AValue fn_name(AValue p1, AValue p2, ...) { ... }` |
| `let x = expr` | `AValue x = expr;` |
| `let mut x = expr` | `AValue x = expr;` (same in C, mutability is implicit) |
| `x = expr` | `x = expr;` |
| Integer literal `42` | `a_int(42)` |
| Float literal `3.14` | `a_float(3.14)` |
| String literal `"hi"` | `a_string("hi")` |
| `true` / `false` | `a_bool(1)` / `a_bool(0)` |
| `x + y` | `a_add(x, y)` |
| `x == y` | `a_eq(x, y)` |
| `!x` | `a_not(x)` |
| `if cond { a } else { b }` | `if (a_truthy(cond)) { a; } else { b; }` |
| `while cond { body }` | `while (a_truthy(cond)) { body; }` |
| `for x in arr { body }` | `{ AValue __arr = arr; for (int __i = 0; __i < a_ilen(__arr); __i++) { AValue x = a_array_get(__arr, a_int(__i)); body; } }` |
| `ret expr` | `return expr;` |
| `foo(a, b)` | `fn_foo(a, b)` |
| `mod.fn(a)` | `fn_mod_fn(a)` (mangled) |
| `println(x)` | `a_println(x)` |
| `len(x)` | `a_len(x)` |
| `push(arr, x)` | `a_array_push(arr, x)` |
| `[1, 2, 3]` | `a_array_new(3, a_int(1), a_int(2), a_int(3))` |
| `#{"k": v}` | `a_map_new(1, "k", v)` |
| `map.get(m, k)` | `a_map_get(m, k)` |
| `"hello {x}"` | `a_concat_n(2, a_string("hello "), a_to_str(x))` |
| `try { expr? }` | `if (setjmp(...)) { ... } else { ... }` |
| `break` / `continue` | `break;` / `continue;` |
| `use std.X` | (inline the used functions or skip for builtins) |

### Builtins recognized by the code generator

Instead of compiling calls to user functions, these map directly to C runtime functions:

| Builtin | C function |
|---------|-----------|
| `println`, `print`, `eprintln` | `a_println`, `a_print`, `a_eprintln` |
| `len` | `a_len` |
| `push` | `a_array_push` |
| `to_str` | `a_to_str` |
| `type_of` | `a_type_of` |
| `int`, `float` | `a_to_int`, `a_to_float` |
| `sort` | `a_sort` |
| `contains` | `a_contains` |
| `str.concat` | `a_str_concat` |
| `str.split` | `a_str_split` |
| `str.contains` | `a_str_contains` |
| `str.replace` | `a_str_replace` |
| `str.trim`, `str.upper`, `str.lower` | `a_str_trim`, etc. |
| `str.join`, `str.chars`, `str.slice` | corresponding |
| `map.get`, `map.set`, `map.has`, `map.keys`, `map.values`, `map.merge` | `a_map_*` |
| `json.parse`, `json.stringify` | `a_json_parse`, `a_json_stringify` |
| `args`, `exit`, `fail` | `a_args`, `exit`, `a_fail` |
| `Ok`, `Err`, `unwrap`, `is_ok`, `is_err` | `a_ok`, `a_err`, `a_unwrap`, etc. |
| `hash.sha256`, `hash.md5` | deferred (not in minimal runtime) |
| `exec` | deferred |
| `http.*`, `fs.*` | deferred |

### What's NOT in v0.41 (deferred to future versions)

- **Closures / lambdas**: Requires function pointer + captures struct in C. Complex to get right. Deferred.
- **Higher-order builtins** (`map`, `filter`, `reduce`, `each`): Depend on closures.
- **Full pattern matching**: Variant destructuring, nested patterns. Complex codegen.
- **Tail call optimization**: `goto`-based TCO in C is possible but deferred.
- **`use` / module system**: For v0.41, all functions must be in one file or inlined.
- **`eval`**: Not possible in native code without embedding a runtime.
- **Concurrency**: `spawn`, `await`, etc. Deferred.
- **HTTP, filesystem, shell**: Need OS-level C implementations. Deferred.

### `use std.*` Handling

For v0.41, the code generator handles `use std.math` and `use std.strings` by inlining the pure-"a" library functions into the output. These modules are written in "a" and don't call any builtins that aren't in the minimal C runtime. Other std modules that depend on closures or system calls are not supported in v0.41.

## CLI Integration

Add `a emit-c program.a` command that:
1. Parses the source with the Rust parser
2. Passes the AST to the cgen module (running on the VM)
3. Outputs C source to stdout (or `-o file.c`)

This requires a small Rust-side addition to `src/main.rs` to add the `emit-c` subcommand. It invokes the self-hosted parser + cgen in "a".

Alternatively (simpler for v0.41): the C code generator is a standalone "a" program:

```
a run std/compiler/cgen.a -- program.a > program.c
gcc program.c c_runtime/runtime.c -o program -lm
./program
```

This is simpler because it requires zero Rust changes. The cgen reads a source file, parses it, generates C, and prints to stdout. Just like `examples/gen_tests.a` uses the metaprogramming toolkit.

## Test Plan

### Milestone 1: Fibonacci (pure math + recursion)

```a
fn fib(n: int) -> int {
  if n < 2 { ret n }
  ret fib(n - 1) + fib(n - 2)
}
fn main() {
  println(fib(10))
}
```

Expected: compiles to C, gcc builds it, prints `55`.

### Milestone 2: String operations

```a
fn main() {
  let name = "world"
  println("hello {name}")
  let x = str.concat("foo", "bar")
  println(x)
  println(len("abc"))
}
```

### Milestone 3: Arrays and loops

```a
fn main() {
  let arr = [1, 2, 3, 4, 5]
  let mut sum = 0
  for x in arr {
    sum = sum + x
  }
  println("sum = {sum}")
  println("len = {len(arr)}")
}
```

### Milestone 4: Maps

```a
fn main() {
  let m = #{"name": "alice", "age": 30}
  println(map.get(m, "name"))
  let m2 = map.set(m, "age", 31)
  println(map.get(m2, "age"))
}
```

### Milestone 5: Multi-function program

```a
use std.math

fn factorial(n: int) -> int {
  if n <= 1 { ret 1 }
  ret n * factorial(n - 1)
}

fn main() {
  let nums = [5, 8, 10, 12]
  for n in nums {
    let f = factorial(n)
    println("{n}! = {f}")
  }
}
```

### Milestone 6: Real program -- project line counter

A non-trivial program using strings, arrays, maps, and I/O (limited to println):

```a
fn count_lines(text: str) -> int {
  let lines = str.split(text, "\n")
  ret len(lines)
}

fn main() {
  let files = ["hello.a", "math.a", "fibonacci.a"]
  let mut total = 0
  for f in files {
    let lines = count_lines(io.read_file(f))
    println("  {f}: {lines} lines")
    total = total + lines
  }
  println("total: {total} lines")
}
```

(This milestone requires `io.read_file` in the C runtime -- a simple `fread` wrapper.)

### Test validation

For each milestone, the test is:
1. `a run program.a` produces output X (reference)
2. `a run std/compiler/cgen.a -- program.a > program.c` produces C source
3. `gcc program.c c_runtime/runtime.c -o program -lm` compiles without errors
4. `./program` produces identical output X

A test script `tests/test_cgen.sh` automates this for all milestone programs.

## File Structure

```
c_runtime/
  runtime.h          -- value types, function declarations
  runtime.c          -- runtime implementation (~600 lines)
std/compiler/
  cgen.a             -- C code generator (~400-500 lines)
examples/
  c_targets/         -- milestone test programs
    fib.a
    strings.a
    arrays.a
    maps.a
    multi_fn.a
tests/
  test_cgen.sh       -- automated compile + run + compare
```

## Implementation Order

1. **C runtime library** -- `c_runtime/runtime.h` + `c_runtime/runtime.c`
2. **Code generator core** -- `std/compiler/cgen.a` with basic expression/statement emission
3. **Milestone 1** -- fibonacci compiles and runs natively
4. **String support** -- string literals, interpolation, str.* builtins in runtime
5. **Milestone 2** -- string programs compile and run
6. **Array + loop support** -- array creation, indexing, for loops
7. **Milestone 3** -- array programs compile and run
8. **Map support** -- map creation, get/set/has
9. **Milestone 4** -- map programs compile and run
10. **Multi-function + module inlining** -- use std.math, cross-function calls
11. **Milestone 5-6** -- complex programs compile and run
12. **Test script + documentation** -- `test_cgen.sh`, update PLANNING.md + REFERENCE.md

## TODO

- [ ] Create c_runtime/runtime.h + runtime.c (value model, refcounting, arithmetic, strings, arrays, maps, I/O)
- [ ] Create std/compiler/cgen.a core (AST walking, expression emission, statement emission)
- [ ] Milestone 1: fibonacci compiles to C and runs natively
- [ ] Add string support to cgen + runtime (literals, interpolation, str.* builtins)
- [ ] Milestone 2: string programs compile and run
- [ ] Add array + loop support to cgen + runtime
- [ ] Milestone 3: array programs compile and run
- [ ] Add map support to cgen + runtime
- [ ] Milestone 4: map programs compile and run
- [ ] Add multi-function support + std.math inlining
- [ ] Milestones 5-6: complex programs compile and run
- [ ] Create test script + update docs
