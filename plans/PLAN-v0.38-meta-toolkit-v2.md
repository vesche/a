---
name: v0.38 Meta Toolkit
overview: Complete the v0.38 metaprogramming toolkit by running tests and adding the PLANNING.md milestone -- all files are already written.
todos:
  - id: run-tests
    content: Run tests/test_meta.a and examples/gen_tests.a, fix any failures
    status: in_progress
  - id: planning-md
    content: Add v0.38 milestone entry to PLANNING.md
    status: pending
isProject: false
---

# v0.38 Meta Toolkit -- Remaining Work

## Status

All source files are already written from before the crash:
- `std/meta.a` (241 lines) -- complete
- `examples/gen_tests.a` (47 lines) -- complete
- `tests/test_meta.a` (232 lines) -- written but not validated

## Remaining tasks

### 1. Run tests and fix any failures

Run `tests/test_meta.a` and verify all 24 tests pass. If any fail due to AST builder mismatches, runtime errors, or parser/emitter edge cases, fix them in `tests/test_meta.a` or `std/meta.a` as needed.

Also verify `examples/gen_tests.a` runs correctly against an example file like `examples/math.a`.

### 2. Add v0.38 milestone to PLANNING.md

Append a new section at the end of [PLANNING.md](PLANNING.md) following the existing format (see the v0.37 entry ending at line 832). The entry should cover:

- Title: `## v0.38 -- Metaprogramming Toolkit (std/meta.a)`
- Completion date
- Description of the module and its purpose
- API summary (core, analysis, transform, codegen)
- Changes checklist (`std/meta.a`, `examples/gen_tests.a`, `tests/test_meta.a`)
