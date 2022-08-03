#!/bin/bash

cmake -H. -Bbuild-linux -GNinja
cmake --build build-linux --target ExtractAssets

cmake -H. -Bbuild-switch -GNinja -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Switch.cmake
cmake --build build-switch --target soh_nro
