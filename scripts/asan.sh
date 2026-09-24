#!/bin/bash
# asan.sh -- memory correctness check for the a runtime and compiler.
#
# Compiles the C runtime, the compiler (bootstrap/cli.c), and every
# tests/native/test_*.a with AddressSanitizer and runs them. Any sanitizer
# report is a failure. Leak detection is off: the generated C still leaks
# temporaries by design; use-after-free and overflows are what we hunt here.
#
# Usage: ./scripts/asan.sh [name-substring]     (run from the repo root)
set -u
cd "$(cd "$(dirname "$0")/.." && pwd)"

FILTER="${1:-}"
RT=c_runtime
WORK=$(mktemp -d /tmp/a_asan_XXXXXX)
trap 'rm -rf "$WORK"' EXIT
mkdir -p "$WORK/obj" "$WORK/out" "$WORK/home"

OS=$(uname -s)
LINK_EXTRA="-ldl"
[ "$OS" = "Darwin" ] && LINK_EXTRA="-framework Security -framework CoreFoundation"
CFLAGS="-g -O1 -fsanitize=address -fno-omit-frame-pointer -I $RT -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION"
export ASAN_OPTIONS=detect_leaks=0:halt_on_error=1
export A_HOME="$WORK/home"

TIMEOUT_CMD=""
command -v timeout >/dev/null && TIMEOUT_CMD="timeout 180"
command -v gtimeout >/dev/null && TIMEOUT_CMD="gtimeout 180"

echo "== compiling runtime with ASan"
for f in runtime gguf sqlite3 miniz stb_impl embedded; do
  gcc -c $CFLAGS "$RT/$f.c" -o "$WORK/obj/$f.o" 2>"$WORK/obj/$f.err" || { echo "FAILED: $RT/$f.c"; cat "$WORK/obj/$f.err"; exit 1; }
done
OBJS="$WORK/obj/runtime.o $WORK/obj/gguf.o $WORK/obj/sqlite3.o $WORK/obj/miniz.o $WORK/obj/stb_impl.o $WORK/obj/embedded.o"

problems=0
run_one() { # name c_file [args...]
  local name=$1 cfile=$2; shift 2
  local bin="$WORK/out/$name" log="$WORK/out/$name.log"
  if ! gcc $CFLAGS "$cfile" $OBJS -lm $LINK_EXTRA -o "$bin" 2>"$log"; then
    echo "  GCC-FAIL $name"; head -5 "$log"; problems=$((problems+1)); return
  fi
  $TIMEOUT_CMD "$bin" "$@" >"$log" 2>&1
  local code=$?
  if grep -q "AddressSanitizer" "$log"; then
    echo "  ASAN $name: $(grep -m1 'ERROR: AddressSanitizer' "$log" | sed 's/.*AddressSanitizer: //')"
    grep -E "^\s+#[0-9]+ " "$log" | head -6
    problems=$((problems+1))
  elif [ $code -ne 0 ]; then
    echo "  EXIT $code $name (not a sanitizer error; see the normal test runner)"
  else
    echo "  ok $name"
  fi
}

echo "== compiler under ASan (a cc src/cli.a)"
run_one compiler bootstrap/cli.c cc src/cli.a -o "$WORK/out/cli_asan.c"

echo "== tests under ASan"
for t in tests/native/test_*.a; do
  name=$(basename "$t" .a)
  [ -n "$FILTER" ] && [[ "$name" != *"$FILTER"* ]] && continue
  if ! ./a cc "$t" -o "$WORK/out/$name.c" 2>"$WORK/out/$name.cc.err"; then
    echo "  CC-FAIL $name"; head -3 "$WORK/out/$name.cc.err"; problems=$((problems+1)); continue
  fi
  run_one "$name" "$WORK/out/$name.c"
done

echo ""
echo "$problems memory problem(s)"
[ "$problems" -eq 0 ]
