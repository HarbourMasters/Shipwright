#!/bin/sh
# Lint JSON in soh/assets/custom: each file must parse and be formatted like
# `jq --indent 4` prints it, same 4-space indentation game writes presets with.
#
#   scripts/lint-json.sh          # check, non-zero exit on any problem
#   scripts/lint-json.sh --fix    # reformat in place
#
# Comments not allowed even though resource manager parses with ignore_comments.
#
# Python's json.tool is used when jq is missing.

cd "$(dirname "$0")/.." || exit 2

if command -v jq > /dev/null; then
    format() { jq --indent 4 . "$1"; }
elif python=$(command -v python3 || command -v python); then
    format() { "$python" -m json.tool --indent 4 --no-ensure-ascii "$1"; }
else
    echo "needs jq or python 3.9+"
    exit 2
fi

find soh/assets/custom -name '*.json' | LC_ALL=C sort | {
    status=0
    count=0
    while IFS= read -r file; do
        count=$((count + 1))
        if ! formatted=$(format "$file" 2>&1); then
            printf '%s: %s\n' "$file" "$formatted"  # printf: dash's echo eats backslashes
            status=1
        elif diff=$(printf '%s\n' "$formatted" | diff -u "$file" -); then
            continue
        elif [ "$1" = "--fix" ]; then
            printf '%s\n' "$formatted" > "$file"
            echo "fixed $file"
        else
            printf '%s\n' "$diff"
            status=1
        fi
    done

    [ "$count" -gt 0 ] || { echo "found no JSON under soh/assets/custom"; exit 2; }
    [ "$status" = 0 ] || echo "run scripts/lint-json.sh --fix to reformat"
    exit "$status"
}
