#!/bin/bash

cmake --no-warn-unused-cli -H. -Bbuild-cmake -GNinja -DCMAKE_BUILD_TYPE:STRING=Release
cmake --build build-cmake --config Release --

pushd
cd build-cmake
cpack -G External -C Release 
popd
