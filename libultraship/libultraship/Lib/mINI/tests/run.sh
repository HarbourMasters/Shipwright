#!/bin/bash
if [[ $# -eq 0 ]] ; then
    echo Use: $0 [test name]
	exit 1
fi
./$1.test -p