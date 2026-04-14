# Vendored C Libraries

Third-party C source files bundled in `c_runtime/`. Each library is compiled directly into the native binary alongside `runtime.c`.

## Policy

- **Security patches**: Applied immediately at any version boundary.
- **Feature updates**: At milestone boundaries, after full test suite passes.
- **License requirement**: MIT, Apache 2.0, public domain, or zlib. No GPL.
- **Verification**: `scripts/vendor_check.sh` validates checksums against this manifest. Runs in CI.

## Libraries

### SQLite

| Field | Value |
|-------|-------|
| **Version** | 3.48.0 |
| **Upstream** | https://www.sqlite.org/2025/sqlite-amalgamation-3480000.zip |
| **License** | Public domain |
| **Date vendored** | 2025-01-14 |
| **Build flags** | `-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION` |

**Files and checksums (SHA-256):**

```
5fdc8109b60ea295a2ffe526bcce47812c501db7a41b4b4baeeea921e6f9bfc8  sqlite3.c
78d2dbfa4ebd6146f26ceb735702685af0e77b83ad3df69e4904786924c6899c  sqlite3.h
```

**What it provides:** `db.open`, `db.exec`, `db.query`, `db.close` builtins for embedded SQL database access.

**Update procedure:**
1. Download the new amalgamation from https://www.sqlite.org/download.html
2. Replace `sqlite3.c` and `sqlite3.h`
3. Update the version, date, and checksums in this file
4. Run `scripts/vendor_check.sh` to verify
5. Run `bash scripts/test_memory.sh` to verify all tests pass
6. Check that `SQLITE_FLAGS` in `build.sh`, `src/cli.a`, and `scripts/test_memory.sh` are still correct

### miniz

| Field | Value |
|-------|-------|
| **Version** | 3.1.1 |
| **Upstream** | https://github.com/richgel999/miniz/releases/tag/3.1.1 |
| **License** | MIT |
| **Date vendored** | 2026-04-13 |
| **Build flags** | None (compiled as-is) |

**Files and checksums (SHA-256):**

```
7487d4c8cd761b951a99d182672bb3badfa72a5ad5760b70b392fafa95223657  miniz.c
a33e04eb7975ab0c7a391ca48bec5336a7bf3a4b2857cfcff3ee02a44766061f  miniz.h
```

**What it provides:** `compress.deflate`, `compress.inflate`, `compress.gzip`, `compress.gunzip` builtins for zlib-compatible compression and gzip encoding.

**Update procedure:**
1. Download the new release from https://github.com/richgel999/miniz/releases
2. Replace `miniz.c` and `miniz.h`
3. Update the version, date, and checksums in this file
4. Run `scripts/vendor_check.sh` to verify
5. Run `./a test tests/native/` to verify all tests pass

### stb_image + stb_image_write

| Field | Value |
|-------|-------|
| **Version** | stb_image v2.30, stb_image_write v1.16 |
| **Upstream** | https://github.com/nothings/stb |
| **License** | Public domain / MIT |
| **Date vendored** | 2026-04-14 |
| **Build flags** | None (compiled via `stb_impl.c` with `STB_IMAGE_IMPLEMENTATION` / `STB_IMAGE_WRITE_IMPLEMENTATION`) |

**Files and checksums (SHA-256):**

```
594c2fe35d49488b4382dbfaec8f98366defca819d916ac95becf3e75f4200b3  stb_image.h
cbd5f0ad7a9cf4468affb36354a1d2338034f2c12473cf1a8e32053cb6914a05  stb_image_write.h
3bdad46da43a131fc3de1520ae8e3aa839b5b800c90409bc10255dcb58410ec6  stb_impl.c
```

**What it provides:** `image.load`, `image.decode`, `image.save`, `image.encode`, `image.width`, `image.height`, `image.resize`, `image.pixels` builtins for image decoding (PNG, JPEG, BMP, GIF), encoding (PNG, BMP, JPEG), and manipulation.

**Update procedure:**
1. Download updated headers from https://github.com/nothings/stb
2. Replace `stb_image.h` and `stb_image_write.h`
3. Update the version, date, and checksums in this file
4. Run `scripts/vendor_check.sh` to verify
5. Run `./a test tests/native/` to verify all tests pass
