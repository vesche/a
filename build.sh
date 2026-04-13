#!/bin/bash
set -e

OS=$(uname -s)
if [ "$OS" = "Darwin" ]; then
    STACK_FLAGS="-Wl,-stack_size,0x10000000"
    TLS_FLAGS="-framework Security -framework CoreFoundation"
else
    STACK_FLAGS=""
    TLS_FLAGS="-ldl"
fi

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

SQLITE_FLAGS="-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION"

echo "Step 2: Build native binary..."
gcc "$TMP_C" "$RUNTIME_DIR/runtime.c" "$RUNTIME_DIR/sqlite3.c" "$RUNTIME_DIR/miniz.c" \
    -o "$OUTPUT" -I "$RUNTIME_DIR" -lm -O2 $STACK_FLAGS $TLS_FLAGS $SQLITE_FLAGS
rm "$TMP_C"
echo "  built $OUTPUT ($(wc -c < "$OUTPUT" | tr -d ' ') bytes)"

LSP_SRC="src/lsp.a"
if [ -f "$LSP_SRC" ]; then
    echo "Step 3: Build language server..."
    TMP_LSP_C=$(mktemp /tmp/a_lsp_XXXXXX.c)
    cargo run --quiet -- run "$CGEN" -- "$LSP_SRC" > "$TMP_LSP_C" 2>/dev/null
    gcc "$TMP_LSP_C" "$RUNTIME_DIR/runtime.c" "$RUNTIME_DIR/sqlite3.c" "$RUNTIME_DIR/miniz.c" \
        -o "./a-lsp" -I "$RUNTIME_DIR" -lm -O2 $STACK_FLAGS $TLS_FLAGS $SQLITE_FLAGS
    rm "$TMP_LSP_C"
    echo "  built ./a-lsp ($(wc -c < "./a-lsp" | tr -d ' ') bytes)"
fi

echo ""
echo "Done. Try:"
echo "  ./a run examples/hello.a"
echo "  ./a build examples/hello.a -o hello"
echo "  ./a test tests/native/"
echo "  ./a cc examples/hello.a"
echo "  ./a-lsp                        (language server for editors)"
