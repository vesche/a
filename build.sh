#!/bin/bash
set -e

# Build the "a" language from pure C + self-hosting. No Rust, no cargo.
#
# Steps:
#   1. Compile bootstrap/cli.c + C runtime -> bootstrap ./a
#   2. Self-host: ./a builds itself from src/cli.a -> a2
#   3. Verify a2 works, replace ./a
#   4. Build language server (a-lsp)
#
# Requirements: gcc (or any C99 compiler). That's it.

OS=$(uname -s)
if [ "$OS" = "Darwin" ]; then
    STACK_FLAGS="-Wl,-stack_size,0x10000000"
    TLS_FLAGS="-framework Security -framework CoreFoundation"
else
    STACK_FLAGS=""
    TLS_FLAGS="-ldl"
fi

SQLITE_FLAGS="-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION"
RUNTIME_DIR="c_runtime"
OUTPUT="./a"

echo "=== Building the 'a' language (gcc only, no Rust) ==="
echo ""

for f in bootstrap/cli.c "$RUNTIME_DIR/runtime.c" "$RUNTIME_DIR/sqlite3.c" "$RUNTIME_DIR/miniz.c" "$RUNTIME_DIR/stb_impl.c" "$RUNTIME_DIR/embedded.c"; do
    if [ ! -f "$f" ]; then
        echo "error: missing $f"
        echo "run this script from the project root."
        exit 1
    fi
done

echo "Step 1: Bootstrap from pre-generated C..."
gcc bootstrap/cli.c "$RUNTIME_DIR/runtime.c" "$RUNTIME_DIR/sqlite3.c" "$RUNTIME_DIR/miniz.c" "$RUNTIME_DIR/stb_impl.c" "$RUNTIME_DIR/embedded.c" \
    -o "$OUTPUT" -I "$RUNTIME_DIR" -lm -O2 $STACK_FLAGS $TLS_FLAGS $SQLITE_FLAGS
echo "  built bootstrap ./a ($(wc -c < "$OUTPUT" | tr -d ' ') bytes)"

echo "Step 2: Self-host (./a builds itself)..."
$OUTPUT build src/cli.a -o a2 2>&1
echo "  built a2 ($(wc -c < a2 | tr -d ' ') bytes)"

echo "Step 3: Verify self-hosted binary..."
VERIFY=$(./a2 run examples/hello.a 2>&1)
if echo "$VERIFY" | grep -q "hello from a"; then
    echo "  verified: a2 works"
else
    echo "  error: a2 verification failed"
    echo "  output: $VERIFY"
    rm -f a2
    exit 1
fi

mv a2 "$OUTPUT"
echo "  replaced ./a with self-hosted binary"

LSP_SRC="src/lsp.a"
if [ -f "$LSP_SRC" ]; then
    echo "Step 4: Build language server..."
    $OUTPUT build "$LSP_SRC" -o "./a-lsp" 2>&1
    echo "  built ./a-lsp ($(wc -c < "./a-lsp" | tr -d ' ') bytes)"
fi

echo ""
echo "Done. Try:"
echo "  ./a run examples/hello.a"
echo "  ./a build examples/hello.a -o hello"
echo "  ./a test tests/native/"
echo "  ./a cc examples/hello.a"
echo "  ./a-lsp                        (language server for editors)"
