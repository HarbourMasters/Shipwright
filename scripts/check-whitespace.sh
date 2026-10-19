#!/bin/sh
# Fail if any C/C++ source in soh/ outside soh/include contains tabs or
# trailing whitespace. Pass --fix to strip trailing whitespace in place
# (tabs still need fixing by hand).

cd "$(dirname "$0")/.." || exit 2

# git grep over the checked sources, extra args go before the file list
grep_sources() {
    git --no-pager grep "$@" -- 'soh/*.c' 'soh/*.cpp' 'soh/*.h' 'soh/*.hpp' ':!soh/include'
}

if [ "$1" = "--fix" ]; then
    grep_sources -lE '[[:space:]]+$' | xargs -r sed -i -E 's/[[:space:]]+$//'
fi

status=0
if grep_sources -n "$(printf '\t')"; then
    echo "replace tabs with spaces"
    status=1
fi
if grep_sources -nE '[[:space:]]+$'; then
    echo "remove trailing whitespace (or run with --fix)"
    status=1
fi
exit $status
