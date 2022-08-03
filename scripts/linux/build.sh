#!/bin/bash

<<<<<<< HEAD
cd soh
make setup -j$(nproc) OPTFLAGS=-O2 DEBUG=0
make -j$(nproc) OPTFLAGS=-O2 DEBUG=0

cd ../OTRGui
mkdir build
cd build
cmake ..
cmake --build . --config Release
=======
cmake -H. -Bbuild-cmake -GNinja
cmake --build build-cmake

(cd build-cmake && cpack -G External)
>>>>>>> b0af4f65 (Update the appimage scripts)
