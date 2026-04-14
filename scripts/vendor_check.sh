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
echo "stb_image v2.30 + stb_image_write v1.16:"
check_sha256 "stb_image.h" "594c2fe35d49488b4382dbfaec8f98366defca819d916ac95becf3e75f4200b3"
check_sha256 "stb_image_write.h" "cbd5f0ad7a9cf4468affb36354a1d2338034f2c12473cf1a8e32053cb6914a05"
check_sha256 "stb_impl.c" "3bdad46da43a131fc3de1520ae8e3aa839b5b800c90409bc10255dcb58410ec6"

echo ""
echo "$PASS passed, $FAIL failed"

if [ "$FAIL" -gt 0 ]; then
    echo ""
    echo "Checksum mismatch! See c_runtime/VENDORS.md for expected values."
    exit 1
fi
