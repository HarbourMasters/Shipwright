#!/bin/bash

cmake -H. -Bbuild-cmake -GNinja
cmake --build build-cmake -- -j$(nproc)

(cd build-cmake && cpack -G External)
