#!/bin/sh
# Fail if any C/C++ source in soh/ outside soh/include contains a tab.

cd "$(dirname "$0")/.." || exit 2

if git --no-pager grep -n "$(printf '\t')" -- 'soh/*.c' 'soh/*.cpp' 'soh/*.h' 'soh/*.hpp' ':!soh/include'; then
    echo "replace tabs with spaces"
    exit 1
fi
