#!/bin/bash
# test_memory.sh -- Memory correctness + native surface regression test
# Runs three-stage bootstrap, memory tests, example tests, and std module tests
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$SCRIPT_DIR"

A="cargo run --quiet -- run"
CGEN="std/compiler/cgen.a"
RUNTIME="c_runtime/runtime.c"
WORK=$(mktemp -d)
SFLAGS="-Wl,-stack_size,0x8000000"
trap "rm -rf $WORK" EXIT

echo "=== v0.48 Test Suite ==="
echo ""

echo "Step 1: VM compiles cgen.a → gen1.c"
$A "$CGEN" -- "$CGEN" 2>/dev/null > "$WORK/gen1.c"
echo "  gen1.c: $(wc -l < "$WORK/gen1.c") lines"

echo "Step 2: gcc gen1.c → ac1"
gcc "$WORK/gen1.c" "$RUNTIME" -o "$WORK/ac1" -I c_runtime -lm -O1 $SFLAGS
echo "  ac1 built"

echo "Step 3: ac1 self-compiles → gen2.c"
"$WORK/ac1" "$CGEN" > "$WORK/gen2.c"
echo "  gen2.c: $(wc -l < "$WORK/gen2.c") lines"
echo ""

echo "=== Fixed point check ==="
if diff -q "$WORK/gen1.c" "$WORK/gen2.c" > /dev/null 2>&1; then
    echo "  gen1.c == gen2.c  PASS"
else
    echo "  gen1.c != gen2.c  FAIL"
    exit 1
fi
echo ""

echo "=== Example tests ==="
PASS=0; FAIL=0
for src in examples/c_targets/arrays.a examples/c_targets/closures.a examples/c_targets/strings.a examples/c_targets/memory_*.a; do
    name=$(basename "$src" .a)
    vm_out=$($A "$src" 2>/dev/null || true)
    if "$WORK/ac1" "$src" > "$WORK/${name}.c" 2>/dev/null && \
       gcc "$WORK/${name}.c" "$RUNTIME" -o "$WORK/$name" -I c_runtime -lm -O1 $SFLAGS 2>/dev/null; then
        nat_out=$("$WORK/$name" 2>/dev/null || true)
        if [ "$vm_out" = "$nat_out" ]; then
            printf "  %-25s PASS\n" "$name"; PASS=$((PASS + 1))
        else
            printf "  %-25s FAIL (mismatch)\n" "$name"; FAIL=$((FAIL + 1))
        fi
    else
        printf "  %-25s FAIL (compile)\n" "$name"; FAIL=$((FAIL + 1))
    fi
done
echo "  $PASS passed, $FAIL failed"
echo ""

echo "=== Std module compilation ==="
SPASS=0; SFAIL=0
for mod in path testing datetime math strings csv re template cli encoding lexer hash; do
    echo "use std.$mod" > "$WORK/stdmod.a"
    echo 'fn main() { println("ok") }' >> "$WORK/stdmod.a"
    if "$WORK/ac1" "$WORK/stdmod.a" > "$WORK/stdmod.c" 2>/dev/null && \
       [ "$(wc -l < "$WORK/stdmod.c")" -gt 0 ] && \
       gcc "$WORK/stdmod.c" "$RUNTIME" -o "$WORK/stdmod" -I c_runtime -lm -O1 $SFLAGS 2>/dev/null; then
        out=$("$WORK/stdmod" 2>/dev/null)
        if [ "$out" = "ok" ]; then
            printf "  %-15s PASS\n" "$mod"; SPASS=$((SPASS + 1))
        else
            printf "  %-15s FAIL (run)\n" "$mod"; SFAIL=$((SFAIL + 1))
        fi
    else
        printf "  %-15s FAIL (compile)\n" "$mod"; SFAIL=$((SFAIL + 1))
    fi
done
echo "  $SPASS passed, $SFAIL failed"
echo ""

echo "=== Summary ==="
echo "  Bootstrap:     gen1==gen2 PASS"
echo "  Examples:      $PASS/$((PASS + FAIL))"
echo "  Std modules:   $SPASS/$((SPASS + SFAIL))"
echo "  Generated C:   $(wc -l < "$WORK/gen1.c") lines"

if [ $FAIL -eq 0 ] && [ $SFAIL -eq 0 ]; then
    echo ""
    echo "=== ALL TESTS PASSED ==="
    exit 0
else
    echo ""
    echo "=== SOME TESTS FAILED ==="
    exit 1
fi
