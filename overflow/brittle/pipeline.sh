#!/bin/bash

usage="Usage: $0 <path-to-Debug+Asserts-build-dir> <basename> [options]"
if [[ "$#" < 2 ]]; then
	echo $usage
	exit 1
fi

baseaddr=$1
if ! [[ -d $baseaddr ]]; then
	echo $usage
	exit 1
fi

prognamein=$2

# Aliases
local_clang="$baseaddr/bin/clang"
local_lli="$baseaddr/bin/lli"
local_llc="$baseaddr/bin/llc"
local_llvmdis="$baseaddr/bin/llvm-dis"
local_opt="$baseaddr/bin/opt"

progname=$prognamein
link=''
irext='ll' # bc for IR bytecode, ll for IR assembly
irflag='-S' # leave blank if using IR bytecode, use -S if using IR assembly
progir=$progname.$irext

# Compile without assembly/linking
$local_clang -emit-llvm $irflag $progname.c -c -o $progir
if ! [[ -f $progir ]]; then
	echo "Compilation failed, exiting."
	exit 1
fi
echo "compiled -> $progname"

# Run optimizer, if requested
if [[ "$#" > 2 ]]; then
	prognameopt=$prognamein"opt"
	optso=""
	for optflag in ${@:3}; do
		if [[ $optflag == "-alloc-insert" ]]; then
			optso="$optso $baseaddr/lib/AllocInsert.so"
		elif [[ $optflag == "-func-reorder" ]]; then
			optso="$baseaddr/lib/FuncReorder.so"
		elif [[ $optflag == "-bb-reorder" ]]; then
			optso="$baseaddr/lib/BBReorder.so"
		else
			echo "Unknown pass: $optflag"
			exit 1
		fi
		$local_opt -load $optso $optflag < $progir $irflag -o $prognameopt.$irext
		progname=$prognameopt
		progir=$prognameopt.$irext
		if ! [[ -f $progir ]]; then
			echo "Optimization failed, exiting."
			exit 1
		fi
		echo "optimized ($optflag) -> $progname"
	done
fi

# Assemble
$local_llc $progir -o $progname.s
if ! [[ -f $progname.s ]]; then
	echo "Assembly failed, exiting."
	exit 1
fi
echo "assembled -> $progname"

# Link
gcc $progname.s -o $progname $link
if ! [[ -x $progname ]]; then
	echo "Link failed, exiting."
	exit 1
fi
echo "linked -> $progname"
