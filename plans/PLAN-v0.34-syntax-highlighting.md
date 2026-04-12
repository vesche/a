---
name: v0.34 Syntax Highlighting
overview: Create a VS Code/Cursor extension that provides syntax highlighting for "a" language files (.a), using a TextMate grammar to colorize keywords, strings, comments, numbers, types, operators, and interpolation.
todos:
  - id: extension-manifest
    content: Create editors/vscode/package.json with language contribution, file associations, and grammar reference
    status: completed
  - id: language-config
    content: Create editors/vscode/language-configuration.json with comments, brackets, auto-close, folding
    status: completed
  - id: tmgrammar
    content: Create editors/vscode/syntaxes/a.tmLanguage.json with full TextMate grammar (comments, keywords, types, strings, interpolation, raw strings, heredocs, numbers, operators, functions)
    status: completed
  - id: install-and-verify
    content: Install the extension in Cursor and verify highlighting works on example .a files
    status: completed
  - id: planning-update
    content: Update PLANNING.md with v0.34 milestone
    status: completed
isProject: false
---

# v0.34 -- Syntax Highlighting Extension

A VS Code / Cursor TextMate grammar extension for `.a` files, packaged as `editors/vscode/` in the repo.

## Structure

```
editors/vscode/
  package.json          -- extension manifest
  language-configuration.json  -- brackets, comments, auto-close
  syntaxes/a.tmLanguage.json   -- TextMate grammar
```

No build step, no bundler, no runtime JS -- pure declarative JSON. Cursor/VS Code loads it directly via "Developer: Install Extension from Location..." or symlink into `~/.vscode/extensions/`.

## TextMate Grammar Scope Map

All scopes follow [VS Code naming conventions](https://macromates.com/manual/en/language_grammars#naming-conventions) so existing color themes apply automatically.

- **Comments**: `; ...` -- `comment.line.semicolon.a`
- **Keywords**: `fn`, `let`, `mut`, `if`, `else`, `match`, `for`, `in`, `while`, `break`, `continue`, `ret`, `use`, `try`, `pub`, `mod`, `ty`, `effects`, `pre`, `post`, `where` -- `keyword.control.a` / `keyword.declaration.a` / `storage.modifier.a`
- **Boolean literals**: `true`, `false` -- `constant.language.boolean.a`
- **Type keywords**: `i8` `i16` `i32` `i64` `u8` `u16` `u32` `u64` `f32` `f64` `bool` `str` `bytes` `void` -- `support.type.primitive.a`
- **Numbers**: integer and float -- `constant.numeric.integer.a` / `constant.numeric.float.a`
- **Strings**: `"..."` with escape highlighting -- `string.quoted.double.a`
  - Escape sequences (`\n`, `\t`, `\\`, `\"`, `\{`, `\}`, `\r`) -- `constant.character.escape.a`
  - **Interpolation**: `{expr}` inside strings -- `meta.interpolation.a` with `punctuation.section.interpolation.begin/end.a`, containing embedded `source.a` patterns
- **Raw strings**: `r"..."` and `r#"..."#` -- `string.quoted.other.raw.a` (no escape/interpolation patterns inside)
- **Heredocs**: triple-backtick blocks -- `string.quoted.other.heredoc.a`
- **Operators**: `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `&&`, `||`, `|>`, `->`, `=>`, `!`, `=` -- `keyword.operator.a`
- **Punctuation**: `(`, `)`, `{`, `}`, `[`, `]`, `,`, `:`, `.`, `#`, `?`, `...`, `|`, `_` -- various `punctuation.*` scopes
- **Function definitions**: `fn name(` -- captures name as `entity.name.function.a`
- **Function calls**: `name(` -- `entity.name.function.call.a` (optional, lower priority)
- **Identifiers**: default -- no special scope (inherits editor foreground)

### Interpolation handling

String interpolation (`"hello {name}"`) requires nested grammar rules. The `string.quoted.double.a` pattern uses `begin`/`end` with nested `patterns` array. When `{` is encountered inside a string (not preceded by `\`), it opens a `meta.interpolation.a` scope that re-includes `#expression` patterns until the matching `}`.

### Raw string hash variants

`r#"..."#` uses a TextMate `begin`/`end` with `\br#"` and `"#\b?` as delimiters. For `r##"..."##` we add a second pattern. Supporting up to 2-3 hash levels covers all practical use.

## Language Configuration ([language-configuration.json](editors/vscode/language-configuration.json))

- Comment toggle: `; `
- Brackets: `()`, `{}`, `[]`
- Auto-closing pairs: `()`, `{}`, `[]`, `""`, `` `` ``
- Surrounding pairs: same
- Folding: brace-based `{` ... `}`
- Word pattern: `[a-zA-Z_][a-zA-Z0-9_]*`

## Installation

Two options (documented in a short README in the extension folder):

1. **Symlink**: `ln -s /path/to/editors/vscode ~/.cursor/extensions/a-lang`
2. **Command**: VS Code/Cursor "Developer: Install Extension from Location..." pointing at `editors/vscode/`

No marketplace publishing needed.

## File Associations

- Extension: `.a`
- Language ID: `a`
- Aliases: `A`, `a-lang`

## Scope of changes

- `editors/vscode/package.json`: ~40 lines
- `editors/vscode/language-configuration.json`: ~30 lines
- `editors/vscode/syntaxes/a.tmLanguage.json`: ~250 lines
- `PLANNING.md`: ~20 lines documenting v0.34
- Total: ~340 lines, all new files in a new directory
