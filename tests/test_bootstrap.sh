#!/bin/bash
# test_bootstrap.sh -- Full bootstrap validation for the "a" language
# 1. VM compiles cgen.a → gen0.c
# 2. gcc gen0.c → ac0 (native compiler)
# 3. ac0 compiles all milestones → verify output matches VM
# 4. ac0 compiles cgen.a → gen1.c
# 5. Verify gen0.c == gen1.c (fixed point)
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$SCRIPT_DIR"

A="cargo run --quiet -- run"
CGEN="std/compiler/cgen.a"
RUNTIME="c_runtime/runtime.c"
TARGETS="examples/c_targets"
TMPDIR=$(mktemp -d)
trap "rm -rf $TMPDIR" EXIT

find . -name "*.ac" -delete 2>/dev/null

echo "=== a language bootstrap test ==="
echo ""

# Step 1: VM compiles cgen.a to C
echo "Step 1: VM compiles cgen.a → gen0.c"
$A "$CGEN" -- "$CGEN" 2>/dev/null > "$TMPDIR/gen0.c"
LINES=$(wc -l < "$TMPDIR/gen0.c")
echo "  gen0.c: $LINES lines"
echo ""

# Step 2: gcc compiles gen0.c to native binary
echo "Step 2: gcc gen0.c → ac0"
gcc "$TMPDIR/gen0.c" "$RUNTIME" -o "$TMPDIR/ac0" -I c_runtime -lm -O2
echo "  ac0 built ($(wc -c < "$TMPDIR/ac0") bytes)"
echo ""

# Step 3: native compiler passes all milestone tests
echo "Step 3: native ac0 compiles all milestones"
PASS=0
FAIL=0
for src in "$TARGETS"/*.a; do
    name=$(basename "$src" .a)
    printf "  %-12s " "$name"

    vm_out=$($A "$src" 2>/dev/null || true)

    if ! "$TMPDIR/ac0" "$src" > "$TMPDIR/${name}.c" 2>/dev/null; then
        echo "FAIL (cgen)"
        FAIL=$((FAIL + 1))
        continue
    fi

    if ! gcc "$TMPDIR/${name}.c" "$RUNTIME" -o "$TMPDIR/$name" -I c_runtime -lm 2>/dev/null; then
        echo "FAIL (gcc)"
        FAIL=$((FAIL + 1))
        continue
    fi

    nat_out=$("$TMPDIR/$name" 2>/dev/null || true)

    if [ "$vm_out" = "$nat_out" ]; then
        echo "PASS"
        PASS=$((PASS + 1))
    else
        echo "FAIL (mismatch)"
        FAIL=$((FAIL + 1))
    fi
done
echo ""
echo "  Milestones: $PASS passed, $FAIL failed"
echo ""

if [ $FAIL -gt 0 ]; then
    echo "BOOTSTRAP FAILED: milestone tests did not pass"
    exit 1
fi

# Step 4: fixed-point test
echo "Step 4: fixed-point test (ac0 compiles itself)"
"$TMPDIR/ac0" "$CGEN" > "$TMPDIR/gen1.c"
GEN1_LINES=$(wc -l < "$TMPDIR/gen1.c")
echo "  gen1.c: $GEN1_LINES lines"

if diff -q "$TMPDIR/gen0.c" "$TMPDIR/gen1.c" > /dev/null 2>&1; then
    echo ""
    echo "  FIXED POINT REACHED: gen0.c == gen1.c"
    echo ""
    echo "=== BOOTSTRAP SUCCESSFUL ==="
    echo "  The 'a' compiler compiles itself to C."
    echo "  The resulting native binary produces identical output."
    echo "  The language is self-hosting."
    exit 0
else
    echo ""
    echo "  FIXED POINT FAILED: gen0.c != gen1.c"
    diff "$TMPDIR/gen0.c" "$TMPDIR/gen1.c" | head -20
    exit 1
fi
