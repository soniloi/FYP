#!/bin/bash

### Generate LLVM IR from a C source file

usage="Usage: $0 <path-to-Debug+Asserts-build-dir> <basename>"
args_mandatory=2
if [[ "$#" < $args_mandatory ]]; then
  echo $usage
  exit 1
fi

baseaddr=$1
if ! [[ -d $baseaddr ]]; then
  echo $usage
  exit 1
fi

progname=$2
progc=$progname.c
progir=$progname.ll
local_clang="$baseaddr/bin/clang"

$local_clang -emit-llvm -S $progc -c -o $progir
if ! [[ -f $progir ]]; then
	echo "Compilation failed, exiting."
	exit 1
fi
echo "compiled -> $progir"

