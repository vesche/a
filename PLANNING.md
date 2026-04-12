# "a" -- A Programming Language by AI, for AI

## Design Philosophy

Every design decision in "a" optimizes for what matters to language-model-based agents:
unambiguous parsing, explicit semantics, token efficiency, local reasoning, and verifiability.
Human readability is a non-goal. Machine clarity is the goal.

## Core Principles

1. **Unambiguous grammar** -- Context-free, LL(1). No operator overloading. Explicit delimiters.
2. **Explicit everything** -- Full type annotations, effect declarations, error paths. No inference needed.
3. **Token-efficient** -- 2-3 char keywords (`fn`, `ty`, `let`, `mod`) that are each 1 BPE token.
4. **Local reasoning** -- Function signatures tell the complete story (types + effects + contracts).
5. **Built-in verification** -- Pre/post conditions and refinement types as first-class syntax.
6. **Machine-readable tooling** -- All errors output as structured JSON. AST is serializable.

## Implementation

Reference implementation in Rust. Chose Rust because:

- Exhaustive pattern matching catches missed AST variants at compile time
- Algebraic data types are the natural representation for syntax trees
- Single compiled binary with zero runtime dependencies

## Status

### v0.1 -- Core Language

- Formal EBNF grammar
- Lexer with structured JSON errors
- Recursive descent LL(1) parser
- AST with serde JSON serialization
- Type checker with effect system
- Tree-walking interpreter
- Canonical formatter
- CLI: `a run`, `a check`, `a fmt`, `a ast`

### v0.2 -- Expressiveness

- Closures / lambdas as first-class values: `fn(x: i64) => x * 2`
- Higher-order builtins: `map`, `filter`, `reduce`, `each`
- Pipe operator: `data |> filter(pred) |> map(transform) |> reduce(0, add)`
- Multi-line pipe chains (newline before `|>`)
- String interpolation: `"hello {name}, you are {to_str(age)} years old"`
- Map data structure: `#{str: i64}` type, `#{"key": val}` literals
- Map builtins: `map.get`, `map.set`, `map.keys`, `map.values`, `map.has`
- 48 integration tests (all passing)

### v0.3 -- Practical Power

- Shell execution: `exec(cmd)` → `{stdout, stderr, code}` record
- JSON round-tripping: `json.parse`, `json.stringify`, `json.pretty`
- String operations: `str.contains`, `str.starts_with`, `str.ends_with`, `str.replace`, `str.trim`, `str.upper`, `str.lower`, `str.join`, `str.chars`, `str.slice`, `str.lines`
- Environment variables: `env.get`, `env.set`, `env.all`
- Type introspection: `type_of(val)` → string
- Type conversion: `int(val)`, `float(val)`
- Parser: `str.` namespace in expression context (type keyword as builtin prefix)
- 68 integration tests (all passing)

### v0.4 -- World Interaction

- HTTP client via `ureq`: `http.get`, `http.post`, `http.put`, `http.patch`, `http.delete`
  - Returns `{status, body, headers}` record
  - Custom headers via map argument
  - Auto JSON content-type for non-string bodies
- Filesystem: `fs.exists`, `fs.is_dir`, `fs.is_file`, `fs.ls`, `fs.mkdir`, `fs.rm`, `fs.mv`, `fs.cp`, `fs.cwd`, `fs.abs`, `fs.glob`
  - `fs.ls` returns `[{name, is_dir}]` records
  - `fs.mkdir` creates recursive
  - `fs.rm` handles files and directories
  - `fs.glob` with simple pattern matching
- 79 integration tests (all passing)

### v0.5 -- Resilient Error Handling

- `try { block }` expression: catches runtime errors, returns `Result`
- `?` postfix operator: unwraps `Ok`, throws on `Err` (catchable by `try`)
- `Ok(val)` / `Err(val)` constructors for explicit Result creation
- `match` patterns for `Ok(v)` / `Err(e)` destructuring (already supported)
- Result builtins: `unwrap`, `unwrap_or`, `is_ok`, `is_err`, `expect`
- Try blocks capture last expression value (implicit return)
- 89 integration tests (all passing)

### v0.6 -- Documentation

- REFERENCE.md: complete language reference (syntax, types, all 60+ builtins, patterns)
- README.md: rewritten to reflect v0.5 capabilities
- grammar.ebnf: updated with pipe, lambda, map, try block, ?, interpolation syntax
- Language is now self-documenting for AI consumption

### v0.7 -- Self-Hosting Primitives & Bootstrapping Proof

- `while cond { body }` loop
- `break` and `continue` for both `for` and `while` loops
- Variant constructors: `ty Token = Num(i64) | Word(str)` values creatable via `Num(42)`, `Word("x")`
- Character builtins: `char_code(s)`, `from_code(n)`, `is_alpha(s)`, `is_digit(s)`, `is_alnum(s)`
- Array builtins: `push(arr, val)` (returns new array), `slice(arr, start, end)`
- Recursive type compatibility in checker (e.g. `[str] = []` now valid)
- **Self-lexer**: `examples/self_lexer.a` -- a complete lexer for the "a" language, written in "a"
  - Reads its own source from disk and tokenizes it (2,552 tokens)
  - Handles all token kinds: keywords, identifiers, numbers, strings, operators, delimiters
  - Correctly tracks line/column positions
- **Self-parser**: `examples/self_parser.a` -- a recursive descent parser for "a", written in "a"
  - Includes integrated lexer (flat token array: kind/text pairs)
  - Full expression parser with correct operator precedence (6 levels: ||, &&, ==, <>, +-, */%)
  - Statement parser: let, ret, if/else, while, for, break, continue, assignment, expression
  - Top-level parser: fn declarations (with params, return types, effects, pre/post blocks), ty declarations
  - Functional state-passing design: every parse function takes (tokens, pos) and returns [ast, new_pos]
  - Produces S-expression AST output
  - Successfully parses hello.a, math.a, and **its own source** (7,385 tokens -> 788 AST lines)
  - Bootstrapping proof: "a" can lex AND parse itself
- 107 integration tests (all passing)

### v0.8 -- Import System & Standard Library

- `use path.to.module` import system
  - Resolves `use std.math` to `std/math.a` (searches source dir, then cwd)
  - Functions registered as `namespace.function_name` (e.g. `math.max`)
  - Leverages existing `module.fn()` call resolution (zero changes to Call handler)
  - Internal calls between module functions work (bare names registered for intra-module use)
  - Circular import protection via `loaded_modules` tracking
  - Transitive imports: imported modules can import other modules
- Standard library (`std/`):
  - `std/math.a`: max, min, clamp, fmax, fmin, pow, sum, range
  - `std/strings.a`: repeat, pad_left, pad_right, reverse, count, center, is_empty
  - `std/testing.a`: assert_eq, assert_int_eq, assert_true, assert_false, report
  - `std/lexer.a`: complete "a" lexer as a reusable library module
- Modular self-parser: `examples/parse_file.a` imports `std/lexer.a` instead of duplicating it
- `examples/import_demo.a`: demonstrates all three std modules with integrated tests
- 111 integration tests (all passing, 4 new import tests)

### v0.9 -- Real-World Tooling

- Array builtins: `sort(arr)`, `reverse_arr(arr)`, `contains(arr, val)`
- `PartialEq` for `Value` (enables equality comparison in array `contains`)
- `examples/project_stats.a`: a real codebase analyzer written in "a"
  - Recursively walks directories, finds `.a` files
  - Counts lines, functions, types, imports, comments, blank lines per file
  - Computes code composition percentages with ASCII bar chart
  - Formatted per-file report with padded columns
  - Uses `std.math`, `std.strings`, `sort`, `push`, `try/is_ok/unwrap`, `fs.ls`, `io.read_file`
  - Runs in <600ms, analyzes 17 files / 2,774 lines
- README.md rewritten to properly represent the language's current capabilities
- REFERENCE.md updated with new array builtins
- 113 integration tests

### v0.10 -- Ergonomics

- **Type inference**: `let` and `for` no longer require type annotations
  - `let x = 5` infers `i64`, `let name = "hi"` infers `str`
  - `for item in arr` infers element type from array expression
  - Explicit annotations still work and are checked: `let x: i64 = 5`
  - Parser emits `TypeExpr::Infer`, checker resolves from value/iterator type
  - Formatter preserves: inferred types print without annotation, explicit types print with
- **Multi-line expressions**: newlines allowed inside `[]`, `()`, and `#{}`
  - Arrays: `[\n  1,\n  2,\n  3\n]`
  - Function calls: `f(\n  arg1,\n  arg2\n)`
  - Parenthesized expressions: `(\n  complex_expr\n)`
  - Backward compatible: single-line still works identically
- 123 integration tests (7 new: 5 inference + 2 multi-line)

### v0.11 -- CLI Scripting

- **Process arguments**: `args()` builtin returns `[str]` of arguments passed after the script file
  - `a run script.a arg1 arg2` -> `args()` returns `["arg1", "arg2"]`
  - Uses clap `trailing_var_arg` for natural argument passing
- `**exit(code)`**: exits the process with the given status code
- `**str.find(s, sub)**`: returns byte index of first occurrence, or -1 if not found
- `**str.count(s, sub)**`: returns number of non-overlapping occurrences
- `examples/search.a`: a grep-like file search tool written in "a"
  - Takes pattern, optional directory, optional extension filter from command line
  - Recursively walks directories (skipping hidden dirs)
  - Searches each file for pattern, prints file:line matches
  - Reports total matches, files matched, files searched
  - Runs in ~600ms across the full codebase
- 127 integration tests

### v0.12 -- Terminal UI & Developer Tools

- `std/cli.a`: terminal color and style library
  - `red`, `green`, `yellow`, `blue`, `magenta`, `cyan`, `gray` color functions
  - `bold`, `dim`, `underline` style functions
  - All composable: `bold(red("text"))` nests correctly
  - Built on `from_code(27)` -- ANSI escapes constructed in pure "a"
- `examples/search.a` upgraded with colored output:
  - Green filenames, yellow line numbers, dim separators
  - Bold red highlighting of matched text within lines
  - Bold summary line
- `examples/todo_scan.a`: annotation scanner
  - Finds TODO, FIXME, HACK, NOTE across a codebase
  - Color-coded by severity (red=FIXME, magenta=HACK, yellow=TODO, blue=NOTE)
  - Summary counts by category
  - 5 standard library modules, 21 source files, 3,000+ lines of "a"

### v0.13 -- Unix Pipeline Support

- `io.read_stdin()`: read all of stdin as a string
- `io.read_line()`: read one line from stdin (returns Err on EOF)
- `eprintln(s)`: print to stderr (essential for pipeline tools)
- `examples/lines.a`: universal line processor (pipeline citizen)
  - 15 flags: --filter, --reject, --trim, --upper, --lower, --number,
  --count, --head, --tail, --skip, --col, --unique, --sort, --reverse, --nonempty
  - Replaces grep | awk | sort | uniq | head | tail | cat -n in a single tool
  - Reads from stdin, writes to stdout, errors to stderr
- 130 integration tests

### v0.14 -- Self-Interpreter (Bootstrap Stage 3)

- `examples/eval.a`: 843-line self-interpreter for "a", written in "a"
  - Structured parser: tokens -> array-based AST nodes (heterogeneous arrays)
  - Tree-walking evaluator: walks AST, maintains environment as flat array
  - Supports: functions, recursion, if/else, while, for, let/assign, break/continue
  - Supports: arithmetic, comparisons, boolean logic, unary ops
  - Supports: arrays, indexing, string/int/float literals
  - Delegates builtins to host runtime (println, sort, str.*, len, push, etc.)
  - Successfully interprets fibonacci.a, math.a, hello.a through itself
  - Bootstrap chain: source -> std/lexer.a -> eval.a parser -> eval.a evaluator
- Type checker changed to warn-only mode for `run` command (needed for heterogeneous arrays)
- 22 source files, 4,193 lines of "a", 179 functions

### v0.15 -- Bytecode VM

- `src/bytecode.rs`: instruction set (30 opcodes)
  - Stack operations: Const, Pop, Dup
  - Locals/globals: GetLocal, SetLocal, GetGlobal, SetGlobal
  - Arithmetic: Add, Sub, Mul, Div, Mod, Neg
  - Comparison: Eq, NotEq, Lt, Gt, LtEq, GtEq
  - Logic: Not, JumpIfFalseKeep (short-circuit and/or)
  - Control: Jump, JumpIfFalse, Loop, Return
  - Functions: Call, CallMethod, CallClosure
  - Data: Array, Index, IndexSet, Field, Record, MapNew, Concat
  - Result: MakeOk, MakeErr, Unwrap, UnwrapOr, IsOk, IsErr, TryStart, TryEnd
  - Match: MatchVariant, MatchLiteral
  - Closures: MakeClosure (lambdas compiled to named functions)
- `src/compiler.rs`: AST-to-bytecode compiler (full language coverage)
  - Compiles: functions, let/assign, return, if/else if/else, while, for
  - Compiles: all expressions (literals, binops, unary, calls, method calls)
  - Compiles: arrays, indexing, index assignment, field access, string interpolation, pipes
  - Compiles: match statements & expressions (literal, constructor, wildcard patterns)
  - Compiles: closures/lambdas (hoisted to named functions, called via string reference)
  - Compiles: try blocks, ? operator, Ok/Err constructors, unwrap/is_ok/is_err
  - Compiles: map literals `#{k: v}`, record literals, block & if expressions
  - Compiles: break/continue with proper loop context tracking
  - Import system: `use std.X` loads and compiles library functions
  - Short-circuit `and`/`or` via conditional jumps
  - Local variable resolution via flat index (no hash lookups at runtime)
- `src/vm.rs`: stack-based bytecode executor
  - Value stack + call frame stack + try handler stack
  - Higher-order functions (map, filter, reduce, each) executed natively in VM
  - Delegates 75+ builtins to existing `builtins.rs` (zero reimplementation)
  - Tight execute loop with O(1) dispatch per instruction
- `**a run` now uses bytecode VM by default**
- `a interp` available as legacy tree-walker fallback
- **Benchmark: fib(30) 2.78s tree-walker → 0.33s VM (8.4x faster)**
- All 6 example programs run correctly through the VM
- 131 tests pass (1 HTTP test fails only due to sandbox)

### v0.16 -- Regex Engine & VM Fixes

- **Bug fix: `continue` in `for` loops** -- was jumping to loop start (skipping index increment), causing infinite loops. Fixed with forward-patching: continue now jumps to the increment section.
- **Bug fix: array index assignment** -- `arr[i] = val` was not storing the modified array back to the variable. Fixed compiler to emit SetLocal after IndexSet.
- `std/re.a`: regex engine written in pure "a" (~430 lines)
  - Recursive backtracking matcher with greedy quantifiers
  - Pattern syntax: `.` `*` `+` `?` `[a-z]` `[^0-9]` `\d` `\w` `\s` `|` `^` `$`
  - API: `match_full`, `test`, `search`, `find_all`, `replace`, `replace_first`, `split`
  - Handles character classes with ranges, negated classes, escape sequences
  - Alternation via `|` with branch splitting
- `examples/search.a` upgraded to regex: uses `re.test` for matching, `re.search` for highlight positioning
- 6 standard library modules, 23+ source files

### v0.17 -- Code Intelligence Tool

- `examples/inspect.a`: static analysis tool for "a" source files (~310 lines)
  - Token-level analysis using `std.lexer` -- no parser required
  - Extracts all function declarations with signatures
  - Builds call graph: which function calls which (method calls via `.` included)
  - Dead code detection: flags functions never called by any other function
  - Complexity metrics: branches (if/match), loops (for/while), returns per function
  - Correctly handles `pre`/`post`/`where` blocks (skips them for body analysis)
  - Import extraction from `use` declarations
  - Two output modes:
    - Colored terminal (default): bold names, dim signatures, green/yellow/red complexity tags, red UNUSED badge
    - JSON (`--json`): machine-readable structured output for AI agent consumption
  - Directory scanning: recursively finds all `.a` files
  - Summary stats: total files, lines, functions, unused count
  - Uses: `std.lexer`, `std.strings`, `std.cli` (4 stdlib modules in one tool)

### v0.18 -- Test Framework

- `fail(msg)` builtin: throws a catchable RuntimeError (works in both VM and tree-walker)
- `VM::run_function(name)`: call any compiled function by name, with state reset between calls
- `VM::set_test_mode(true)`: makes `exit()` throw RuntimeError instead of killing the process
- `a test <file>` subcommand:
  - Auto-discovers all `fn test_*()` functions via AST walking
  - Runs each test in isolation (fresh VM state per test)
  - Catches assertion failures and runtime errors
  - Reports pass/fail with green checkmarks / red X marks and timing
  - `--filter <substr>` to run only matching tests
  - Non-zero exit code on any failure
- `std/testing.a` rewritten: assertions now call `fail()` instead of returning bool
  - `assert_eq(actual, expected)`, `assert_int_eq`, `assert_true`, `assert_false`
  - `assert_contains(haystack, needle)`, `assert_not_eq`
- Test suites:
  - `tests/test_core.a`: 8 tests covering arithmetic, strings, booleans, comparisons, arrays, interpolation, type conversion, conditionals
  - `tests/test_strings.a`: 14 tests covering `std.strings` functions and `str.`* builtins
  - `tests/test_re.a`: 17 tests covering the regex engine (full match, partial, search, find_all, replace, split, classes, alternation, anchors)
- Total: 39 "a"-native tests across 3 suites

### v0.19 -- Bootstrap (Self-Hosting)

- **eval.a expanded** (1,036 lines):
  - String interpolation: `interp_str` scans for `{expr}` in strings, evaluates variable lookups, index access, and single-argument function calls
  - `use` / import system: `load_module` reads, lexes, parses `.a` files and adds namespaced functions to the function table
  - Index assignment: `arr[i] = val` via `["idx_assign", ...]` AST node
  - New builtins delegated to host: `io.read_file`, `args()`, `exit()`, `fail()`, `is_alpha`, `is_digit`, `is_alnum`, `char_code`, `from_code`
  - Host-delegated lexer: `lexer.lex`, `lexer.tk`, `lexer.tv`, `lexer.tcount`, `lexer.skip_nl` intercepted in `eval_call` for performance
- **boot.a** (686 lines): compact self-interpreter optimized for bootstrap
  - Full parser: precedence climbing for all operators, postfix chains (calls, index, dot-method), all statements
  - Full evaluator: let, assign, index-assign, if/else, while, for, break, continue, return
  - Method call support: `mod.fn(args)` parsed as `["mc", mod, fn, args]`
  - Args propagation: inner interpreter receives remaining args via `__args_`_ env variable
  - Type-safe sentinel checks: `type_of(r) == "str"` guards before comparing to `"__brk__"`/`"__cnt__"`
- **BOOTSTRAP PROOF: boot.a interpreting boot.a interpreting fibonacci.a**
  - Three layers of "a": Rust VM → boot.a → boot.a → fibonacci.a
  - Correct output through all three layers
  - 5m 45s on release build (performance-limited by O(n) env lookups at double-interpretation depth)

### v0.20 -- VM Performance Overhaul

- **Zero-copy opcode dispatch**: `Op` enum is `Copy`, string-bearing opcodes use interned `usize` indices
- **String interning**: `Chunk`-local `strings: Vec<String>` table, referenced by index in opcodes
- **Mutable builtin operations**: 25+ common builtins intercepted directly in VM, avoiding generic call overhead
- **Function dispatch table**: `HashMap<String, usize>` for O(1) function lookup
- **Rc-wrapped values**: `Value::String(Rc<String>)` and `Value::Array(Rc<Vec<Value>>)` for O(1) cloning
- **Performance results**: 345x bootstrap speedup, 40x regex, 96x self-parser
- 131 integration tests (all passing)

### v0.21 -- VM Closure Captures

- **Flat closure model**: lambdas capture enclosing variables at creation time (value semantics)
- **Capture analysis**: `collect_free_vars()` AST walker identifies free variables in lambda bodies
- `**VMClosure` Value variant**: `VMClosure { name: Rc<String>, captures: Rc<Vec<Value>> }` -- lightweight, cloneable
- `**MakeClosure(name_idx, n_captures)`**: pops captured values from stack, builds `VMClosure`
- `**CallClosure` / `call_value**`: pushes captures as first locals before explicit arguments
- **Local variable call dispatch**: calls to local variables (e.g., `f(5)` where `f` is a closure) now correctly emit `CallClosure` instead of `Call`
- Closures work in pipes, HOFs (`map`/`filter`/`reduce`), direct calls, and nested scopes
- 143 integration tests (12 new VM closure tests)

### v0.22 -- Functional Toolkit

- **Higher-order functions** (all VM-optimized with closure support):
  - `sort_by(arr, cmp)`: sort with comparator closure returning negative/zero/positive
  - `find(arr, pred)`: first matching element → `Ok(val)` / `Err("not found")`
  - `any(arr, pred)` / `all(arr, pred)`: boolean quantifiers with short-circuit
  - `flat_map(arr, fn)`: map then flatten one level
  - `min_by(arr, fn)` / `max_by(arr, fn)`: extremes by key function → `Result`
- **Data operations** (all VM-optimized):
  - `zip(a, b)`: combine two arrays pairwise into `[[a0, b0], [a1, b1], ...]`
  - `enumerate(arr)`: pairs of `[[0, a], [1, b], [2, c]]`
  - `take(arr, n)` / `drop(arr, n)`: first/remaining n elements
  - `chunk(arr, n)`: split into sub-arrays of size n
  - `unique(arr)`: deduplicate preserving order
- All 13 new builtins work with closures, captures, and pipe chains
- `tests/test_functional.a`: 26 native test functions covering all new operations
- 170 Rust integration tests + 65 native "a" tests (all passing)

### v0.23 -- Tail Call Optimization

- **Peephole optimization in compiler**: `optimize_tail_calls` post-pass detects `Call+Return` / `CallClosure+Return` pairs and fuses them into `TailCall` / `TailCallClosure` opcodes
- **VM frame rewrite**: `TailCall` rewrites the current call frame in-place (copies args to `stack_base`, resets `ip` to 0, updates `fn_idx`) instead of pushing a new frame
- `**TailCallClosure`**: handles `VMClosure` captures (prepends captures before args) with same frame rewrite
- **Builtin fallback**: tail calls to builtins/HOFs fall back to normal call+return semantics
- Enables **unbounded recursion in constant stack space**: `countdown(1000000)` runs with 1 frame
- Handles **mutual recursion**: `is_even`/`is_odd` with 100k depth, no overflow
- **Tail-recursive fibonacci**: `fib_tail(n, a, b)` in O(n) time, O(1) stack
- **Math builtins**: `floor(x)`, `ceil(x)`, `round(x)` -- convert float to nearest integer
- `tests/test_tco.a`: 7 native tests (deep countdown, accumulator, mutual recursion, tail fibonacci)
- 181 Rust integration tests + 72 native "a" tests (all passing)

### v0.24 -- Destructuring & Spread

- **Array destructuring in `let`**: `let [a, b, c] = expr` -- fixed-length binding from array elements
- **Rest pattern**: `let [first, ...rest] = expr` -- bind remaining elements to a new array
- **Wildcard in destructuring**: `let [_, second, _] = expr` -- skip positions with `_`
- **Array destructuring in `for`**: `for [key, val] in enumerate(arr)` / `for [a, b] in zip(xs, ys)`
- **Spread in array literals**: `[...arr1, x, ...arr2]` -- inline-expand arrays during construction
- **Mixed spread**: `[1, ...middle, 99]` -- interleave individual elements and spreads
- `...` token added to lexer, `Spread` expression + `LetDestructure`/`ForDestructure` statement AST nodes
- Compiler emits `Index`+`SetLocal` for bindings, `drop()` call for rest, `push`/`concat_arr` for spreads
- Full support in VM (bytecode), interpreter (tree-walker), checker, and formatter
- `tests/test_destructure.a`: 18 native tests covering all forms
- 195 Rust integration tests + 90 native "a" tests (all passing)

### v0.25 -- Source-Mapped Stack Traces

- **Line table in bytecode**: `Chunk` stores `lines: Vec<u32>` parallel to `code`, one entry per opcode tracking the source line that generated it
- **Compiler line propagation**: `compile_stmt()` and `compile_expr()` call `chunk.set_line(span.line)` from AST `Span` before emitting opcodes
- **StackFrame struct**: `errors::StackFrame { function: String, line: u32 }` for structured stack trace entries
- **Stack trace in AError**: `AError` gains `stack: Vec<StackFrame>` field, serialized to JSON (skipped when empty)
- **VM stack trace capture**: `capture_stack_trace()` walks the call frame stack on error, maps each frame to function name + source line via the line table
- **Automatic attachment**: all runtime errors get stack traces via the `execute()` error wrapper -- zero changes needed at individual error sites
- **Span from trace**: error `span` is automatically set from the innermost stack frame's line number
- **Tail-call aware**: TCO-elided frames correctly absent from traces; non-TCO frames all present
- **Human-readable CLI output**: `RuntimeError: msg` + indented `at function (line N)` for each frame, printed to stderr before JSON
- **Test runner integration**: failing tests now show inline stack traces with function names and line numbers
- `tests/test_stack_traces.a`: 4 native tests for error-free execution with traced call chains
- 203 Rust integration tests + 94 native "a" tests (all passing)

### v0.26 -- First-Class HashMap Maps

- **HashMap-backed maps**: `Value::Map(Rc<HashMap<String, Value>>)` replaces O(n) association lists with O(1) hash lookups
- **String keys**: map keys are strings (auto-converted from other types), enabling standard `HashMap` with no custom Hash impl
- **Bracket indexing (read)**: `m["key"]` works in both VM (`Op::Index`) and interpreter -- returns `Void` for missing keys
- **Bracket indexing (write)**: `m["key"] = val` works in both VM (`Op::IndexSet`) and interpreter via COW (`Rc::try_unwrap`)
- **Field access on maps**: `m.field_name` syntax works as sugar for `m["field_name"]` in both VM and interpreter
- **Map iteration**: `for [k, v] in m` and `for item in m` work via positional indexing yielding `[key, value]` pairs
- **ForDestructure on maps**: `for [k, v] in map` works with the v0.24 destructuring syntax
- **New builtins**: `map.delete(m, key)`, `map.merge(m1, m2)`, `map.entries(m)`, `map.from_entries(arr)`
- **Existing builtins updated**: `map.get`, `map.set`, `map.keys`, `map.values`, `map.has` all use HashMap O(1) operations
- **JSON roundtrip**: `json.parse` produces HashMap maps, `json.stringify`/`value_to_json` serializes them correctly
- **HTTP headers**: response headers and request headers use HashMap maps
- **Type checker**: `Index` on `Type::Map(K, V)` now returns `V`
- `tests/test_maps.a`: 21 native tests covering all map operations
- 229 Rust integration tests + 115 native "a" tests (all passing)

### v0.27 -- Pattern Matching Overhaul

- **MatchLiteral bug fix**: changed from `MatchLiteral(usize)` to `MatchLiteral(usize, usize)` with proper jump-on-mismatch in the VM
- **Array patterns**: `[a, b, c]` fixed-length, `[head, ...tail]` rest element, `[]` empty, `[1, x]` literal elements
- **Map patterns**: `#{"key": val}` key-binding, `#{"status": 200}` literal value matching, `#{"a": x, "b": y}` multi-key
- **Guard clauses**: `pat if cond => body` -- guard expression evaluated after pattern binds, arm skipped if falsy
- **Nested patterns**: `#{"pos": [x, y]}` map-containing-array, `#{"type": "click", "data": d}` literal+binding mix
- **Full recursive nesting**: array-in-map, map-in-array, literals nested inside structural patterns at any depth
- **Interpreter support**: `match_pattern` extended for `Array`/`Map` patterns; guard evaluation in both stmt and expr match
- **Compiler support**: inline type/length/key checks via `type_of`, `len`, `map.has` calls; temp-local approach for nested structural patterns avoiding stack depth issues
- **Checker/Formatter**: `bind_pattern` handles new pattern types; `format_pattern` renders `[a, ...r]` and `#{"k": p}` syntax; guard expressions formatted as `pat if cond =>`
- **Free-var analysis**: closure capture analysis updated to track pattern bindings and guard expressions
- `tests/test_patterns.a`: 22 native tests covering all new pattern features
- 259 Rust integration tests + 137 native "a" tests (all passing)

### v0.28 -- Self-Hosted Compiler Frontend

- **Self-hosted lexer** (`std/compiler/lexer.a`): full-parity reimplementation of `src/lexer.rs` in "a"
  - All token types: keywords, identifiers, numbers, strings, operators, delimiters
  - Underscore token (`_`), DotDotDot spread (`...`), string interpolation (`InterpStart`, `InterpMid`, `InterpEnd`)
  - Flat token array output: `[kind1, val1, kind2, val2, ...]` for efficient O(1) indexing
  - Helper functions: `tk(toks, i)`, `tv(toks, i)`, `tcount(toks)`, `skip_nl(toks, i)`
- **Tagged-map AST** (`std/compiler/ast.a`): constructor functions for all AST node types
  - Every node is a HashMap with a `"tag"` field: `#{"tag": "FnDecl", "name": ..., "params": ..., ...}`
  - Leverages First-Class HashMaps (v0.26) and Pattern Matching (v0.27)
  - 40+ constructors covering: Program, FnDecl, Param, TypeDecl, TypeSum, TypeRecord, Variant, UseDecl, ModDecl
  - Statements: Block, Let, Assign, Return, If, Match, MatchArm, For, ForDestructure, LetDestructure, While, Break, Continue, ExprStmt
  - Expressions: Int, Float, Str, Bool, Void, Ident, BinOp, UnaryOp, Call, FieldAccess, Index, Lambda, Array, Spread, MapLiteral, Pipe, Try, Interpolation
  - Patterns: PatWildcard, PatIdent, PatLiteral, PatConstructor, PatArray, PatRest, PatMap
  - Types: TyPrim, TyArray, TyMap, TyFn, TyNamed, TyInfer, TyTuple
- **Self-hosted recursive-descent parser** (`std/compiler/parser.a`): 1,500+ line parser
  - Full recursive-descent parser mirroring `src/parser.rs`
  - Precedence climbing for expressions (7 levels: pipe, or, and, equality, comparison, additive, multiplicative)
  - Postfix chain parsing: function calls, field access, index, method calls
  - All statement types: let (typed/untyped/mutable), assign, return, if/else, match with guards, for/while, break/continue, destructuring
  - All top-level declarations: fn (with optional types, effects, pre/post), ty (sum/record/alias), mod, use
  - Pattern parsing: wildcard, identifier, literal, constructor, array (with rest), map (with key binding)
  - Lambda expressions: `fn(params) => expr` and `fn(params) { block }`
  - String interpolation: multi-part `InterpStart` / `InterpMid` / `InterpEnd` assembly
- **Rust parser extended**: function parameters and return types now optional (defaults to `TypeExpr::Infer`)
  - Enables the self-hosted `.a` modules to be compiled without explicit type annotations
- **Module error reporting**: `load_module` now prints parse/lex errors to stderr instead of silently returning
- **Test suite**: `tests/test_self_parse.a` -- 30 tests verifying AST structure for all language constructs
  - Integer/float literals, binary operators with precedence, function calls, if/else, match with guards
  - For/while loops, lambdas, array/map literals, field access, indexing, unary ops
  - Use declarations, typed parameters, effects, let mut, assignment, break/continue
  - Array/map/constructor patterns, pipes, spread, string interpolation, type declarations
  - Parses real source files: `examples/math.a` and `std/compiler/lexer.a`
- `**examples/dump_ast.a`**: CLI tool that reads any `.a` file, parses it with the self-hosted parser, and dumps the AST as pretty-printed JSON
  - Usage: `a run examples/dump_ast.a -- <file.a>`
  - Produces identical structure to the Rust `a ast` command
- **Bootstrap milestone**: "a" can now lex AND parse its own compiler frontend source files using code written entirely in "a"

### v0.29 -- Self-Hosted Compiler Backend

- **Self-hosted bytecode compiler** (`std/compiler/compiler.a`): 2,682-line direct port of `src/compiler.rs` into "a"
  - Compiles tagged-map ASTs (from `std/compiler/parser.a`) into bytecode represented as "a" maps/arrays
  - State model: mutable `s` (per-function: locals, code, constants, strings, loops) and `g` (global: pending functions, lambda counter) HashMaps threaded through all functions
  - Chunk helpers: `emit`, `intern_str`, `add_const`, `patch_jump`, `resolve_local`, `push_local`, loop context management
  - Full statement compilation: Let, Assign, Return, ExprStmt, If/ElseIf/Else, While, For, ForDestructure, LetDestructure, Break, Continue
  - Full expression compilation: Literal, Ident, BinOp, UnaryOp, Call, FieldAccess, Index, Array, Spread, Interpolation, MapLiteral, Record, TryBlock, Try, Pipe, Lambda, If-expr, Block-expr
  - Match compilation: `compile_match_stmt`/`_expr` with literal, constructor, array, map, wildcard, and ident patterns; guard clauses; nested structural patterns
  - Closure capture analysis: `collect_free_vars` with `walk_expr_free`/`walk_stmt_free`/`walk_block_free` identifying free variables across scopes
  - Tail call optimization: peephole pass converting `Call+Return` into `TailCall+Pop`
  - Module system: `collect_top_level`, `load_module_fns`, pending function drain loop
  - Output format: `#{"functions": [...], "main_idx": N}` where each function contains `name`, `arity`, `chunk` with `code`, `constants`, `strings`, `locals`
- **Rust bridge** (`src/bridge.rs`): 190-line deserializer converting "a" runtime `Value::Map` bytecode into native Rust `CompiledProgram`/`CompiledFn`/`Chunk`/`Op` structs
  - `value_to_op`: converts `Value::Array` (e.g. `["Const", 0]`) into Rust `Op` enum for all 40+ opcodes
  - `value_to_chunk`: converts map with `code`, `constants`, `strings`, `locals` into native `Chunk`
  - `value_to_fn`: converts function map into `CompiledFn`
  - `value_to_program`: entry point producing a `CompiledProgram` executable by the existing Rust VM
- **Bridge builtins** in `src/vm.rs`:
  - `__bridge_convert__(prog)`: returns metadata map (`ok`, `functions` count, `has_main`)
  - `__bridge_exec__(prog)`: converts self-compiled program to native Rust structs, spawns new VM, executes
  - Dispatched directly in `VM::do_call` for guaranteed recognition
- **Self-compile test suite** (`tests/test_self_compile.a`): 22 tests verifying bytecode structure and opcode sequences
  - Tests: simple function, parameters, let/assign, if/else, while loop, for loop, array literal, map literal, string interpolation, field access, index, binary ops, short-circuit, method calls, tail calls, match, lambdas/closures, try/result, concat, locals resolution, main_idx detection
- **Bridge test suite** (`tests/test_bridge.a`): 12 end-to-end tests compiling and executing through the full pipeline
  - Tests: bridge convert metadata, hello world, math, function calls, if/else, while, for, arrays, maps, recursion, string interpolation, tail call optimization
- `**examples/compile_and_run.a`**: CLI tool for the self-hosted compiler
  - `--dump`: human-readable bytecode listing (function names, constants, strings, opcodes with indices)
  - `--json`: full bytecode as pretty-printed JSON
  - `--exec`: compile and execute via the Rust bridge
  - Usage: `a run examples/compile_and_run.a -- <file.a> [--dump] [--exec] [--json]`
- **Bug fix: double-indexed assignment**: `s["code"][idx] = val` silently discarded modifications due to COW semantics; fixed `patch_jump` and `patch_continues` to extract-modify-reassign
- **Bootstrap milestone**: "a" can now lex, parse, AND compile its own source code into executable bytecode, entirely in "a"
- Self-hosted compiler pipeline: 4,979 lines across 4 modules (`lexer.a`, `ast.a`, `parser.a`, `compiler.a`)
- 260 Rust integration tests + 218 native "a" tests (all passing)

### v0.30 -- Bytecode Serialization

- `**.ac` file format**: JSON serialization of `CompiledProgram` maps, loadable by both Rust CLI and self-hosted bridge
  - Schema: `{"version": 1, "functions": [...], "main_idx": N}` with opcodes as `["OpName", arg1, arg2]` arrays
  - Constants serialize naturally: `Int` as number, `String` as string, `Bool` as boolean, `Void` as `null`
  - Interoperable: `.ac` files from the Rust compiler and the self-hosted compiler are identical format
- **Inverse bridge** (`src/bridge.rs`): `program_to_value` converts native `CompiledProgram`/`CompiledFn`/`Chunk`/`Op` structs into `Value::Map` for JSON serialization
  - `op_to_value`: converts each `Op` enum variant to `Value::Array` (e.g., `Op::Call(3, 2)` becomes `["Call", 3, 2]`)
  - `chunk_to_value`, `fn_to_value`: wrap code/constants/strings/lines/params/locals into nested maps
  - Symmetric with the existing `value_to_program` deserializer
- `**a compile <file.a>`**: New CLI command that compiles and writes `.ac` files
  - Default output: `<file>.ac` (e.g., `a compile foo.a` writes `foo.ac`)
  - `-o path` flag for explicit output path
  - Uses Rust compiler + `program_to_value` + `value_to_json` + `serde_json::to_string_pretty`
- `**a run <file.ac>**`: Extended to detect `.ac` extension and load precompiled programs directly
  - Reads JSON, deserializes via `json_to_value` + `value_to_program`, executes in VM
  - Skips lex/parse/compile entirely -- instant startup from precompiled bytecode
- **Self-hosted serialization module** (`std/compiler/serialize.a`): 33 lines
  - `serialize(prog)` / `serialize_pretty(prog)`: wrap program map with version field, convert to JSON
  - `deserialize(s)`: parse JSON back to program map
  - `compile_to_file(src, path)` / `load_from_file(path)`: file I/O wrappers
  - `compile_file_to_file(src_path, out_path)`: read source, compile, write `.ac`
- `**examples/precompile.a`**: CLI tool for self-hosted compilation to `.ac` files
  - Usage: `a run examples/precompile.a -- <file.a> [output.ac] [--exec] [--pretty]`
  - Full self-hosted pipeline: lex + parse + compile + serialize + write to disk
  - Optional `--exec` flag to execute the compiled program via bridge after writing
- **Serialization test suite** (`tests/test_serialize.a`): 22 tests
  - Structure preservation: function count, main_idx, names, params, locals, constants, strings, opcodes, lines, version field
  - Execution roundtrips: hello, math, function calls, if/else, while, recursion
  - File I/O roundtrips: compile_to_file + load_from_file + bridge_exec
  - Edge cases: pretty serialize, no-main programs, string/bool constant preservation, multi-function programs
- 260 Rust integration tests + 240 native "a" tests (all passing)

## v0.31 -- Bootstrap: Self-Hosted Compiler Compiles Itself (DONE)

The self-hosted compiler (`std/compiler/*.a`) successfully compiles its own source code. The resulting G1 compiler produces functionally equivalent output to the G0 (Rust-compiled) version, and G1 self-compiles into G2 with identical structure -- proving a fixed point.

### Bootstrap chain: G0 -> G1 -> G2

```
G0 (Rust-compiled self-hosted compiler)
  -> compiles bootstrap driver -> G1 (392 functions)
    -> G1 compiles test programs: correct output
    -> G1 self-compiles bootstrap driver -> G2 (392 functions)
      -> G2 compiles test programs: correct output
      -> Fixed point: G1 and G2 have identical function count
```

### Changes

- **Bug fix: `load_module_fns` type check** (`std/compiler/compiler.a`): Changed `type_of(src) != "string"` to `type_of(src) != "str"` -- `type_of()` returns `"str"` for strings, not `"string"`. This fix enabled the self-hosted compiler's module loading (`use` declarations) to work correctly, allowing recursive loading of `std/compiler/lexer.a`, `parser.a`, `ast.a`, `compiler.a`, and `serialize.a`.
- **Bug fix: `examples/compile_and_run.a`**: Changed `f["arity"]` to `len(f["params"])` since the arity field doesn't exist in self-hosted compiler output.
- **Bootstrap proof driver** (`examples/bootstrap.a`): ~120 lines
  - `compile` mode: standalone compiler CLI (`args()[0]` = source, `args()[1]` = output)
  - `test` mode: full 8-step bootstrap proof (G0 compile, roundtrip, self-compile modules, G0->G1, G1 compiles test, G1->G2, G2 compiles test, fixed point verification)
  - Uses `__bridge_exec__(g1_prog, "compile", src, out)` for cross-generation compilation
- **Bytecode comparison tool** (`examples/compare_bc.a`): ~90 lines
  - Compares two `.ac` files for structural differences (function count, names, opcode count, constants, strings)
  - Optional `--exec` flag to execute both and show output
- **Bootstrap test suite** (`tests/test_bootstrap.a`): 19 tests
  - Self-compilation: lexer (10 fns), ast (71 fns), parser (212 fns), compiler (320 fns), serialize
  - Behavioral equivalence: hello, math, fib, while, for, string ops, maps, TCO
  - Roundtrips: compile -> serialize -> deserialize -> execute, file I/O roundtrip
  - Full bootstrap chain: G1 compiles test programs, G2 fixed point verification, G2 compiles and executes
- All self-compilation benchmarks: lexer 112ms, ast 46ms, parser 1.9s, compiler 8.6s, driver 9.1s
- All 19 bootstrap tests + 22 self-compile tests + 22 serialize tests + 12 bridge tests passing

## v0.32 -- `eval`: Runtime Code Generation (DONE)

First-class `eval(code_string)` builtin that compiles and executes arbitrary "a" source code at runtime using the Rust compiler. Supports automatic expression wrapping, value return, argument passing, nested eval, and error handling.

### Semantics

- `eval("1 + 2")` returns `3` (auto-wraps expressions in `fn main() { ret ... }`)
- `eval("fn fib(n) { ... } fn main() { ret fib(10) }")` returns `55` (full programs)
- `eval("println(42)")` prints `42` (side effects work)
- `eval(code, "arg1", "arg2")` passes args to child VM's `args()`
- Compile/runtime errors return `"eval error: ..."` strings (non-crashing)
- Nested eval works: `eval("eval(\"1 + 2\")")` returns `3`

### Changes

- `**eval` builtin** (`src/vm.rs`): ~45 lines
  - `eval_compile(code)`: tries to compile as a full program; if no `main`, auto-wraps as `fn main() { ret <code> }`; graceful fallback
  - `try_compile(code)`: helper that runs Rust lexer + parser + compiler pipeline
  - `do_call` handler: pops code string and optional extra args, compiles, runs in isolated child VM, pushes result
  - Registered in `is_builtin` list (`src/builtins.rs`)
- **REPL example** (`examples/repl.a`): ~25 lines
  - Interactive read-eval-print loop using `eval` + `io.read_line`
  - Error display, void suppression, exit/quit commands
- **Eval test suite** (`tests/test_eval.a`): 25 tests
  - Expression eval: int, float, bool, string, array, comparison, string concat
  - Statement eval: let+return, if/else, while loops
  - Function eval: multi-function programs, recursion
  - Builtins in eval: len, to_str, type_of
  - Side effects: println
  - Error handling: compile errors, non-string input
  - Nested eval, argument passing, maps, complex programs
- 260 Rust integration tests + 25 eval tests + all prior test suites passing

---

## v0.33 -- Raw Strings, Heredocs, and Regex  ✅

*Completed: April 2026*

Eliminates escape hell for code generation and enables real text processing with native regex.

### Raw Strings

Two forms, both produce `StringLit` tokens (no parser/compiler changes needed):

- `**r"..."`** -- no escapes, no interpolation. Content is literal until closing `"`.
- `**r#"..."#**` -- can contain `"` characters. Closes on `"#` (or `"##` for `r##"..."##`, etc.).

```a
let path = r"C:\Users\name\file.txt"
let code = r#"fn main() { ret eval("1 + 2") }"#
```

### Heredocs

Triple-backtick multiline strings. No escapes, no interpolation. Leading newline after opener and trailing newline before closer are stripped.

```a
let program = ```
fn fib(n) {
  if n < 2 { ret n }
  ret fib(n - 1) + fib(n - 2)
}
fn main() { ret fib(10) }
```
```

### Regex Builtins

Native regex powered by the Rust `regex` crate:

- `regex.is_match(pattern, text)` -- returns `bool`
- `regex.find(pattern, text)` -- returns `#{"match": str, "start": int, "end": int}` or `false`
- `regex.find_all(pattern, text)` -- returns array of match strings
- `regex.replace(pattern, text, replacement)` -- replace first occurrence
- `regex.replace_all(pattern, text, replacement)` -- replace all occurrences
- `regex.split(pattern, text)` -- returns array of parts
- `regex.captures(pattern, text)` -- returns array of capture groups or `false`

### Changes

- **Rust lexer** (`src/lexer.rs`): ~60 lines
  - Raw string lexing (`r"..."`, `r#"..."#`) in identifier arm with hash counting and peek-ahead matching
  - Heredoc lexing (triple-backtick) with leading/trailing newline stripping
- **Self-hosted lexer** (`std/compiler/lexer.a`): ~80 lines
  - Mirror of Rust raw string and heredoc lexing for bootstrap compatibility
- **Regex builtins** (`src/builtins.rs`): ~90 lines
  - 7 regex functions registered in `is_builtin` and dispatched in `call_builtin`
  - `regex` crate v1 added to `Cargo.toml`
- **Raw string tests** (`tests/test_raw_strings.a`): 17 tests
  - Raw string basics, hash-delimited raw strings, heredocs, eval integration
- **Regex tests** (`tests/test_regex.a`): 15 tests
  - is_match, find, find_all, replace, replace_all, split, captures, error handling
- All prior test suites passing (194+ core tests + 32 new v0.33 tests)

---

## v0.34 -- Syntax Highlighting Extension  ✅

*Completed: April 2026*

VS Code / Cursor extension providing full syntax highlighting for `.a` files via a TextMate grammar. Pure declarative JSON -- no build step, no runtime JS.

### Highlights

- Line comments (`;`)
- Keywords: `fn`, `let`, `mut`, `if`, `else`, `match`, `for`, `in`, `while`, `break`, `continue`, `ret`, `use`, `try`, `pub`, `mod`, `ty`, `effects`, `pre`, `post`, `where`
- Boolean literals: `true`, `false`
- Primitive types: `i8`..`i64`, `u8`..`u64`, `f32`, `f64`, `bool`, `str`, `bytes`, `void`
- Number literals (integer and float)
- String literals with escape sequence highlighting
- String interpolation (`"hello {expr}"`) with nested expression highlighting
- Raw strings (`r"..."`, `r#"..."#`, `r##"..."##`)
- Heredocs (triple-backtick blocks)
- Function definition names, function call names
- Type and module declaration names
- All operators (`+`, `-`, `*`, `/`, `%`, `==`, `!=`, `&&`, `||`, `|>`, `->`, `=>`, etc.)
- Bracket matching, auto-close, comment toggling, indentation rules

### Files

- `editors/vscode/package.json` -- extension manifest with language/grammar contributions
- `editors/vscode/language-configuration.json` -- brackets, comments, folding, indentation
- `editors/vscode/syntaxes/a.tmLanguage.json` -- full TextMate grammar (~230 lines)

### Installation

Symlink into Cursor extensions:

```sh
ln -s /path/to/a/editors/vscode ~/.cursor/extensions/a-lang
```

Then reload the window. All `.a` files will be syntax-highlighted automatically.

---

## v0.35 -- Language Reference  ✅

*Completed: April 2026*

Comprehensive language reference document (`REFERENCE.md`) at the repo root. A single file that makes the "a" language immediately usable by any AI instance or human reader without needing to read source code.

### Contents

14 sections covering every aspect of the language:

1. **Quick Start** -- hello world, CLI commands
2. **Syntax Basics** -- comments, newlines, top-level forms
3. **Types** -- primitives, arrays, maps, tuples, function types, records, sum types, generics
4. **Variables and Bindings** -- let, mut, destructuring
5. **Functions** -- declarations, lambdas, effects, contracts
6. **Control Flow** -- if/else, match, for, while, break/continue, ret
7. **Expressions and Operators** -- precedence table, pipe, try, field access
8. **Strings** -- regular, interpolation, raw, heredocs
9. **Pattern Matching** -- all pattern forms, guards
10. **Modules and Imports** -- mod, use, dotted access
11. **Error Handling** -- Ok/Err, try, unwrap, expect
12. **Builtin Functions Reference** -- 100+ builtins organized by category with signatures and examples
13. **Standard Library** -- std.math, std.strings, std.cli, std.testing, std.re
14. **Complete Example** -- full working program demonstrating multiple features

### Files

- `REFERENCE.md` -- ~650 lines, the authoritative language guide

---

## v0.36 -- Module Precompilation Cache  ✅

*Completed: April 2026*

Per-module `.ac` bytecode caching, analogous to Python's `.pyc` files. When `use std.math` is encountered, the compiler checks for `std/math.ac` alongside `std/math.a`. If the `.ac` exists and is newer than the `.a` source, the pre-compiled functions are loaded directly -- skipping lex, parse, and compile entirely. If no cache exists or the source is newer, the module is compiled normally and the `.ac` cache is auto-written for next time.

### How it works

- `CompiledProgram.functions` is a flat `Vec<CompiledFn>` -- all imported module functions are merged in
- Function calls use `Op::Call(name_string_idx, nargs)` -- the VM resolves by name at runtime, not by index
- Pre-compiled functions can be freely appended to the program without index fixups
- Each `.ac` stores the module's compiled functions plus its `use` dependency list (for transitive loading)

### Cache format

JSON envelope wrapping the existing `.ac` serialization:

```json
{
  "program": { "version": 1, "functions": [...], "main_idx": null },
  "uses": [["std", "compiler", "lexer"], ["std", "compiler", "parser"]]
}
```

The `uses` field ensures transitive dependencies are loaded even when the parent module is served from cache.

### Architecture

- `**Compiler.precompiled_fns**`: new field holding `Vec<CompiledFn>` from cached modules
- `**Compiler.pending_caches**`: tracks modules compiled from source that need cache files written
- `**try_load_cached**`: checks `.ac` freshness (modified time vs source), deserializes functions, processes `uses` for transitive deps
- `**flush_caches**`: after `compile_program` finishes, extracts the already-compiled functions by name and writes `.ac` files -- zero double-compilation
- `**compile_program**`: appends `precompiled_fns` after compiling all `FnDecl` from source

### Freshness

- Cache invalidation by file modification time: `ac_modified >= src_modified`
- Touching a `.a` source file forces recompilation and cache regeneration on next use
- Cache files are written atomically after successful compilation

### Changes

- `**src/compiler.rs**`: ~80 lines added
  - `PendingCache` struct tracking `ac_path`, `fn_names`, `use_paths` per module
  - `precompiled_fns` and `pending_caches` fields on `Compiler`
  - `try_load_cached`: freshness check, JSON envelope parsing, `uses` processing, function deserialization
  - `flush_caches`: post-compilation cache write from already-compiled functions (no re-compilation)
  - `compile_program`: merges precompiled functions into final program
  - `load_module`: cache check before lex/parse, pending cache registration after compilation
- All 247+ native "a" tests passing (both cache miss and cache hit)
- Bootstrap tests passing (19 tests including G2 fixed point)
- Cache freshness verified: `touch std/testing.a` triggers regeneration

---

## v0.37 -- AST Emitter: Code Generation from ASTs  ✅

*Completed: April 2026*

Self-hosted AST-to-source pretty printer (`std/compiler/emitter.a`) that converts tagged-map ASTs back into formatted, compilable "a" source code. This closes the metaprogramming loop: `source -> parse -> transform -> emit -> eval`.

### Why it matters

The self-hosted compiler can go **source -> AST -> bytecode**, and `eval` can execute code at runtime. But there was no way to go **AST -> source** from within "a" itself. With the emitter, the full metaprogramming loop is available:

```
Source Code -> lex + parse -> Tagged-Map AST -> transform -> Modified AST -> emit -> New Source -> eval / write
```

Code that writes code, cleanly -- using structural AST builders (`mk_*` from `ast.a`) instead of fragile string concatenation.

### The module: `std/compiler/emitter.a`

A direct port of the formatting logic from `src/formatter.rs`, operating on tagged-map ASTs (`#{"tag": "...", ...}`) instead of Rust enum types.

### API

- `**emit(ast)**` -- entry point. Takes a `Program` AST node, returns formatted source string
- `**emit_top_level(node, level)**` -- dispatches on FnDecl, TypeDecl, ModDecl, UseDecl
- `**emit_fn_decl(node, level)**` -- function with params, return type, effects, pre/post, body
- `**emit_block(node, level)**` -- `{ ... }` with indented statements
- `**emit_stmt(node, level)**` -- Let, Assign, Return, If, Match, For, While, Break, Continue, destructuring
- `**emit_expr(node)**` -- all expression tags (Int, Float, String, BinOp, Call, Lambda, Pipe, etc.)
- `**emit_pattern(node)**` -- PatIdent, PatWildcard, PatLiteral, PatConstructor, PatArray, PatMap
- `**emit_type(node)**` -- TyPrim, TyArray, TyMap, TyFn, TyNamed, TyInfer

### Formatting conventions (matching `formatter.rs`)

- 2-space indentation per nesting level
- BinOp: `(left OP right)` with spaces around operator
- Blocks: `{\n` + indented stmts + `}`
- Functions: `fn name(p1, p2) -> RetType` then effects/pre/post on next lines
- Match arms: `pattern => expr` or `pattern => {\n  body\n}`
- Strings with `\"` escaping, interpolation parts as `{expr}`
- All 66 AST node tags handled

### Changes

- `**std/compiler/emitter.a**`: ~580 lines
  - `emit()` entry point with `emit_top_level` dispatch
  - `is_present()` helper for null/void/Void-node checking
  - `mk_indent()` for 2-space indentation
  - Full statement emitter: Let (typed/untyped/mutable), Assign, Return, ExprStmt, If/ElseIf/Else, Match with guards, For (typed/untyped), While, LetDestructure, ForDestructure, Break, Continue
  - Full expression emitter: Int, Float, String, Bool, Void, Ident, BinOp, UnaryOp, Call, FieldAccess, Index, Try, TryBlock, Array, Record, Lambda, Pipe, Interpolation, MapLiteral, Spread, IfExpr, MatchExpr, BlockExpr
  - Pattern emitter: PatWildcard, PatIdent, PatLiteral, PatConstructor, PatArray (with PatElem/PatRest), PatMap
  - Type emitter: TyPrim, TyNamed, TyArray, TyTuple, TyRecord, TyFn, TyMap, TyInfer
  - Type body emitter: TypeRecord, TypeSum, TypeAlias (with where clause)
  - `escape_str()` for string literal escaping
- `**tests/test_emitter.a**`: 23 tests
  - Roundtrip tests: parse `examples/hello.a`, `examples/math.a`, `examples/fibonacci.a` -> emit -> eval (correct execution through emitted code)
  - AST builder tests: programmatically construct ASTs using `mk_*`, emit to source, compile and execute
  - Per-construct tests: simple fn, params, let, if/else, while, for, array, map, string, call, field access, index, pipe, lambda, use, break/continue, match, unary, typed params, spread
- All prior test suites passing (247+ native tests + 23 emitter tests)

---

## v0.38 -- Metaprogramming Toolkit  ✅

*Completed: April 2026*

High-level metaprogramming library (`std/meta.a`) providing AST walking, searching, analysis, transformation, and code generation utilities. Built on top of the parser (v0.28), emitter (v0.37), and AST constructors (v0.28), it gives "a" programs the ability to read, analyze, rewrite, and generate other "a" programs at the AST level.

### Why it matters

The compiler infrastructure (parser, emitter, AST builders) was spread across `std/compiler/` and required knowledge of internal AST tag names and field conventions. `std/meta.a` provides a single high-level API that makes metaprogramming accessible: parse a file, walk the AST, find functions, transform code, generate new functions, emit back to source -- all in a few lines.

### The module: `std/meta.a`

~240 lines importing `std.compiler.parser`, `std.compiler.emitter`, and `std.compiler.ast`.

### API

**Parsing & Emitting**

- `meta.parse(source)` -- parse source string to AST
- `meta.parse_file(path)` -- read file and parse
- `meta.emit(prog_ast)` -- AST to formatted source string
- `meta.emit_to_file(prog_ast, path)` -- emit and write to disk

**AST Walking**

- `meta.walk(node, visitor_fn)` -- depth-first traversal, calls visitor on every tagged node
- `meta.find_all(node, pred_fn)` -- collect all nodes matching a predicate
- `meta.find_fns(prog_ast)` -- find all FnDecl nodes
- `meta.find_calls(node)` -- find all Call nodes

**AST Analysis**

- `meta.fn_names(prog_ast)` -- list of function names
- `meta.fn_signatures(prog_ast)` -- list of `#{"name", "params", "param_count", "has_return_type"}` records
- `meta.uses(prog_ast)` -- list of use declaration paths
- `meta.call_graph(prog_ast)` -- map of function name → list of called function names

**AST Transformation**

- `meta.transform(source, transform_fn)` -- parse, apply transform, emit back to source
- `meta.transform_file(path, transform_fn)` -- transform file in place
- `meta.map_items(prog_ast, map_fn)` -- map over top-level items
- `meta.filter_items(prog_ast, pred_fn)` -- filter top-level items
- `meta.add_items(prog_ast, new_items)` -- append items to program
- `meta.inject_stmt(fn_node, position, stmt_node)` -- inject statement at start/end of function body

**Code Generation**

- `meta.gen_fn(name, param_names, body_stmts)` -- generate a FnDecl AST node
- `meta.gen_test(name, body_stmts)` -- generate a `test_`* function
- `meta.gen_call(fn_name, arg_exprs)` -- generate a Call expression node

### Example: auto-generate test skeletons

`examples/gen_tests.a` demonstrates the full metaprogramming loop:

```a
use std.meta
use std.compiler.ast

fn main() {
  let prog = meta.parse_file(args()[0])
  let sigs = meta.fn_signatures(prog)
  let items = [ast.mk_use_decl(["std", "testing"])]
  for sig in sigs {
    if sig["name"] == "main" { continue }
    if str.starts_with(sig["name"], "test_") { continue }
    let body_stmts = [ast.mk_expr_stmt(
      meta.gen_call("println", [ast.mk_string("TODO: test " + sig["name"])])
    )]
    items = push(items, meta.gen_test(sig["name"], body_stmts))
  }
  println(meta.emit(ast.mk_program(items)))
}
```

### Bug fix: module precompilation cache name collision

Fixed a bug in `flush_caches` where bare function names (e.g. `emit`) could collide across modules (both `emitter.a` and `compiler.a` define `emit`). The cache writer now looks up functions by their namespaced name (e.g. `emitter.emit`) to ensure the correct bytecode is cached. This was causing `test_meta.a` to fail when run after `test_emitter.a`.

### Changes

- `**std/meta.a**`: ~240 lines -- full metaprogramming API
- `**examples/gen_tests.a**`: ~40 lines -- demonstration tool
- `**tests/test_meta.a**`: 24 tests
  - Walk/find: visit all tags, visit nested, find by tag, find fns, find calls, empty program
  - Analysis: fn_names, fn_signatures, uses, call_graph
  - Transformation: transform_filter, map_items, add_items, inject_stmt_start, inject_stmt_end
  - Code generation: gen_fn_simple, gen_fn_emits_valid_code, gen_test, gen_call, gen_call_emits_valid_code
  - Roundtrips: parse_emit_eval, generate_and_execute, inject_and_execute, add_items_and_execute
- `**src/compiler.rs**`: cache fix -- `flush_caches` uses namespaced function lookup to avoid cross-module name collisions
- All 181 native "a" tests passing across 9 test suites

---

## v0.39 -- Structured Concurrency  ✅

*Completed: April 2026*

Safe, structured concurrency primitives enabling parallel I/O, batch processing, and concurrent workflows. Each spawned task runs in an isolated VM with its own stack -- no shared mutable state, no data races, no deadlocks. Communication happens exclusively through return values.

### Design

- **Isolated VMs**: Each spawned task gets its own `VM` instance with a fresh stack and call frames
- **Shared program**: All VMs share the same `Arc<CompiledProgram>` (zero-cost cloning of bytecode)
- **Value safety**: `Value` uses `Arc<T>` instead of `Rc<T>`, making all values `Send + Sync`
- **Structured**: Tasks are always awaited or joined -- no fire-and-forget threads leaking

### Concurrency Primitives

```a
; spawn + await: start a task, get result later
let handle = spawn(fn() => http.get("https://api.example.com"))
let result = await(handle)   ; Ok(response) or Err(message)

; await_all: wait for multiple tasks
let handles = [spawn(fn() => task1()), spawn(fn() => task2())]
let results = await_all(handles)   ; [Ok(...), Ok(...)]

; parallel_map: map over array concurrently
let responses = parallel_map(urls, fn(url) => http.get(url))

; parallel_each: concurrent side effects
parallel_each(files, fn(f) => process(f))

; timeout: run with deadline
let r = timeout(3000, fn() => slow_operation())   ; Err("timeout") if too slow
```

### API

- `**spawn(fn)**` -- Start a 0-arg function/closure in a new OS thread. Returns opaque `TaskHandle`.
- `**await(handle)**` -- Block until task completes. Returns `Ok(value)` or `Err(message)`. Consumes the handle.
- `**await_all(handles)**` -- Wait for array of handles. Returns array of Results in order.
- `**parallel_map(arr, fn)**` -- Map 1-arg function over array concurrently. Returns array of results.
- `**parallel_each(arr, fn)**` -- Like parallel_map but discards results. For side effects.
- `**timeout(ms, fn)**` -- Run function with millisecond deadline. Returns `Ok(value)` or `Err("timeout")`.

### Changes

- `**src/interpreter.rs**`: `Value` migrated from `Rc<T>` to `Arc<T>` for thread safety; `TaskHandle(usize)` variant added
- `**src/vm.rs**`: `VM.program` changed to `Arc<CompiledProgram>`; `new_shared()` constructor; `task_handles` HashMap + `next_task_id` counter; all 6 concurrency builtins implemented in `do_call`
- `**src/builtins.rs**`: `Rc` to `Arc` migration; `spawn`, `await`, `await_all`, `parallel_map`, `parallel_each`, `timeout` registered in `is_builtin`
- `**src/bridge.rs**`: `Rc` to `Arc` migration
- `**tests/test_concurrency.a**`: 23 tests covering spawn/await, closures, error propagation, await_all, parallel_map, parallel_each, timeout, independence, eval in tasks
- `**examples/parallel_fetch.a**`: demo fetching multiple URLs concurrently
- All 204 native "a" tests passing across 10 test suites

---

## v0.40 -- Practical Standard Library  ✅

*Completed: April 2026*

Six new standard library modules that fill the practical gaps blocking real-world use. After this, "a" is a language you can actually use to build real tools -- static site generators, data pipelines, deploy scripts, API test harnesses.

### New Rust-Side Builtins

Four new builtins implemented in the Rust runtime (functionality that cannot be implemented in pure "a"):

- `**time.now()**` -- current Unix epoch milliseconds (`std::time::SystemTime`)
- `**time.sleep(ms)**` -- sleep for milliseconds (`std::thread::sleep`)
- `**hash.sha256(s)**` -- SHA-256 hex digest (via `sha2` crate)
- `**hash.md5(s)**` -- MD5 hex digest (via `md-5` crate)

### New Modules

**`std/path.a`** -- Path manipulation (pure "a", ~115 lines)
- `join`, `join3`, `dirname`, `basename`, `extension`, `stem`, `with_extension`, `is_absolute`, `segments`, `normalize`
- Unix-oriented, string-based, no OS dependencies

**`std/datetime.a`** -- Date and time (~130 lines, uses `time.now`/`time.sleep` builtins)
- `now`, `timestamp`, `sleep`, `to_parts`, `from_parts`, `format`, `iso`
- `add_ms`, `add_seconds`, `add_minutes`, `add_hours`, `add_days`, `diff_ms`, `diff_seconds`
- Format tokens: `%Y`, `%m`, `%d`, `%H`, `%M`, `%S`, `%F` (date), `%T` (time)
- Full epoch-to-calendar conversion with leap year handling

**`std/hash.a`** -- Hashing utilities (~25 lines, wraps Rust builtins)
- `sha256`, `md5`, `sha256_file`, `md5_file`, `quick` (short md5 for cache keys)

**`std/encoding.a`** -- Encoding/decoding (pure "a", ~185 lines)
- `base64_encode`, `base64_decode` -- lookup-table based, RFC 4648
- `hex_encode`, `hex_decode` -- byte-to-hex conversion
- `url_encode`, `url_decode` -- percent-encoding with `+` for space

**`std/csv.a`** -- CSV parsing and generation (pure "a", ~140 lines)
- `parse`, `parse_records`, `stringify`, `stringify_records`, `parse_row`, `escape_field`
- RFC 4180 compliant: quoted fields, embedded commas, escaped quotes

**`std/template.a`** -- Mustache-style string templating (pure "a", ~210 lines)
- `render(template, vars)`, `render_file(path, vars)`
- Variable substitution: `{{name}}`
- Conditionals: `{{#if key}}...{{#else}}...{{/if}}`
- Iteration: `{{#each arr}}...{{.}}...{{@index}}...{{#else}}...{{/each}}`
- Nested block support, map-item field promotion in each blocks

### Changes

- `**Cargo.toml**`: Added `sha2 = "0.10"`, `md-5 = "0.10"` dependencies
- `**src/builtins.rs**`: `time.now`, `time.sleep`, `hash.sha256`, `hash.md5` implemented and registered
- `**src/vm.rs**`: Fast-path VM handling for all 4 new builtins
- `**std/path.a**`, `**std/datetime.a**`, `**std/hash.a**`, `**std/encoding.a**`, `**std/csv.a**`, `**std/template.a**`: 6 new standard library modules
- `**tests/test_path.a**`: 35 tests
- `**tests/test_datetime.a**`: 15 tests
- `**tests/test_hash.a**`: 10 tests
- `**tests/test_encoding.a**`: 20 tests
- `**tests/test_csv.a**`: 17 tests
- `**tests/test_template.a**`: 15 tests
- `**examples/site_gen.a**`: Static site generator demo exercising all 6 modules
- All 498 native "a" tests passing across 27 test suites

## v0.41 -- C Code Generation ✅

### Motivation

The self-hosted compiler can lex, parse, and compile "a" to bytecode, but that bytecode still runs on the Rust VM. The language is not truly independent. v0.41 adds a **C code generation backend**: the self-hosted compiler's parser produces an AST; a new code generator walks that AST and emits C source code. Combined with a small C runtime library, this produces **native executables via gcc**. No Rust, no VM, no interpreter. Just machine code.

This is the first step toward full bootstrap: eventually the C code generator itself will be compiled to C, and "a" will exist without any Rust dependency.

### Architecture

```
program.a → parser (std.compiler.parser) → AST → cgen (new) → program.c
program.c + runtime.c → gcc → native binary
```

### C Runtime Library (~600 lines)

`c_runtime/runtime.h` + `c_runtime/runtime.c` -- a minimal C library providing:

- **Tagged union value model** (`AValue`): int64, float64, bool, void, string, array, map, result
- **Reference-counted heap types**: `AString` (flexible array member), `AArray`, `AMap`
- **Memory management**: `a_retain()`, `a_release()` -- simple refcounting matching the Rust `Arc` model
- **Arithmetic + comparison**: `a_add`, `a_sub`, `a_mul`, `a_div`, `a_mod`, `a_eq`, `a_lt`, etc. with int/float promotion
- **String operations**: `a_str_concat`, `a_str_split`, `a_str_replace`, `a_str_trim`, `a_str_upper`, `a_str_lower`, `a_str_join`, `a_str_chars`, `a_str_slice`, `a_str_starts_with`, `a_str_ends_with`, `a_concat_n` (variadic, for interpolation)
- **Array operations**: `a_array_new` (variadic), `a_array_get`, `a_array_push`, `a_array_slice`, `a_sort`, `a_contains`
- **Map operations**: `a_map_new` (variadic), `a_map_get`, `a_map_set`, `a_map_has`, `a_map_keys`, `a_map_values`, `a_map_merge`
- **I/O**: `a_println`, `a_print`, `a_eprintln`, `a_io_read_file`, `a_io_write_file`
- **Result**: `a_ok`, `a_err`, `a_is_ok`, `a_is_err`, `a_unwrap`
- **Utility**: `a_type_of`, `a_args`, `a_fail`, `a_to_int`, `a_to_float`, `a_to_str`

### C Code Generator (`std/compiler/cgen.a`, ~400 lines)

Written entirely in "a", uses `std.compiler.parser` and `std.compiler.ast`. Handles:

- Function declarations with forward declarations
- All expressions: literals, identifiers, binary/unary ops, calls, field access, indexing
- All statements: let, assign, return, if/else/else-if, while, for, break, continue
- String interpolation → `a_concat_n()` calls
- Array literals → `a_array_new()` calls
- Map literals → `a_map_new()` calls
- Builtin recognition: 40+ builtins map directly to C runtime functions
- Function name mangling for `mod.fn` → `fn_mod_fn` style

### Usage

```bash
# Generate C from any "a" program
a run std/compiler/cgen.a -- program.a > program.c

# Compile to native binary
gcc program.c c_runtime/runtime.c -o program -I c_runtime -lm -O2

# Run natively
./program
```

### Performance

fib(35) benchmark: **28s on VM → 0.17s native (164x speedup)**

### Test Results

6 milestone programs all pass (VM output == native output):

| Program | Features tested |
|---------|----------------|
| `fib.a` | Recursion, arithmetic, conditionals |
| `strings.a` | String literals, interpolation, `str.concat`, `len` |
| `arrays.a` | Array creation, for loops, mutation, interpolation |
| `maps.a` | Map creation, `map.get`, `map.set` |
| `multi_fn.a` | Multi-function calls, factorial, array iteration |
| `complex.a` | FizzBuzz, helper functions, maps, arrays, while loops |

### Changes

- **`c_runtime/runtime.h`**: Value types, function declarations (~140 lines)
- **`c_runtime/runtime.c`**: Full runtime implementation (~600 lines)
- **`std/compiler/cgen.a`**: C code generator (~400 lines)
- **`examples/c_targets/*.a`**: 6 milestone test programs
- **`tests/test_cgen.sh`**: Automated compile + run + compare test script

## v0.42 -- The Bootstrap: Self-Hosting via C  ✅

*Completed: April 2026*

The "a" language is now **self-hosting through native compilation**. The C code generator (`std/compiler/cgen.a`) compiles its own source -- including the full lexer, parser, and AST modules -- into 3,856 lines of C. gcc compiles that C into a native binary (`ac`) that is a standalone "a"-to-C compiler. This native compiler produces output identical to the VM-hosted version, reaching a **fixed point**: the language compiles itself and the output doesn't change.

### The Bootstrap Chain

```
Step 1: VM runs cgen.a on cgen.a → gen0.c (3,856 lines of C)
Step 2: gcc gen0.c + runtime.c → ac0 (native "a"-to-C compiler, ~213KB)
Step 3: ac0 compiles cgen.a → gen1.c
Step 4: diff gen0.c gen1.c → IDENTICAL (fixed point reached)
```

The native `ac` binary has no Rust dependency. It reads `.a` source, parses it with the full self-hosted parser, and emits C. It is a freestanding compiler.

### What was needed

**Module inlining** -- The C code generator previously only handled single-file programs. For bootstrap, it needs to resolve `use` declarations, read the referenced module files, parse them, and emit all their functions with namespace prefixes. `cgen.a` uses `parser` and `ast`; `parser.a` uses `lexer` and `ast`. The inliner recursively loads the entire dependency tree, deduplicates modules, and emits functions as `fn_<module>_<name>` (e.g. `fn_parser_parse`, `fn_lexer_lex`, `fn_ast_mk_program`).

**C runtime extensions:**
- `a_is_alpha`, `a_is_digit`, `a_is_alnum` -- character classification builtins needed by the lexer
- `a_array_get` extended to handle map indexing (`arr["key"]` → `a_map_get` when arr is a map)
- `\r` (carriage return) escaping in `_escape_c_str`

**Variable pre-declaration** -- "a" allows `let r = ...` multiple times in the same scope (shadowing). C doesn't. The code generator now pre-scans each function body for all variable names and declares them once at the top: `AValue r, pos, name, ...;`. All `let` statements emit as assignments.

**C keyword escaping** -- Variables named `short`, `long`, `int`, etc. are prefixed with `_` to avoid conflicts with C reserved words.

### Performance

The native `ac` compiler runs in ~125ms to compile a program (vs ~18s for the VM to run cgen.a). That's a **144x speedup** for compilation itself.

### Test Results

All 8 milestone programs pass through the native compiler (native `ac` → gcc → execute → compare with VM output):

| Program | Status |
|---------|--------|
| arrays | PASS |
| bench_fib | PASS |
| complex | PASS |
| fib | PASS |
| maps | PASS |
| multi_fn | PASS |
| newline_test | PASS |
| strings | PASS |

Fixed-point test: gen0.c == gen1.c (3,856 lines, byte-identical)

### Changes

- **`std/compiler/cgen.a`**: Module inlining, variable pre-declaration, C keyword escaping, `\r` escaping, `is_alpha`/`is_digit`/`is_alnum` in builtin map (~530 lines, up from ~400)
- **`c_runtime/runtime.c`**: `a_is_alpha`, `a_is_digit`, `a_is_alnum` builtins; `a_array_get` map indexing support
- **`c_runtime/runtime.h`**: New function declarations
- **`tests/test_bootstrap.sh`**: Full bootstrap validation script (gen0 → ac0 → milestones → gen1 → fixed point)
- **`examples/c_targets/newline_test.a`**: Test case for special character escaping

## v0.43 -- Closures & Higher-Order Functions in Native Compilation  ✅

*Completed: April 2026*

The native compiler now supports **closures, lambdas, higher-order functions, and the pipe operator**. Every `map`, `filter`, `reduce`, `sort_by`, `find` call that takes a lambda -- the bread and butter of idiomatic "a" code -- now compiles to native C. The self-compiled output grew from 3,856 to 4,388 lines of C, and the bootstrap fixed point still holds.

### Closure Implementation

Closures in C are represented as a struct pairing a **function pointer** with a **captured environment**:

```c
typedef struct AClosure {
    int rc;
    AClosureFn fn;       // AValue (*)(AValue env, int argc, AValue* argv)
    AValue env;           // TAG_ARRAY of captured values, or TAG_VOID if none
} AClosure;
```

`AValue` gained a new tag `TAG_CLOSURE` with `AClosure* cval` in the union. The runtime manages closure lifecycle through the existing reference counting system.

### Lambda Lifting

When the code generator encounters a lambda expression:

1. **Free variable analysis** walks the lambda body, collects Ident references not in the lambda's own params or local bindings -- these are captures.
2. **Lift to top-level C function**: generate `AValue __lambda_N(AValue __env, int __argc, AValue* __argv)`. Captures are loaded from `__env` by index, params from `__argv`.
3. **Emit closure creation** at the original expression site: `a_closure(__lambda_N, a_array_new(K, capture1, ...))`.

Nested lambdas are handled correctly -- a closure-returning-closure (like a curried `add_n`) works through recursive lambda lifting with proper capture propagation.

### Code Generator Refactoring

All emit functions were refactored to return `[code, lambda_id, [lifted_fns]]` triples instead of plain strings. This threads the lambda counter through emission and accumulates lifted function definitions that get prepended in the output.

### What was added

**C runtime (`runtime.c` / `runtime.h`):**
- `TAG_CLOSURE`, `AClosure` struct, `a_closure()`, `a_closure_call()`, `a_closure_call_arr()`
- Reference counting for closures in `a_retain` / `a_release`
- 11 higher-order functions: `a_hof_map`, `a_hof_filter`, `a_hof_reduce`, `a_hof_each`, `a_hof_sort_by`, `a_hof_find`, `a_hof_any`, `a_hof_all`, `a_hof_flat_map`, `a_hof_min_by`, `a_hof_max_by`
- 6 array utilities: `a_enumerate`, `a_zip`, `a_take`, `a_drop`, `a_unique`, `a_chunk`
- String comparison support in `a_lt` / `a_lteq` (was numeric-only)
- Result display in `val_to_buf` (prints `Ok(...)` / `Err(...)`)

**Code generator (`cgen.a`):**
- Free variable analysis (`_collect_idents_in_expr/block/stmt`, `_compute_captures`, `_collect_lets_in_block`)
- Lambda lifting with environment capture
- Pipe operator desugaring (`|>` inserts left-hand side as first argument)
- Closure-aware call dispatch (unknown idents emit `a_closure_call` instead of direct calls)
- Lambda forward declarations in output
- All HOF and array utility builtins in the builtin map
- Emit function refactoring to `[code, li, lifted_fns]` triples (~957 lines, up from ~530)

**Test program (`examples/c_targets/closures.a`):**
- map, filter, reduce, each, sort_by, find, any, all, flat_map
- Closure captures, nested closures (closure-returning-closure)
- Array utilities: enumerate, zip, take, drop, unique, chunk

### Test Results

All 9 test programs pass (original 8 + closures):

| Program | Status |
|---------|--------|
| arrays | PASS |
| bench_fib | PASS |
| closures | PASS |
| complex | PASS |
| fib | PASS |
| maps | PASS |
| multi_fn | PASS |
| newline_test | PASS |
| strings | PASS |

Fixed-point test: gen0.c == gen1.c (4,388 lines, byte-identical)

### Known Limitation

Closures capture variables **by value**. Mutating a captured variable inside a lambda does not affect the outer scope. This matches the compiled C semantics where each `AValue` is a stack-local struct. Patterns like `each(arr, fn(x) { total = total + x })` will not propagate `total` back; use `reduce` instead.

---

## v0.45 -- Error Handling, Destructuring & Spread in Native Compilation ✅

### Motivation

Close the biggest remaining feature gaps between VM and native: `try`/`?` error handling, `let [a, b] = ...` destructuring, `for [k, v] in ...` destructured loops, and `[...arr, x]` array spread. These patterns are used heavily in real "a" programs (`search.a`, `todo_scan.a`, `std/meta.a`).

### Architecture

**Try/? (setjmp/longjmp):**
- Global try stack: `jmp_buf a_try_stack[64]` + depth counter in C runtime
- `a_try_unwrap(v)`: if Ok, returns inner value; if Err and inside try block, `longjmp`; if Err and no try block, exits with error
- `TryBlock` emission: GCC statement expression `({...})` wrapping `setjmp`/`longjmp` scaffold with local variable pre-declaration
- `Try` / `?` emission: simple `a_try_unwrap(<expr>)` call

**LetDestructure:**
- Emits indexed access: `__dest = <value>; a = a_array_get(__dest, a_int(0)); ...`
- Supports rest patterns: `let [first, ...rest] = arr` emits `a_drop(__dest, a_int(N))`
- Skips `_` bindings

**ForDestructure:**
- Same loop structure as `For`, but each iteration indexes into the element: `__elem = a_array_get(__iter_arr, a_int(__fi)); k = a_array_get(__elem, a_int(0)); ...`

**Array Spread:**
- Detects `Spread` nodes in array elements
- Switches from `a_array_new(N, ...)` to incremental `a_array_push` / `a_concat_arr` chain

### Changes

| File | What |
|------|------|
| `c_runtime/runtime.h` | `setjmp.h` include, try stack globals, `a_try_unwrap` declaration |
| `c_runtime/runtime.c` | Try stack initialization, `a_try_unwrap` implementation |
| `std/compiler/cgen.a` | `Try`, `TryBlock`, `LetDestructure`, `ForDestructure` emission; spread-aware array emission; var collection for destructure and try blocks |

### Test Results

All 10 test programs pass (original 8 + patterns + features):

| Program | Status |
|---------|--------|
| arrays | PASS |
| closures | PASS |
| complex | PASS |
| features | PASS |
| fib | PASS |
| maps | PASS |
| multi_fn | PASS |
| patterns | PASS |
| strings | PASS |
| bench_fib | PASS |

Fixed-point test: gen1.c == gen2.c (4,951 lines, byte-identical)

---

## v0.45.1 -- Expression Completeness ✅

### Motivation

Audit and fix expression handling in the native compiler to ensure every expression the VM evaluates compiles correctly to C.

### Findings & Fixes

**Lambda implicit return (critical bug fix):**
Lambda bodies like `fn(x) { x * 2 }` were not returning their last expression value — they always returned `void`. The lambda body emission now detects when the last statement is an `ExprStmt` and emits it as `return <expr>;` instead of just `<expr>;`. This fixed closures, map, filter, reduce, and all HOF results.

**Void-returning builtins:**
`a_println`, `a_print`, `a_eprintln` were `void` in C but could appear as the last expression in a lambda body. Changed them to return `AValue` (returning `a_void()`) for consistency, so `return a_println(x);` is valid C.

**MatchExpr / IfExpr:**
Investigated — neither the Rust parser nor self-hosted parser produces `MatchExpr` or `IfExpr` nodes. These are AST infrastructure for future use. Added `_emit_match_expr` to cgen.a for when the parser adds support.

**BlockExpr trailing Return:**
Fixed `BlockExpr` to handle `Return` as the last statement (previously only handled `ExprStmt`).

**UnaryOp:**
Audited — both the VM and native compiler handle only `!` (not) and `-` (negate). cgen.a maps these to `a_not()` and `a_neg()`. Complete.

### Test Results

All 11 test programs pass (original 10 + expressions):

| Program | Status |
|---------|--------|
| arrays | PASS |
| closures | PASS |
| complex | PASS |
| expressions | PASS |
| features | PASS |
| fib | PASS |
| maps | PASS |
| multi_fn | PASS |
| patterns | PASS |
| strings | PASS |
| bench_fib | PASS |

Fixed-point test: gen1.c == gen2.c (5,108 lines, byte-identical)

---

## v0.45.2 -- Native I/O Primitives ✅

### Motivation

The native compiler could emit C code, but it relied on the Rust VM to read source files. This version adds POSIX I/O to the C runtime so the native binary is fully standalone.

### C Runtime Additions (~190 lines)

| Function | What |
|----------|------|
| `a_fs_ls(path)` | `opendir`/`readdir`/`closedir`, returns array of `#{name, is_dir}` maps |
| `a_fs_mkdir(path)` | `mkdir()` with 0755 permissions |
| `a_fs_cwd()` | `getcwd()` |
| `a_fs_exists(path)` | `access(path, F_OK)` |
| `a_fs_is_dir(path)` | `stat()` + `S_ISDIR` |
| `a_exec(cmd)` | `popen`/`pclose`, returns `#{stdout, stderr, code}` map |
| `a_env_get(key)` | `getenv()` |
| `a_json_parse(input)` | Recursive-descent JSON parser (~130 lines), handles strings, numbers, arrays, objects, booleans, null |

Also: `io.read_file` and `io.write_file` already existed from earlier work.

### The Milestone: Three-Stage Bootstrap

```
Stage 1: VM compiles cgen.a → gen1.c (5,108 lines)
Stage 2: gcc gen1.c → ac_s1 (native binary)
Stage 3: ac_s1 reads cgen.a from disk → gen2.c (5,108 lines)
Stage 4: gen1.c == gen2.c (byte-identical)
Stage 5: gcc gen2.c → ac_s2 → gen3.c == gen2.c (three-stage fixed point)
```

No Rust VM in the loop. The native compiler reads its own source, compiles itself, and produces identical output across three generations.

### Test Results

All 12 test programs pass:

| Program | Status |
|---------|--------|
| arrays | PASS |
| closures | PASS |
| complex | PASS |
| expressions | PASS |
| features | PASS |
| fib | PASS |
| io_test | PASS |
| maps | PASS |
| multi_fn | PASS |
| patterns | PASS |
| strings | PASS |
| bench_fib | PASS |

C runtime: ~1,400 lines. cgen.a: 1,522 lines. Generated C: 5,108 lines.

---

## v0.45.3 -- Test Hardening ✅

### Motivation

Before adding FFI complexity, every existing language feature must work correctly in native compilation. This version ran the VM test suite through native, built a test harness, and fixed every edge case.

### Test Harness

Created `scripts/native_test.sh` -- a shell script that:
1. Scans test files for `fn test_*` functions
2. Generates a `main()` wrapper that calls each test (handling both bool-returning and assert-based tests)
3. Compiles to C via the VM → C → gcc pipeline
4. Runs the binary and reports pass/fail

### Bugs Found and Fixed

1. **`else if` codegen completely broken** -- The `ElseIf` AST node stores its inner `If` under key `"stmt"`, but cgen accessed `"if"` (which returns void). Every `else if` in the language emitted dead code. Fixed in three locations: `_collect_idents_in_stmt`, `emit_stmt`, `_collect_vars_in_stmts`.

2. **Index assignment broken** (`m["key"] = val`) -- Emitted `m = val` instead of `m = a_index_set(m, key, val)`. Added `a_index_set` to C runtime (dispatches on array vs map).

3. **FieldAccess missing paren** (`m.name`) -- `a_map_get(m, a_string("name")` was missing closing `)`. One character.

4. **Module import name resolution missing** -- `use std.testing` imported functions with prefixed names (e.g., `fn_testing_assert_eq`) but call sites in the importing file used unprefixed names (`assert_eq`). Added `import_aliases` map to track unprefixed→prefixed mappings. Call resolution now checks aliases first.

5. **Map iteration broken** -- `for [k,v] in map` assumed the iterable was an array. Added `a_iterable()` runtime helper that auto-converts maps to entry arrays.

6. **Missing builtins** -- Added `map.delete`, `map.entries`, `map.from_entries`, `str.find`, `str.count` to both C runtime and cgen builtin map.

### Test Results

7 test suites, 113 individual tests, all passing:

| Test Suite | Tests | Status |
|-----------|-------|--------|
| test_patterns | 22 | PASS |
| test_maps | 21 | PASS |
| test_destructure | 18 | PASS |
| test_functional | 26 | PASS |
| test_strings | 14 | PASS |
| test_core | 8 | PASS |
| test_stack_traces | 4 | PASS |

Plus 13 example programs all passing. Three-stage bootstrap verified: 5,138 lines C, byte-identical.

C runtime: ~1,500 lines. cgen.a: ~1,530 lines. Generated C: 5,138 lines.

---

## v0.46 -- C Foreign Function Interface

**Goal**: Allow "a" programs to call arbitrary C functions via `extern fn` declarations with automatic type marshalling.

### What Was Added

1. **`extern fn` syntax**: New `extern` keyword in both self-hosted lexer (`KwExtern`) and Rust lexer (`TokenKind::Extern`). Parser produces `ExternFn` AST nodes with typed parameters and return type. No function body — just a declaration.

2. **`ptr` type**: `TAG_PTR` in C runtime's `ATag` enum, `void* pval` in `AValue` union, `TyPtr` token in lexer, constructors `a_ptr()`, `a_ptr_null()`, `a_is_null()`. Builtin map entries `ptr.null` and `ptr.is_null`.

3. **Shim generation** (the key insight): Each `extern fn` generates two C declarations:
   - A raw C prototype: `extern int32_t abs(int32_t);`
   - An AValue shim wrapper: `AValue fn_abs(AValue __p0) { int32_t __result = abs((int32_t)__p0.ival); return a_int((int64_t)__result); }`
   
   Call sites use the shim, so the entire existing call resolution system (builtin check, known-fn check, closure fallback) works unchanged.

4. **Type marshalling**: `_ffi_c_type()` maps "a" types to C types, `_ffi_extract()` generates AValue→C extraction code, `_ffi_wrap()` generates C→AValue construction code. Full table: i8-i64/u8-u64 ↔ int*_t/uint*_t, f32/f64 ↔ float/double, str ↔ const char*, bool ↔ int, ptr ↔ void*, void.

5. **Rust VM parity**: `ExternFn` variant in `TopLevelKind`, parsed by Rust parser, handled as no-op in interpreter/compiler/checker/formatter. VM can parse files with `extern fn` without errors.

### Verification

- Three-stage bootstrap: 5,415 lines C, byte-identical across all stages
- 113 native tests passing (7 suites), 14 example programs (including `ffi_test.a`)
- `ffi_test.a` exercises `abs`, `atoi`, `strlen`, `getpid` — all correct

### Deferred

- Callback trampolines (passing closures as C function pointers)
- `extern struct` for direct C struct access
- Variadic C functions (`printf`, etc.)
- `#link` build integration

---

## v0.47 -- Memory Architecture

*Completed: April 2026*

The native runtime now has a **three-tier memory management system**: reference counting with ownership semantics, arena allocation infrastructure, and a mark-and-sweep garbage collector for cycle collection. The generated C code grew from 5,415 to 12,300 lines due to retain/release instrumentation, and the three-stage bootstrap still holds.

### Ownership Model

All expressions produce **owned values**. The code generator enforces this with three mechanisms:

1. **Zero-initialization**: Every local variable is initialized to `AValue {0}` to prevent undefined behavior when released.
2. **Retain on copy**: When a value is assigned from one variable to another (or passed to a function that stores it), `a_retain` increments the reference count.
3. **Release at scope exit**: At every function return (explicit and implicit), all local variables are released via `a_release`. Loop variables are released at the end of each iteration.

The assignment pattern uses a safe three-step protocol:
```c
{ AValue __old = target; target = new_value; a_release(__old); }
```
This ensures the old value isn't freed before the new value is computed (which may reference the old value, e.g., `arr = push(arr, x)`).

### Reference Counting (Phase 1-2)

- `a_release` hardened with `rc <= 0` guard to prevent double-free
- All collection constructors (`a_array_new`, `a_array_push`, `a_map_set`, etc.) retain values they store
- `a_array_get`, `a_map_get`, `a_unwrap`, `a_iterable` return retained (owned) values
- Runtime helpers (`a_str_concat`, `a_concat_n`, `a_str_join`) release intermediate values
- Cleanup blocks emitted at function exits, loop boundaries, and lambda returns

### Escape Analysis (Phase 3)

Analysis functions defined (`_ea_collect_idents`, `_escape_analysis`) that walk the AST to determine which variables' values can leave their scope (returned, captured, stored in collections). The context slot `escaping_vars` is prepared in `fn_ctx`. Full wiring deferred to avoid intermediate allocation overhead during self-compilation -- the analysis itself generates significant temporary data that, without arena allocation for the analysis pass, causes memory pressure.

### Arena Allocator (Phase 4)

Runtime API implemented:
- `a_arena_new(size)` / `a_arena_free(arena)` -- lifecycle
- `a_arena_alloc(arena, bytes)` -- bump allocation with auto-grow
- `a_arena_save(arena)` / `a_arena_restore(arena, pos)` -- scope checkpoints

Designed for loop bodies: save position before each iteration, restore after. Codegen integration deferred until escape analysis is fully wired (to know which allocations are safe to arena-allocate).

### Mark-and-Sweep GC (Phase 5)

Infrastructure for cycle collection:
- `GCNode` side-linked list tracks registered heap objects by type (`GC_STRING`, `GC_ARRAY`, `GC_MAP`, `GC_CLOSURE`)
- Root shadow stack (`a_gc_push_root` / `a_gc_pop_roots`) for marking reachable objects
- `gc_mark_value` recursively marks arrays, maps, and closure environments
- `gc_sweep` frees unreachable nodes, `a_gc_collect` provides the public API with adaptive threshold

The GC is opt-in -- not wired into every allocation. RC handles the common case; the GC API is available for programs that create closure cycles.

### Verification

- Three-stage bootstrap: 12,300 lines C, byte-identical across all stages
- 11/12 existing example programs match VM output (1 pre-existing try-block issue)
- 4 memory-specific test programs (basic, sharing, closures, loops) all passing
- All memory tests clean under AddressSanitizer (no use-after-free, double-free, buffer overflow)
- Self-compilation time: ~3.9s (native compiler compiling its own source)
- Requires `-Wl,-stack_size,0x4000000` (64MB stack) due to increased frame depth from cleanup blocks

### Files Modified

| File | Changes |
|------|---------|
| `c_runtime/runtime.h` | `GCNode`/`GCType` types, GC API declarations, `AArena` struct and API |
| `c_runtime/runtime.c` | RC hardening, owned-return convention, collection retain fixes, GC infrastructure, arena implementation (~1,440 lines, up from ~1,000) |
| `std/compiler/cgen.a` | Zero-init locals, retain/release at assignments, cleanup at returns/loops/lambdas, escape analysis stubs (~1,830 lines, up from ~1,530) |

C runtime: ~1,700 lines. cgen.a: ~1,830 lines. Generated C: 12,300 lines.

