#!/bin/bash

make setup -C soh -j$(nproc) OPTFLAGS=-O2 DEBUG=0
make -f Makefile.switch -j$(nproc)
