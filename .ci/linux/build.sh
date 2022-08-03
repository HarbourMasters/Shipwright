#!/bin/bash

cmake -H. -Bbuild-cmake -GNinja
cmake --build build-cmake

cpack -G External
