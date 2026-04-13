#!/bin/bash
set -e

# Verify vendored C library checksums against c_runtime/VENDORS.md.
# Exit 0 if all match, exit 1 on any mismatch or missing file.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
RUNTIME_DIR="$PROJECT_ROOT/c_runtime"

PASS=0
FAIL=0

check_sha256() {
    local file="$1"
    local expected="$2"
    local path="$RUNTIME_DIR/$file"

    if [ ! -f "$path" ]; then
        echo "  FAIL  $file (missing)"
        FAIL=$((FAIL + 1))
        return
    fi

    local actual
    actual=$(shasum -a 256 "$path" | awk '{print $1}')

    if [ "$actual" = "$expected" ]; then
        echo "  OK    $file"
        PASS=$((PASS + 1))
    else
        echo "  FAIL  $file"
        echo "        expected: $expected"
        echo "        actual:   $actual"
        FAIL=$((FAIL + 1))
    fi
}

echo "Verifying vendored library checksums..."
echo ""

echo "SQLite 3.48.0:"
check_sha256 "sqlite3.c" "5fdc8109b60ea295a2ffe526bcce47812c501db7a41b4b4baeeea921e6f9bfc8"
check_sha256 "sqlite3.h" "78d2dbfa4ebd6146f26ceb735702685af0e77b83ad3df69e4904786924c6899c"

echo ""
echo "miniz 3.1.1:"
check_sha256 "miniz.c" "7487d4c8cd761b951a99d182672bb3badfa72a5ad5760b70b392fafa95223657"
check_sha256 "miniz.h" "a33e04eb7975ab0c7a391ca48bec5336a7bf3a4b2857cfcff3ee02a44766061f"

echo ""
echo "$PASS passed, $FAIL failed"

if [ "$FAIL" -gt 0 ]; then
    echo ""
    echo "Checksum mismatch! See c_runtime/VENDORS.md for expected values."
    exit 1
fi
