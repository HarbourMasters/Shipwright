#!/bin/bash

cp -av /usr/local/lib/libSDL2* /lib/x86_64-linux-gnu/
git config --global --add safe.directory /soh
make setup -C soh -j$(nproc) OPTFLAGS=-O2 DEBUG=0 CC="gcc" CXX="g++"

/opt/devkitpro/portlibs/switch/bin/aarch64-none-elf-cmake -B StormLib/build-switch -S StormLib -DCMAKE_INSTALL_PREFIX=/opt/devkitpro/portlibs/switch/
make -C StormLib/build-switch -j$(nproc)
make install -C StormLib/build-switch

make -f Makefile.switch -j$(nproc) OPTFLAGS=-O2 DEBUG=0
