#!/bin/bash

## Compile, assemble, and link from IR to executable with function-reordering randomization applied

if [[ $# -ne 1 ]]; then
	echo "Usage: $0 <path-to-Debug+Asserts>"
	exit 1
fi

daa=$1

set -x

bindir=$daa/bin
libdir=$daa/lib

$bindir/opt -S -load $libdir/FuncReorder.so -func-reorder -rnd-seed=13 < eggshell.ll -o eggshell-rand.ll
$bindir/llc eggshell-rand.ll -o eggshell-rand.s
$bindir/clang eggshell-rand.s -o eggshell-rand
