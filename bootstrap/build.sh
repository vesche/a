#!/bin/bash
set -e

# Build the "a" language from pure C. No Rust, no cargo -- just gcc.
#
# Usage:
#   cd <project-root>
#   ./bootstrap/build.sh
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

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

BOOTSTRAP_C="$PROJECT_ROOT/bootstrap/cli.c"
RUNTIME_C="$PROJECT_ROOT/c_runtime/runtime.c"
SQLITE_C="$PROJECT_ROOT/c_runtime/sqlite3.c"
MINIZ_C="$PROJECT_ROOT/c_runtime/miniz.c"
STB_IMPL_C="$PROJECT_ROOT/c_runtime/stb_impl.c"
INCLUDE_DIR="$PROJECT_ROOT/c_runtime"
OUTPUT="$PROJECT_ROOT/a"

for f in "$BOOTSTRAP_C" "$RUNTIME_C" "$SQLITE_C" "$MINIZ_C" "$STB_IMPL_C"; do
    if [ ! -f "$f" ]; then
        echo "error: missing $f"
        echo "run this script from the project root or ensure all files exist."
        exit 1
    fi
done

echo "Building 'a' from bootstrap C (no Rust required)..."
gcc "$BOOTSTRAP_C" "$RUNTIME_C" "$SQLITE_C" "$MINIZ_C" "$STB_IMPL_C" \
    -o "$OUTPUT" \
    -I "$INCLUDE_DIR" \
    -lm -O2 $STACK_FLAGS $TLS_FLAGS $SQLITE_FLAGS

echo "Built $OUTPUT ($(wc -c < "$OUTPUT" | tr -d ' ') bytes)"
echo ""
echo "Done. Try:"
echo "  ./a run examples/hello.a"
echo "  ./a build examples/hello.a -o hello"
echo "  ./a test tests/native/"
