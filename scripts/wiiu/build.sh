#!/bin/bash

cmake -H. -Bbuild-wiiu -GNinja -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/WiiU.cmake -DCMAKE_BUILD_TYPE:STRING=Release
cmake --build build-wiiu --target soh --config Release
