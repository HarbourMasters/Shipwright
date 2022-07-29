#!/bin/bash

cd soh
make setup -j$(nproc) OPTFLAGS=-O2 DEBUG=0
make -j$(nproc) OPTFLAGS=-O2 DEBUG=0

cd ../OTRGui
mkdir build
cd build
cmake ..
cmake --build . --config Release
