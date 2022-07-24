#!/bin/bash

cmake -H. -Bbuild-cmake -GNinja
cmake --build build-cmake

(cd build-cmake && cpack -G External)
