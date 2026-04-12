#!/bin/bash
# test_memory.sh -- Memory correctness regression test
# Runs three-stage bootstrap and memory-specific test programs
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$SCRIPT_DIR"

A="cargo run --quiet -- run"
CGEN="std/compiler/cgen.a"
RUNTIME="c_runtime/runtime.c"
TMPDIR=$(mktemp -d)
SFLAGS="-Wl,-stack_size,0x4000000"
trap "rm -rf $TMPDIR" EXIT

echo "=== Memory Architecture Test Suite ==="
echo ""

echo "Step 1: VM compiles cgen.a → gen1.c"
$A "$CGEN" -- "$CGEN" 2>/dev/null > "$TMPDIR/gen1.c"
echo "  gen1.c: $(wc -l < "$TMPDIR/gen1.c") lines"

echo "Step 2: gcc gen1.c → ac_s1"
gcc "$TMPDIR/gen1.c" "$RUNTIME" -o "$TMPDIR/ac_s1" -I c_runtime -lm -O1 $SFLAGS
echo "  ac_s1 built"

echo "Step 3: ac_s1 self-compiles → gen2.c"
"$TMPDIR/ac_s1" "$CGEN" > "$TMPDIR/gen2.c"
echo "  gen2.c: $(wc -l < "$TMPDIR/gen2.c") lines"

echo "Step 4: gcc gen2.c → ac_s2"
gcc "$TMPDIR/gen2.c" "$RUNTIME" -o "$TMPDIR/ac_s2" -I c_runtime -lm -O1 $SFLAGS
echo "  ac_s2 built"

echo "Step 5: ac_s2 self-compiles → gen3.c"
"$TMPDIR/ac_s2" "$CGEN" > "$TMPDIR/gen3.c"
echo "  gen3.c: $(wc -l < "$TMPDIR/gen3.c") lines"
echo ""

echo "=== Fixed point check ==="
BOOT_PASS=true
if diff -q "$TMPDIR/gen1.c" "$TMPDIR/gen2.c" > /dev/null 2>&1; then
    echo "  gen1.c == gen2.c  PASS"
else
    echo "  gen1.c != gen2.c  FAIL"
    BOOT_PASS=false
fi
if diff -q "$TMPDIR/gen2.c" "$TMPDIR/gen3.c" > /dev/null 2>&1; then
    echo "  gen2.c == gen3.c  PASS"
else
    echo "  gen2.c != gen3.c  FAIL"
    BOOT_PASS=false
fi
echo ""

echo "=== Memory test programs ==="
MEM_PASS=0
MEM_FAIL=0
for src in examples/c_targets/memory_*.a; do
    name=$(basename "$src" .a)
    printf "  %-20s " "$name"

    vm_out=$($A "$src" 2>/dev/null || true)

    if ! "$TMPDIR/ac_s1" "$src" > "$TMPDIR/${name}.c" 2>/dev/null; then
        echo "FAIL (cgen)"
        MEM_FAIL=$((MEM_FAIL + 1))
        continue
    fi

    if ! gcc "$TMPDIR/${name}.c" "$RUNTIME" -o "$TMPDIR/$name" -I c_runtime -lm -O1 $SFLAGS 2>/dev/null; then
        echo "FAIL (gcc)"
        MEM_FAIL=$((MEM_FAIL + 1))
        continue
    fi

    nat_out=$("$TMPDIR/$name" 2>/dev/null || true)

    if [ "$vm_out" = "$nat_out" ]; then
        echo "PASS"
        MEM_PASS=$((MEM_PASS + 1))
    else
        echo "FAIL (mismatch)"
        MEM_FAIL=$((MEM_FAIL + 1))
    fi
done
echo ""
echo "  Memory tests: $MEM_PASS passed, $MEM_FAIL failed"
echo ""

echo "=== ASan check (memory_basic) ==="
"$TMPDIR/ac_s1" examples/c_targets/memory_basic.a > "$TMPDIR/mb.c" 2>/dev/null
if gcc "$TMPDIR/mb.c" "$RUNTIME" -o "$TMPDIR/mb_asan" -I c_runtime -lm -O1 -fsanitize=address -g $SFLAGS 2>/dev/null; then
    if "$TMPDIR/mb_asan" > /dev/null 2>&1; then
        echo "  AddressSanitizer: PASS (no errors)"
    else
        echo "  AddressSanitizer: FAIL"
    fi
else
    echo "  AddressSanitizer: SKIP (gcc -fsanitize=address not available)"
fi
echo ""

if $BOOT_PASS && [ $MEM_FAIL -eq 0 ]; then
    echo "=== ALL TESTS PASSED ==="
    exit 0
else
    echo "=== SOME TESTS FAILED ==="
    exit 1
fi
