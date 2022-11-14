#!/bin/bash

cmake --no-warn-unused-cli -H. -Bbuild-cmake -G"Ninja Multi-Config" -DCMAKE_BUILD_TYPE:STRING=Release
cmake --build build-cmake --target OTRGui --
cmake --build build-cmake --config Release --

pushd
cd build-cmake && cpack -G External -C Release --trace-expand --debug
popd
