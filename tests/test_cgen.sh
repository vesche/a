#!/bin/bash
# test_cgen.sh -- Validate C code generation backend
# For each test program: run on VM, generate C, compile, run native, compare output.
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$SCRIPT_DIR"

A="cargo run --quiet -- run"
CGEN="std/compiler/cgen.a"
RUNTIME="c_runtime/runtime.c"
TARGETS="examples/c_targets"
TMPDIR=$(mktemp -d)
trap "rm -rf $TMPDIR" EXIT

PASS=0
FAIL=0

find . -name "*.ac" -delete 2>/dev/null

for src in "$TARGETS"/*.a; do
    name=$(basename "$src" .a)
    printf "  %-12s " "$name"

    # Reference output from VM
    if ! $A "$src" > "$TMPDIR/ref_$name.txt" 2>/dev/null; then
        echo "SKIP (vm error)"
        continue
    fi

    # Generate C
    if ! $A "$CGEN" -- "$src" > "$TMPDIR/$name.c" 2>/dev/null; then
        echo "FAIL (cgen error)"
        FAIL=$((FAIL + 1))
        continue
    fi

    # Compile
    if ! gcc "$TMPDIR/$name.c" "$RUNTIME" -o "$TMPDIR/$name" -I c_runtime -lm 2>"$TMPDIR/gcc_$name.txt"; then
        echo "FAIL (gcc error)"
        cat "$TMPDIR/gcc_$name.txt"
        FAIL=$((FAIL + 1))
        continue
    fi

    # Run native and compare
    "$TMPDIR/$name" > "$TMPDIR/nat_$name.txt" 2>/dev/null
    if diff -q "$TMPDIR/ref_$name.txt" "$TMPDIR/nat_$name.txt" > /dev/null 2>&1; then
        echo "PASS"
        PASS=$((PASS + 1))
    else
        echo "FAIL (output mismatch)"
        echo "    expected: $(cat "$TMPDIR/ref_$name.txt" | head -3)"
        echo "    got:      $(cat "$TMPDIR/nat_$name.txt" | head -3)"
        FAIL=$((FAIL + 1))
    fi
done

echo ""
echo "C code generation: $PASS passed, $FAIL failed"
[ $FAIL -eq 0 ] && exit 0 || exit 1
