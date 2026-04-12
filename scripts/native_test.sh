#!/bin/bash
# Native test harness: compile "a" test files to C, build, and run.
# Usage: ./scripts/native_test.sh tests/test_patterns.a [tests/test_maps.a ...]
# Or:    ./scripts/native_test.sh --all  (runs all known-good tests)

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
AC="${ROOT}/target/release/a"
RUNTIME="${ROOT}/c_runtime/runtime.c"
INCLUDE="${ROOT}/c_runtime"
TMPDIR="/tmp/a_native_tests"
mkdir -p "$TMPDIR"

PASS=0
FAIL=0
SKIP=0

run_test() {
    local testfile="$1"
    local base="$(basename "$testfile" .a)"
    local wrapper="$TMPDIR/${base}_wrapped.a"
    
    # Extract test function names (fn test_*)
    local test_fns=$(grep -o 'fn test_[a-zA-Z0-9_]*' "$testfile" | sed 's/fn //' | sort -u)
    
    if [ -z "$test_fns" ]; then
        echo "  SKIP $base (no test_ functions found)"
        SKIP=$((SKIP + 1))
        return
    fi
    
    # Build wrapper: original file + main() that calls all tests
    cp "$testfile" "$wrapper"
    
    # Skip adding main if one already exists
    if grep -q '^fn main()' "$testfile"; then
        # File has its own main — use as-is
        :
    else
    # Add main function
    echo "" >> "$wrapper"
    echo "fn main() {" >> "$wrapper"
    for fn in $test_fns; do
        # Check if function returns bool (simple heuristic: look for -> bool)
        if grep -q "fn ${fn}.*-> bool" "$testfile"; then
            echo "  if !${fn}() { fail(\"FAIL: ${fn}\") }" >> "$wrapper"
        else
            echo "  ${fn}()" >> "$wrapper"
        fi
        echo "  println(\"  PASS ${fn}\")" >> "$wrapper"
    done
    echo "  println(\"all ${base} tests passed\")" >> "$wrapper"
    echo "}" >> "$wrapper"
    fi
    
    # Generate C
    local cfile="$TMPDIR/${base}.c"
    if ! timeout 30 "$AC" run std/compiler/cgen.a -- "$wrapper" > "$cfile" 2>&1; then
        echo "  FAIL $base (codegen failed)"
        FAIL=$((FAIL + 1))
        return
    fi
    
    if [ ! -s "$cfile" ]; then
        echo "  FAIL $base (empty C output)"
        FAIL=$((FAIL + 1))
        return
    fi
    
    # Compile C
    local bin="$TMPDIR/${base}"
    if ! cc -o "$bin" "$cfile" "$RUNTIME" -I "$INCLUDE" -lm 2>"$TMPDIR/${base}_cc.err"; then
        echo "  FAIL $base (C compilation failed)"
        cat "$TMPDIR/${base}_cc.err" | head -5
        FAIL=$((FAIL + 1))
        return
    fi
    
    # Run
    local output
    if output=$("$bin" 2>&1); then
        echo "  PASS $base ($(echo "$test_fns" | wc -w | tr -d ' ') tests)"
        PASS=$((PASS + 1))
    else
        echo "  FAIL $base (runtime failure)"
        echo "$output" | tail -5
        FAIL=$((FAIL + 1))
    fi
}

if [ "$1" = "--all" ]; then
    echo "Running all known-compatible native tests..."
    echo ""
    for f in \
        tests/test_patterns.a \
        tests/test_maps.a \
        tests/test_stack_traces.a \
        tests/test_core.a \
        tests/test_destructure.a \
        tests/test_functional.a \
        tests/test_strings.a; do
        if [ -f "$ROOT/$f" ]; then
            run_test "$ROOT/$f"
        fi
    done
else
    for f in "$@"; do
        run_test "$f"
    done
fi

echo ""
echo "Results: $PASS passed, $FAIL failed, $SKIP skipped"
exit $FAIL
