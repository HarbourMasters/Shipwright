#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"/../..

export PATH="$HERE"/bin:"$HERE"/usr/bin:"$PATH"
export LD_LIBRARY_PATH="$HERE"/usr/lib:"$LD_LIBRARY_PATH"
export ZENITY=$(command -v zenity)

if [ -z ${SHIP_HOME+x} ]; then
export SHIP_HOME=$PWD
fi

if [ -z ${SHIP_BIN_DIR+x} ]; then
export SHIP_BIN_DIR="$HERE/usr/bin"
fi

cd "$SHIP_BIN_DIR"; ./soh.elf
