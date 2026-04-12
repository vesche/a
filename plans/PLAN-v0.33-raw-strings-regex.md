---
name: v0.33 Raw Strings Heredocs Regex
overview: Add raw strings (r"..."), heredocs (triple-backtick multiline strings), and regex builtins to both the Rust and self-hosted compilers, eliminating escape hell for code generation and enabling real text processing.
todos:
  - id: raw-strings-rust
    content: Add raw string lexing (r"..." and r#"..."#) to src/lexer.rs
    status: completed
  - id: heredoc-rust
    content: Add heredoc lexing (triple-backtick) to src/lexer.rs
    status: completed
  - id: raw-strings-self-hosted
    content: Add raw string and heredoc lexing to std/compiler/lexer.a
    status: completed
  - id: regex-builtins
    content: Add regex crate to Cargo.toml and implement regex.* builtins in src/builtins.rs
    status: completed
  - id: test-raw-strings
    content: "Write tests/test_raw_strings.a: raw strings, heredocs, eval integration"
    status: completed
  - id: test-regex
    content: "Write tests/test_regex.a: match, find_all, replace, split, captures"
    status: completed
  - id: build-and-verify
    content: Build, run all tests, fix any issues
    status: completed
  - id: planning-update-v33
    content: Update PLANNING.md with v0.33 milestone
    status: completed
isProject: false
---

# v0.33 -- Raw Strings, Heredocs, and Regex

## Raw Strings

Two forms, both producing `TokenKind::StringLit` (no new AST or compiler changes needed):

- **`r"..."`** -- no escapes, no interpolation. Content is literal until closing `"`.
- **`r#"..."#`** -- can contain `"`. Closes on `"#` (or `"##` for `r##"..."##`, etc.).

```mermaid
flowchart LR
    RawStr["r#\"hello {world}\"#"] --> Lexer["Rust Lexer"]
    Lexer --> Token["StringLit('hello {world}')"]
    Token --> Same["Same parser/compiler path as regular strings"]
```

### Rust lexer -- [src/lexer.rs](src/lexer.rs)

Add `'r'` arm in `next_token` **before** the general `is_ascii_alphabetic` arm (~line 174):

```rust
'r' => {
    if self.peek() == Some('"') {
        self.advance(); // consume "
        self.lex_raw_string(start, 0)
    } else if self.peek() == Some('#') {
        // count hashes, check for "
        // if found: lex_raw_string(start, num_hashes)
        // else: backtrack, lex_ident_or_keyword(start)
    } else {
        self.lex_ident_or_keyword(start)
    }
}
```

New method `lex_raw_string(&mut self, start: usize, num_hashes: usize) -> AResult<Token>`:
- If `num_hashes == 0`: scan until next `"`, no escape processing
- If `num_hashes > 0`: scan until `"` followed by exactly `num_hashes` `#` chars
- Return `TokenKind::StringLit(s)` -- same token type, parser/compiler unchanged

### Self-hosted lexer -- [std/compiler/lexer.a](std/compiler/lexer.a)

In the `lex` function, before the identifier branch (`is_id_start(c)`), check if `c == "r"` and `chars[pos+1] == "\""` (or `"#"`). If so, consume and scan raw content. Emit `"Str"` token -- same as regular strings.

## Heredocs (Triple-Backtick Strings)

Syntax:

````
let code = ```
fn main() {
  println("hello")
}
```
````

### Rust lexer

`` ` `` is currently an unknown character error. Add a `'`'` arm that checks for three backticks:

```rust
'`' => {
    if self.peek() == Some('`') && self.peek_at(1) == Some('`') {
        self.advance(); self.advance(); // consume 2nd and 3rd `
        self.lex_heredoc(start)
    } else {
        Err(AError::lex("unexpected character '`'", ...))
    }
}
```

`lex_heredoc`: scan until next `` ``` ``. Strip leading newline after opener, strip trailing newline before closer. No escape processing. Return `StringLit(s)`.

Need to add `peek_at(n)` helper (peek N chars ahead) -- or just use `self.src.get(self.pos + n)`.

### Self-hosted lexer

Same pattern: when `c == "\`"`, check for two more backticks, then scan until closing triple-backtick. Emit `"Str"` token.

## Regex Builtins

Add `regex` crate to [Cargo.toml](Cargo.toml):

```toml
regex = "1"
```

Add to [src/builtins.rs](src/builtins.rs) `call_builtin` match:

- **`regex.match(pattern, text)`** -- returns first match as map `#{"match": str, "start": int, "end": int}` or `false`
- **`regex.is_match(pattern, text)`** -- returns `bool`
- **`regex.find_all(pattern, text)`** -- returns array of match strings
- **`regex.replace(pattern, text, replacement)`** -- returns new string
- **`regex.replace_all(pattern, text, replacement)`** -- replace all occurrences
- **`regex.split(pattern, text)`** -- returns array of parts
- **`regex.captures(pattern, text)`** -- returns array of capture group strings (index 0 = full match)

Register all names in `is_builtin` match.

## Test Suites

### `tests/test_raw_strings.a` (~20 tests)

- Raw string basics: `r"hello"`, `r"no \n escape"`, `r"braces { } ok"`
- Raw string with quotes: `r#"she said "hi""#`
- Heredoc basics: multiline content, braces, quotes
- Heredoc with code: full "a" program as heredoc, pass to `eval`
- Raw string in eval: `eval(r"1 + 2")` returns 3
- Edge cases: empty raw string, empty heredoc, raw string with backslashes

### `tests/test_regex.a` (~15 tests)

- `regex.is_match`: digit pattern, email-like pattern
- `regex.match`: extract first match with position
- `regex.find_all`: find all numbers in text
- `regex.replace` / `regex.replace_all`: substitution
- `regex.split`: split on pattern
- `regex.captures`: capture groups
- Error handling: invalid regex pattern

## Estimated Scale

- `src/lexer.rs`: ~50 lines (raw string + heredoc lexing)
- `src/builtins.rs`: ~80 lines (regex builtins)
- `std/compiler/lexer.a`: ~40 lines (raw string + heredoc in self-hosted)
- `tests/test_raw_strings.a`: ~120 lines
- `tests/test_regex.a`: ~100 lines
- Cargo.toml: 1 line
- PLANNING.md: ~25 lines
- Total: ~420 lines
