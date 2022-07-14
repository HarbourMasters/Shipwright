#!/bin/bash

git clone https://github.com/ladislav-zezula/StormLib external/StormLib
cmake -B external/StormLib/build -S external/StormLib
cmake --build external/StormLib/build
cp external/StormLib/build/libstorm.a external
cp /usr/local/lib/libGLEW.a external
cd soh
make setup -j$(nproc) OPTFLAGS=-O2 DEBUG=0
make -j$(nproc) OPTFLAGS=-O2 DEBUG=0
cd ../OTRGui
mkdir build
cd build
cmake ..
cmake --build . --config Release