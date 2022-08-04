#!/bin/bash

cmake -H. -Bbuild-linux -GNinja
(cd build-linux && ninja ExtractAssets)

cmake -H. -Bbuild-switch -GNinja -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Switch.cmake
(cd build-switch && ninja soh_nro)
