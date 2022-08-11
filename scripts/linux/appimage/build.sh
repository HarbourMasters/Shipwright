#!/bin/bash

cmake --no-warn-unused-cli -H. -Bbuild-cmake -GNinja -DCMAKE_BUILD_TYPE:STRING=Release
cmake --build build-cmake --target OTRGui --
cmake --build build-cmake --config Release --

(cd build-cmake && cpack -G External)
