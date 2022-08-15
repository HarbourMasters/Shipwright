#!/bin/bash

# to build shaders you need to place a copy of latte-assembler into the current directory
# latte-assembler is part of decaf-emu <https://github.com/decaf-emu/decaf-emu>

# shader
./latte-assembler assemble --vsh=shader.vsh --psh=shader.psh shader.gsh
xxd -i shader.gsh > shader.h
