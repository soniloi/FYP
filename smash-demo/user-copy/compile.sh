#!/bin/bash

## Compile, assemble, and link from C source to executable without randomization

if [[ $# -ne 1 ]]; then
	echo "Usage: $0 <path-to-Debug+Asserts>"
	exit 1
fi

daa=$1

set -x

bindir=$daa/bin
libdir=$daa/lib

$bindir/clang -emit-llvm -S -c eggshell.c
$bindir/llc eggshell.ll -o eggshell.s
$bindir/clang eggshell.s -o eggshell
