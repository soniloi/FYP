#!/bin/bash

### Generate LLVM IR from a C source file

usage="Usage: $0 <path-to-Debug+Asserts-build-dir> <basename> <opt-level>"
args_mandatory=3
if [[ "$#" -ne $args_mandatory ]]; then
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
local_opt="$baseaddr/bin/opt"
frontendflag='-emit-llvm -S -c -w'

optlevel=$3
optflag="$optlevel -S"

# Run frontend
$local_clang $frontendflag $progc -o $progir
if ! [[ -f $progir ]]; then
	echo "Compilation failed, exiting."
	exit 1
fi
echo "compiled -> $progir"

# Run common optimizations
$local_opt $optflag $progir -o $progir
echo "optimized ($optlevel) -> $progir"
