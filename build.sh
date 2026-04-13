#!/bin/bash
set -e

echo "=== Building the 'a' language native CLI ==="
echo ""

RUNTIME_DIR="c_runtime"
CLI_SRC="src/cli.a"
CGEN="std/compiler/cgen.a"
OUTPUT="./a"

if [ ! -f "$RUNTIME_DIR/runtime.c" ]; then
    echo "error: cannot find $RUNTIME_DIR/runtime.c"
    echo "run this script from the project root."
    exit 1
fi

if [ ! -f "$CLI_SRC" ]; then
    echo "error: cannot find $CLI_SRC"
    exit 1
fi

echo "Step 1: Compile CLI to C via VM..."
TMP_C=$(mktemp /tmp/a_cli_XXXXXX.c)
cargo run --quiet -- run "$CGEN" -- "$CLI_SRC" > "$TMP_C" 2>/dev/null
echo "  generated $(wc -l < "$TMP_C" | tr -d ' ') lines of C"

echo "Step 2: Build native binary..."
gcc "$TMP_C" "$RUNTIME_DIR/runtime.c" -o "$OUTPUT" \
    -I "$RUNTIME_DIR" -lm -O2 -Wl,-stack_size,0x10000000
rm "$TMP_C"
echo "  built $OUTPUT ($(wc -c < "$OUTPUT" | tr -d ' ') bytes)"

echo ""
echo "Done. Try:"
echo "  ./a run examples/hello.a"
echo "  ./a build examples/hello.a -o hello"
echo "  ./a test tests/native/"
echo "  ./a cc examples/hello.a"
