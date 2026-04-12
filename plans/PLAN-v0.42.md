# v0.42 -- The Bootstrap

## Motivation

The code generator (`cgen.a`) compiles "a" programs to C. It is itself written in "a", using the self-hosted parser (`parser.a`), lexer (`lexer.a`), and AST (`ast.a`) -- all also written in "a".

If cgen can compile itself and its dependencies to C, and gcc compiles that C to a native binary, and that native binary produces correct output... then "a" exists without Rust. The compiler compiles itself. The chain is complete.

## What we have

- `cgen.a` (403 lines) -- C code generator, written in "a"
- `parser.a` (1526 lines) -- recursive descent parser, written in "a"
- `lexer.a` (566 lines) -- tokenizer, written in "a"
- `ast.a` (308 lines) -- AST node constructors, written in "a"
- `c_runtime/runtime.c` (~700 lines) -- C runtime library

Total: ~2,800 lines of "a" that need to compile themselves.

## Feasibility analysis

The four compiler files use NONE of the hard unsupported features:
- No closures / lambdas
- No pattern matching (match expressions)
- No higher-order functions (map, filter, reduce)
- No try/catch or ? operator
- No pipe operator, spread, or destructuring
- No type declarations (ty)

They use only: functions, recursion, if/else, while, for..in, let/let mut, arrays, maps, string operations, and a small set of builtins. All of this is already in the code generator.

## What needs to be added

### 1. Module inlining in cgen (`use` support)

The code generator currently ignores `UseDecl` nodes. For the bootstrap, it needs to:

1. When encountering `use std.compiler.parser`, load `std/compiler/parser.a`
2. Parse it and recursively handle its own `use` declarations
3. Emit all its functions with names prefixed by the module namespace
4. Track which functions belong to which module for intra-module call resolution

**Name resolution rules:**
- Function definition `fn parse(...)` in module `parser` → emits as `AValue fn_parser_parse(...)`
- Intra-module call `parse_program(...)` in parser → emits as `fn_parser_parse_program(...)`
- Cross-module call `lexer.lex(...)` → emits as `fn_lexer_lex(...)` (already works via FieldAccess)
- Builtin calls → map to `a_*` functions (unchanged)

The code generator needs a "current module prefix" context passed through emit_fn/emit_stmt/emit_expr.

### 2. Three missing builtins

`is_alpha`, `is_alnum`, `is_digit` are used by `lexer.a` but missing from both the C runtime and the builtin map.

In `c_runtime/runtime.c`:
```c
AValue a_is_alpha(AValue v) {
    if (v.tag == TAG_STRING && v.sval->len > 0)
        return a_bool(isalpha((unsigned char)v.sval->data[0]));
    return a_bool(0);
}
// similar for is_alnum, is_digit
```

In `_builtin_map()`: add `"is_alpha": "a_is_alpha"`, etc.

### 3. String indexing

The lexer does `src[i]` where `src` is a string. The code generator emits `a_array_get(src, idx)`, but the C runtime's `a_array_get` only handles arrays. Fix: make it also handle strings by returning a single-character string.

```c
AValue a_array_get(AValue arr, AValue idx) {
    if (arr.tag == TAG_STRING && idx.tag == TAG_INT) {
        int i = (int)idx.ival;
        if (i < 0 || i >= arr.sval->len) return a_void();
        return a_string_len(arr.sval->data + i, 1);
    }
    // ... existing array logic ...
}
```

## Architecture

```
cgen.a + parser.a + lexer.a + ast.a  (source, written in "a")
            |
            v  [a run std/compiler/cgen.a -- std/compiler/cgen.a]
            |  (code generator running on Rust VM, with module inlining)
            v
        ac_bootstrap.c  (generated C source, ~all compiler functions)
            |
            v  [gcc ac_bootstrap.c c_runtime/runtime.c -o ac -O2]
            |
            v
           ac  (native binary -- the "a" compiler, no Rust needed)
            |
            v  [./ac program.a > program.c]
            |
            v
        program.c  (native output, identical to VM-hosted cgen output)
```

## Validation

The bootstrap is correct if and only if:

1. `a run std/compiler/cgen.a -- examples/c_targets/fib.a` produces output X
2. `./ac examples/c_targets/fib.a` produces identical output X
3. This holds for ALL milestone test programs

Stronger test (fixed-point):
4. `./ac std/compiler/cgen.a > ac2.c` -- the native compiler compiles itself
5. `diff ac_bootstrap.c ac2.c` -- the output is identical (fixed point reached)

If step 5 passes, the bootstrap is complete. The compiler is a fixed point of itself.

## Implementation order

1. Add `is_alpha`, `is_alnum`, `is_digit` to C runtime + builtin map
2. Fix string indexing in `a_array_get`
3. Add module inlining to `cgen.a` (the `use` handler)
4. Test: generate C from cgen.a itself (with all deps inlined)
5. Compile with gcc
6. Run the native binary on all milestone tests
7. Fixed-point test: native compiler compiles itself, output matches
8. Create `tests/test_bootstrap.sh`
9. Update PLANNING.md

## File changes

```
c_runtime/runtime.h     -- add is_alpha, is_alnum, is_digit declarations
c_runtime/runtime.c     -- implement the three builtins + fix string indexing
std/compiler/cgen.a     -- add module inlining (UseDecl handling, module prefix context)
tests/test_bootstrap.sh -- automated bootstrap validation
```

## TODO

- [ ] Add is_alpha/is_alnum/is_digit to C runtime + builtin map
- [ ] Fix string indexing in a_array_get
- [ ] Add module inlining to cgen.a (use handling + namespace prefixing)
- [ ] Bootstrap: cgen compiles itself to C
- [ ] Compile ac with gcc, verify all milestone tests pass
- [ ] Fixed-point test: ac compiles itself, output matches
- [ ] Test script + update PLANNING.md
