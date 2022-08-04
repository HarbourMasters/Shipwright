#!/bin/bash

cmake -H. -Bbuild-cmake -GNinja
(cd build-cmake && ninja)

(cd build-cmake && cpack -G External)
