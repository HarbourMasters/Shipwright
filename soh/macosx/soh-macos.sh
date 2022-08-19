#!/bin/sh
BUNDLE="`echo "$0" | sed -e 's/\/Contents\/MacOS\/.*//'`"
RESOURCES="$BUNDLE/Contents/Resources"

export DYLD_FRAMEWORK_PATH=$RESOURCES/Frameworks
export DYLD_LIBRARY_PATH=$RESOURCES/bin

exec "$RESOURCES/soh-macos"
