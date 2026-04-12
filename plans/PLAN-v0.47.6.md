# v0.47.6 -- Verification

**Goal**: Comprehensively verify that the memory architecture implemented in
Phases 1-5 is correct, complete, and does not regress functionality. This
phase produces no new features -- it is pure testing, measurement, and
documentation.

**Prerequisite**: v0.47.1 and v0.47.2 must be complete. v0.47.3-5 should be
complete but can be verified incrementally.

---

## 1. Three-stage bootstrap

The most critical verification: the self-hosting compiler must still reach a
fixed point.

```bash
# Stage 1: VM compiles cgen.a to C
./target/release/a run std/compiler/cgen.a -- std/compiler/cgen.a > /tmp/c1.c

# Stage 2: Compile C1 to native, use native to compile cgen.a
gcc -O2 -o /tmp/native1 /tmp/c1.c c_runtime/runtime.c -lm
/tmp/native1 < std/compiler/cgen.a > /tmp/c2.c

# Stage 3: Compile C2 to native, use native to compile cgen.a
gcc -O2 -o /tmp/native2 /tmp/c2.c c_runtime/runtime.c -lm
/tmp/native2 < std/compiler/cgen.a > /tmp/c3.c

# Verify fixed point
diff /tmp/c1.c /tmp/c2.c
diff /tmp/c2.c /tmp/c3.c
```

All three C outputs must be byte-identical.

---

## 2. Existing test suite

Run all existing test programs through native compilation and verify output:

```bash
for f in examples/c_targets/*.a; do
  echo "=== Testing $f ==="
  ./target/release/a run std/compiler/cgen.a -- "$f" > /tmp/test.c
  gcc -O2 -o /tmp/test_native /tmp/test.c c_runtime/runtime.c -lm
  /tmp/test_native
done
```

Compare output against expected results (or against VM execution):

```bash
for f in examples/c_targets/*.a; do
  vm_out=$(./target/release/a run "$f" 2>&1)
  native_out=$(/tmp/test_native 2>&1)
  if [ "$vm_out" != "$native_out" ]; then
    echo "MISMATCH: $f"
  fi
done
```

---

## 3. Memory-specific test programs

Create targeted test programs in `examples/c_targets/` that exercise memory
management:

### `memory_basic.a`

Tests basic reference counting:
- Create strings, arrays, maps in local scope
- Let them go out of scope
- Verify no crash (double-free, use-after-free)

### `memory_sharing.a`

Tests aliased values:
- `let x = "hello"; let y = x` -- both should work after function returns
- Mutating collections after aliasing
- Passing values to functions and using them after the call

### `memory_closures.a`

Tests closure memory:
- Closures that capture local variables
- Calling closures after the capturing function returns
- Nested closures
- (If GC is enabled) Cyclic closure references

### `memory_loops.a`

Tests loop memory:
- Tight loop creating strings (should not OOM)
- Loop building an array incrementally
- Nested loops with temporaries

### `memory_stress.a`

Stress test:
- Allocate millions of small strings in a loop
- Build and discard large arrays
- Verify RSS stays bounded (not growing without limit)

---

## 4. Valgrind testing (if available)

On Linux, run test programs under Valgrind to detect:
- Memory leaks
- Use-after-free
- Double-free
- Uninitialized reads

```bash
gcc -g -O0 -o /tmp/test_debug /tmp/test.c c_runtime/runtime.c -lm
valgrind --leak-check=full --error-exitcode=1 /tmp/test_debug
```

On macOS, use `leaks` or AddressSanitizer:

```bash
gcc -g -O0 -fsanitize=address -o /tmp/test_asan /tmp/test.c c_runtime/runtime.c -lm
/tmp/test_asan
```

---

## 5. Performance measurement

### Before/after comparison

Measure compile-time (self-hosting) and runtime performance:

```bash
# Self-hosting compile time
time /tmp/native2 < std/compiler/cgen.a > /dev/null

# Runtime benchmark: string-heavy program
time /tmp/test_native  # run memory_stress.a
```

### Memory usage

```bash
# Peak RSS
/usr/bin/time -l /tmp/test_native  # macOS
/usr/bin/time -v /tmp/test_native  # Linux
```

Document before/after numbers for:
- Self-hosting compile time
- Peak RSS during self-hosting
- Peak RSS during memory_stress.a

---

## 6. Documentation updates

### `PLANNING.md`

Add v0.47 section documenting:
- What was implemented (refcounting, escape analysis, arenas, GC)
- Ownership model ("all expressions produce owned values")
- Performance characteristics

### `REFERENCE.md`

Update "Supported Features" to include memory management. Add a section
on the memory model visible to users (if any user-facing API exists).

### `README.md`

Update self-hosting description with new line count, mention memory
management as a feature.

### `plans/ROADMAP-v0.43-to-v0.52.md`

Mark v0.47 as complete with summary of what was delivered.

### `Cargo.toml`

Bump version to `0.47.0`.

---

## 7. Regression test script

Create a script that future development can run to verify memory correctness:

```bash
#!/bin/bash
# scripts/test_memory.sh
set -e
echo "Building native compiler..."
./target/release/a run std/compiler/cgen.a -- std/compiler/cgen.a > /tmp/c1.c
gcc -O2 -o /tmp/native1 /tmp/c1.c c_runtime/runtime.c -lm

echo "Three-stage bootstrap..."
/tmp/native1 < std/compiler/cgen.a > /tmp/c2.c
gcc -O2 -o /tmp/native2 /tmp/c2.c c_runtime/runtime.c -lm
/tmp/native2 < std/compiler/cgen.a > /tmp/c3.c
diff /tmp/c2.c /tmp/c3.c && echo "Bootstrap: PASS" || echo "Bootstrap: FAIL"

echo "Running memory tests..."
for f in examples/c_targets/memory_*.a; do
  ./target/release/a run std/compiler/cgen.a -- "$f" > /tmp/mem_test.c
  gcc -O2 -o /tmp/mem_test /tmp/mem_test.c c_runtime/runtime.c -lm
  /tmp/mem_test && echo "$f: PASS" || echo "$f: FAIL"
done
```

---

## Files Modified/Created

- `examples/c_targets/memory_basic.a` (new)
- `examples/c_targets/memory_sharing.a` (new)
- `examples/c_targets/memory_closures.a` (new)
- `examples/c_targets/memory_loops.a` (new)
- `examples/c_targets/memory_stress.a` (new)
- `PLANNING.md` (updated)
- `REFERENCE.md` (updated)
- `README.md` (updated)
- `plans/ROADMAP-v0.43-to-v0.52.md` (updated)
- `Cargo.toml` (version bump)

---

## Acceptance Criteria

- [ ] Three-stage bootstrap passes with all memory changes
- [ ] All existing test programs produce correct output
- [ ] Memory-specific test programs pass
- [ ] No memory errors under AddressSanitizer / Valgrind
- [ ] Performance is within acceptable bounds (no major regressions)
- [ ] Documentation updated
- [ ] Version bumped to 0.47.0
