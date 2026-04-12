---
name: v0.35 Language Reference
overview: Create a comprehensive REFERENCE.md for the "a" language covering syntax, types, control flow, builtins, standard library, and error handling -- a single document that makes the language immediately usable by any AI or human reader.
todos:
  - id: write-reference
    content: "Write REFERENCE.md with all sections: quick start, syntax, types, variables, functions, control flow, expressions, strings, patterns, modules, error handling, builtins reference, standard library"
    status: completed
  - id: planning-update-v35
    content: Update PLANNING.md with v0.35 milestone
    status: completed
isProject: false
---

# v0.35 -- Language Reference

A single [REFERENCE.md](REFERENCE.md) file at the repo root: the authoritative, complete guide to the "a" language. Written so that any AI instance (or human) can read it and immediately write correct "a" code without spelunking source files.

## Why this matters

The language has 100+ builtins, 6 standard library modules, raw strings, heredocs, regex, eval, string interpolation, sum types, pattern matching, effects, contracts, pipe operators, and a self-hosted compiler. All of this knowledge currently lives only in Rust source code, scattered test files, and PLANNING.md milestone notes. A reference document is what turns "a" from a language that was *built* into one that can be *used*.

## Document structure

### 1. Quick Start
- Minimal hello world
- How to run: `a run file.a`, `a test file.a`
- CLI subcommands: `run`, `test`, `check`, `fmt`, `ast`, `compile`, `interp`

### 2. Syntax Basics
- Comments: `; line comment`
- Newline significance (statement separator)
- Top-level forms: `fn`, `ty`, `mod`, `use`

### 3. Types
- Primitives: `i8`..`i64`, `u8`..`u64`, `f32`, `f64`, `bool`, `str`, `bytes`, `void`
- Arrays: `[T]`, literals `[1, 2, 3]`, spread `[...a, 4]`
- Maps: `#{K: V}`, literals `#{"key": val}`
- Tuples: `(T, U)`
- Function types: `fn(T) -> U`
- Named/generic types: `Name<T>`
- Type declarations: records `ty Point = {x: f64, y: f64}`, sum types `ty Option = Some(T) | None`, aliases with `where`

### 4. Variables and Bindings
- `let name = expr`, `let name: Type = expr`
- `let mut name = expr`
- Destructuring: `let [a, _, ...rest] = arr`

### 5. Functions
- Declaration: `fn name(params) -> RetType { body }`
- Lambdas: `fn(x) => x * 2`, `fn(x) { ret x * 2 }`
- Effects: `effects [io]`
- Contracts: `pre { expr }`, `post { expr }`
- Inferred return types

### 6. Control Flow
- `if` / `else` / `else if`
- `match expr { pattern => body, ... }` with guards
- `for x in collection { }`, `for [k, v] in entries { }`
- `while expr { }`, `break`, `continue`
- `ret expr`

### 7. Expressions and Operators
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Logical: `&&`, `||`, `!`
- Pipe: `|>` (including newline continuation)
- Arrows: `->` (return type), `=>` (match/lambda)
- Try: `try expr`, `expr?`
- Field access: `expr.field`, index: `expr[i]`, call: `expr(args)`

### 8. Strings
- Regular: `"hello"`, escapes `\n \t \r \\ \" \{ \}`
- Interpolation: `"hello {name}"`
- Raw: `r"no \n escape"`, `r#"can contain "quotes""#`
- Heredocs: triple-backtick blocks

### 9. Pattern Matching
- Wildcards `_`, literals, identifiers
- Array patterns `[a, b, ...rest]`
- Map patterns `#{"key": val}`
- Constructor patterns `Some(x)`
- Guards: `pattern if condition => ...`

### 10. Modules and Imports
- `mod name { ... }`
- `use std.testing`, `use std.math`
- Dotted access: `std.math.range(1, 10)`

### 11. Error Handling
- `Ok(val)`, `Err(msg)`
- `try expr`, `expr?`
- `unwrap`, `unwrap_or`, `is_ok`, `is_err`, `expect`

### 12. Builtin Functions Reference
Organized by category with signature, description, and example for each:
- **I/O**: `print`, `println`, `eprintln`, `io.write`, `io.read_file`, `io.write_file`, `io.read_line`, `io.read_stdin`
- **Conversion**: `to_str`, `type_of`, `int`, `float`
- **Math**: `len`, `sqrt`, `abs`, `floor`, `ceil`, `round`
- **Arrays**: `push`, `slice`, `sort`, `reverse_arr`, `contains`, `concat_arr`, `map`, `filter`, `reduce`, `each`, `sort_by`, `find`, `any`, `all`, `flat_map`, `min_by`, `max_by`, `zip`, `enumerate`, `take`, `drop`, `chunk`, `unique`
- **Maps**: `map.get`, `map.set`, `map.keys`, `map.values`, `map.has`, `map.delete`, `map.merge`, `map.entries`, `map.from_entries`
- **Strings**: `str.concat`, `str.split`, `str.contains`, `str.starts_with`, `str.ends_with`, `str.replace`, `str.trim`, `str.upper`, `str.lower`, `str.join`, `str.chars`, `str.slice`, `str.lines`, `str.find`, `str.count`
- **Regex**: `regex.is_match`, `regex.find`, `regex.find_all`, `regex.replace`, `regex.replace_all`, `regex.split`, `regex.captures`
- **JSON**: `json.parse`, `json.stringify`, `json.pretty`
- **HTTP**: `http.get`, `http.post`, `http.put`, `http.patch`, `http.delete`
- **Filesystem**: `fs.exists`, `fs.is_dir`, `fs.is_file`, `fs.ls`, `fs.mkdir`, `fs.rm`, `fs.mv`, `fs.cp`, `fs.cwd`, `fs.abs`, `fs.glob`
- **Environment**: `env.get`, `env.set`, `env.all`
- **Shell**: `exec`
- **Characters**: `char_code`, `from_code`, `is_alpha`, `is_digit`, `is_alnum`
- **Runtime**: `eval`, `args`, `exit`, `fail`

### 13. Standard Library
Brief reference for each `std/` module:
- `std.math`: `max`, `min`, `clamp`, `pow`, `sum`, `range`
- `std.strings`: `repeat`, `pad_left`, `pad_right`, `reverse`, `center`, `is_empty`
- `std.cli`: ANSI colors and formatting
- `std.testing`: `assert_eq`, `assert_true`, `assert_false`, `assert_contains`, `assert_not_eq`
- `std.re`: Pure-"a" regex engine (pattern matching, search, replace, split)

## Scale
- `REFERENCE.md`: ~600-800 lines
- `PLANNING.md`: ~15 lines for v0.35 entry
- Total: one new file + minor update to one existing file
