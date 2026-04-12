# v0.40 -- Practical Standard Library

## Motivation

The language has a self-hosted compiler, metaprogramming, and structured concurrency. But if you sit down to write a real tool -- a static site generator, a data pipeline, a deploy script -- you hit walls immediately. You can't manipulate file paths safely, get the current time, hash a string, parse CSV, or render a template. These are the bread and butter of real-world programming.

v0.40 fills the practical gap with six new standard library modules. After this, "a" is a language you can actually use to build real things.

## New Rust-Side Builtins

Four new builtins implemented in the Rust runtime. These are thin wrappers around functionality that cannot be implemented in pure "a" (system clock, cryptographic hash algorithms).


| Builtin          | Signature   | Description                             |
| ---------------- | ----------- | --------------------------------------- |
| `time.now()`     | -> i64      | Current time as Unix epoch milliseconds |
| `time.sleep(ms)` | i64 -> void | Sleep for `ms` milliseconds             |
| `hash.sha256(s)` | str -> str  | SHA-256 hash, returned as lowercase hex |
| `hash.md5(s)`    | str -> str  | MD5 hash, returned as lowercase hex     |


### Rust changes

- `**src/builtins.rs**`: Implement `time.now`, `time.sleep`, `hash.sha256`, `hash.md5`. Register in `is_builtin()`.
- `**src/vm.rs**`: Forward the four new builtins to the builtin handler (same pattern as existing `fs.*`, `http.*` builtins).
- `**Cargo.toml**`: Add `sha2 = "0.10"` and `md-5 = "0.10"` dependencies (RustCrypto project, standard and well-audited).

`time.now` uses `std::time::SystemTime::UNIX_EPOCH.elapsed()`. `time.sleep` uses `std::thread::sleep`. Hash functions use the `Digest` trait from `sha2` and `md5` crates, converting output to hex via format.

## New Modules

### 1. `std/path.a` -- Path Manipulation

Pure "a" module. All string-based, Unix-oriented (matches the existing `fs.*` builtins).


| Function                      | Signature       | Description                                             |
| ----------------------------- | --------------- | ------------------------------------------------------- |
| `path.join(parts...)`         | str, str -> str | Join path segments with `/`, normalizing double slashes |
| `path.dirname(p)`             | str -> str      | Parent directory (`"a/b/c.txt"` -> `"a/b"`)             |
| `path.basename(p)`            | str -> str      | Final component (`"a/b/c.txt"` -> `"c.txt"`)            |
| `path.extension(p)`           | str -> str      | File extension (`"c.txt"` -> `"txt"`, `"c"` -> `""`)    |
| `path.stem(p)`                | str -> str      | Basename without extension (`"c.txt"` -> `"c"`)         |
| `path.normalize(p)`           | str -> str      | Collapse `//`, resolve `.` and `..` segments            |
| `path.is_absolute(p)`         | str -> bool     | Starts with `/`                                         |
| `path.segments(p)`            | str -> [str]    | Split into path components                              |
| `path.with_extension(p, ext)` | str, str -> str | Replace extension                                       |


~80 lines. Uses `str.split`, `str.join`, `str.ends_with`, `str.starts_with`, `str.slice`.

### 2. `std/datetime.a` -- Date and Time

Mixed module: calls `time.now()` and `time.sleep()` Rust builtins, implements formatting/parsing/arithmetic in "a".


| Function                                  | Signature       | Description                                                          |
| ----------------------------------------- | --------------- | -------------------------------------------------------------------- |
| `datetime.now()`                          | -> i64          | Current Unix epoch milliseconds                                      |
| `datetime.timestamp()`                    | -> i64          | Current Unix epoch seconds                                           |
| `datetime.sleep(ms)`                      | i64 -> void     | Sleep for milliseconds                                               |
| `datetime.from_parts(y, mo, d, h, mi, s)` | i64... -> i64   | Construct epoch ms from components                                   |
| `datetime.to_parts(epoch_ms)`             | i64 -> map      | `#{"year", "month", "day", "hour", "minute", "second", "ms"}`        |
| `datetime.format(epoch_ms, fmt)`          | i64, str -> str | Format: `%Y`, `%m`, `%d`, `%H`, `%M`, `%S`, `%F` (date), `%T` (time) |
| `datetime.iso(epoch_ms)`                  | i64 -> str      | ISO 8601 format (`2026-04-10T15:30:00Z`)                             |
| `datetime.add_ms(epoch_ms, ms)`           | i64, i64 -> i64 | Add milliseconds                                                     |
| `datetime.add_seconds(epoch_ms, s)`       | i64, i64 -> i64 | Add seconds                                                          |
| `datetime.add_minutes(epoch_ms, m)`       | i64, i64 -> i64 | Add minutes                                                          |
| `datetime.add_hours(epoch_ms, h)`         | i64, i64 -> i64 | Add hours                                                            |
| `datetime.add_days(epoch_ms, d)`          | i64, i64 -> i64 | Add days                                                             |
| `datetime.diff_ms(a, b)`                  | i64, i64 -> i64 | Difference in milliseconds                                           |
| `datetime.diff_seconds(a, b)`             | i64, i64 -> i64 | Difference in seconds                                                |


~200 lines. The date math (epoch <-> year/month/day) is non-trivial but well-documented (days since epoch algorithm with leap year handling).

### 3. `std/hash.a` -- Hashing

Thin "a" wrapper around the Rust `hash.*` builtins. Adds convenience functions.


| Function                 | Signature  | Description                                       |
| ------------------------ | ---------- | ------------------------------------------------- |
| `hash.sha256(s)`         | str -> str | SHA-256 hex digest                                |
| `hash.md5(s)`            | str -> str | MD5 hex digest                                    |
| `hash.sha256_file(path)` | str -> str | SHA-256 of file contents                          |
| `hash.md5_file(path)`    | str -> str | MD5 of file contents                              |
| `hash.quick(s)`          | str -> str | Fast hash for cache keys (uses md5, short output) |


~30 lines. The `*_file` variants just call `io.read_file` then the hash builtin.

### 4. `std/encoding.a` -- Encoding/Decoding

Pure "a" module. Lookup-table based encoding algorithms.


| Function                    | Signature  | Description                            |
| --------------------------- | ---------- | -------------------------------------- |
| `encoding.base64_encode(s)` | str -> str | Base64 encode                          |
| `encoding.base64_decode(s)` | str -> str | Base64 decode (returns Err on invalid) |
| `encoding.hex_encode(s)`    | str -> str | Hex encode (each byte -> 2 hex chars)  |
| `encoding.hex_decode(s)`    | str -> str | Hex decode                             |
| `encoding.url_encode(s)`    | str -> str | Percent-encode for URLs                |
| `encoding.url_decode(s)`    | str -> str | Percent-decode                         |


~180 lines. Base64 uses a 64-char lookup table and `char_code`/`from_code` for byte manipulation. URL encoding maps reserved characters to `%XX` hex escapes.

### 5. `std/csv.a` -- CSV Parsing and Generation

Pure "a" module. RFC 4180 compliant (quoted fields, embedded commas, embedded newlines).


| Function                                  | Signature           | Description                                                                     |
| ----------------------------------------- | ------------------- | ------------------------------------------------------------------------------- |
| `csv.parse(text)`                         | str -> [[str]]      | Parse CSV text to array of rows (each row is array of fields)                   |
| `csv.parse_records(text)`                 | str -> [map]        | Parse CSV with first row as headers, returns array of `#{"header": value}` maps |
| `csv.stringify(rows)`                     | [[str]] -> str      | Convert array of rows to CSV text                                               |
| `csv.stringify_records(records, headers)` | [map], [str] -> str | Convert array of maps to CSV with header row                                    |
| `csv.parse_row(line)`                     | str -> [str]        | Parse a single CSV line                                                         |
| `csv.escape_field(field)`                 | str -> str          | Quote/escape a field if needed                                                  |


~150 lines. The parser is a state machine with three states: field start, in quoted field, in unquoted field.

### 6. `std/template.a` -- String Templating

Pure "a" module. Mustache-style template rendering with variable substitution, conditionals, and iteration.


| Function                           | Signature       | Description                       |
| ---------------------------------- | --------------- | --------------------------------- |
| `template.render(tmpl, vars)`      | str, map -> str | Render template with variable map |
| `template.render_file(path, vars)` | str, map -> str | Read template file and render     |


Template syntax:

- `{{name}}` -- variable substitution from vars map
- `{{#if key}}...{{/if}}` -- conditional (truthy: non-empty string, non-zero, non-empty array, `true`)
- `{{#if key}}...{{#else}}...{{/if}}` -- conditional with else
- `{{#each items}}...{{/each}}` -- iterate over array, `{{.}}` for current item, `{{@index}}` for index
- `{{#each items}}...{{#else}}...{{/each}}` -- iterate with empty fallback

~200 lines. The renderer is a recursive parser that tokenizes the template into text/tag segments, then evaluates tags against the vars map.

## Test Plan

### `tests/test_path.a` (~20 tests)

- join: two segments, trailing slash, leading slash, multiple segments, empty
- dirname/basename: normal, nested, root, no dir, trailing slash
- extension/stem: normal, no ext, multiple dots, hidden files
- normalize: double slash, dot, dotdot, mixed
- is_absolute, segments, with_extension

### `tests/test_datetime.a` (~15 tests)

- now returns positive integer
- timestamp is now/1000
- to_parts round-trips with from_parts for known epoch
- format with %Y, %m, %d, %H, %M, %S, %F, %T
- iso format
- add_* and diff_* arithmetic
- sleep (verify elapsed time >= requested)

### `tests/test_hash.a` (~10 tests)

- sha256 known vectors (empty string, "abc", "hello world")
- md5 known vectors
- sha256_file / md5_file on a temp file
- quick returns consistent results
- deterministic (same input -> same output)

### `tests/test_encoding.a` (~18 tests)

- base64 encode/decode roundtrip
- base64 known vectors ("" -> "", "f" -> "Zg==", "fo" -> "Zm8=", "foo" -> "Zm9v", etc.)
- base64 decode invalid input returns error
- hex encode/decode roundtrip
- hex known vectors
- url encode/decode roundtrip
- url encode preserves alphanumerics
- url encode escapes special characters (space, &, =, ?, etc.)

### `tests/test_csv.a` (~15 tests)

- parse simple (no quotes)
- parse quoted fields
- parse fields with commas
- parse fields with newlines
- parse empty fields
- parse_records with headers
- stringify roundtrip
- stringify_records roundtrip
- escape_field quoting rules
- parse_row single line

### `tests/test_template.a` (~15 tests)

- variable substitution
- missing variable (empty string or preserved)
- if truthy / falsy
- if/else
- each over array
- each with @index
- each with else (empty array)
- nested each
- mixed tags
- render_file
- no tags (passthrough)
- escaped braces

## Example: `examples/site_gen.a`

A minimal static site generator that exercises all six new modules. ~80 lines.

Reads markdown-like content files, applies a template, generates HTML pages with timestamps, checksums, and file path handling. Demonstrates that "a" can now build something real.

```a
use std.path
use std.datetime
use std.hash
use std.template
use std.csv

fn main() -> void effects [io] {
  let pages = csv.parse_records(io.read_file("site/pages.csv"))
  let tmpl = io.read_file("site/template.html")

  for page in pages {
    let vars = map.merge(page, #{
      "generated": datetime.iso(datetime.now()),
      "checksum": hash.quick(map.get(page, "body"))
    })
    let html = template.render(tmpl, vars)
    let out = path.join("build", path.with_extension(map.get(page, "slug"), "html"))
    io.write_file(out, html)
    println("wrote {out}")
  }
}
```

## Implementation Order

1. **Rust-side builtins** -- `time.now`, `time.sleep`, `hash.sha256`, `hash.md5` in `src/builtins.rs` + `src/vm.rs` + `Cargo.toml` deps
2. `**std/path.a`** + `tests/test_path.a` -- pure "a", no deps
3. `**std/encoding.a**` + `tests/test_encoding.a` -- pure "a", no deps
4. `**std/datetime.a**` + `tests/test_datetime.a` -- uses `time.now`/`time.sleep` builtins
5. `**std/hash.a**` + `tests/test_hash.a` -- uses `hash.sha256`/`hash.md5` builtins
6. `**std/csv.a**` + `tests/test_csv.a` -- pure "a"
7. `**std/template.a**` + `tests/test_template.a` -- pure "a"
8. `**examples/site_gen.a**` -- demo exercising all modules
9. **Update `PLANNING.md` and `REFERENCE.md`**

## TODO

- Add Rust builtins: time.now, time.sleep, hash.sha256, hash.md5
- Create std/path.a + tests/test_path.a
- Create std/encoding.a + tests/test_encoding.a
- Create std/datetime.a + tests/test_datetime.a
- Create std/hash.a + tests/test_hash.a
- Create std/csv.a + tests/test_csv.a
- Create std/template.a + tests/test_template.a
- Create examples/site_gen.a
- Update PLANNING.md + REFERENCE.md

