#!/bin/bash
if [[ $# -eq 0 ]] ; then
    echo Use: $0 [test name]
	exit 1
fi
g++ -Wall -Wextra -std=c++14 -I./lest -I./../src -o $1.test $1.cpp
