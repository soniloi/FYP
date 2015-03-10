#!/bin/bash

usage="Usage: $0 <path-to-Debug+Asserts-build-dir> <basename> <seed> <link> [options]"
args_mandatory=4
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
seed=$3
link=$4

# Aliases
local_clang="$baseaddr/bin/clang"
local_lli="$baseaddr/bin/lli"
local_llc="$baseaddr/bin/llc"
local_llvmdis="$baseaddr/bin/llvm-dis"
local_opt="$baseaddr/bin/opt"

irext='ll' # bc for IR bytecode, ll for IR assembly
irflag='-S' # leave blank if using IR bytecode, use -S if using IR assembly
progir=$progname.$irext
progirsaved=$progir.saved
progs=$progname.s
progc=$progname.c

# Compile to IR
$local_clang -emit-llvm $irflag $progname.c -c -o $progir
if ! [[ -f $progir ]]; then
	echo "Compilation failed, exiting."
	exit 1
fi
echo "compiled -> $progir"

# Run optimizer, if requested
if [[ "$#" > $args_mandatory ]]; then
	cp $progir $progirsaved
	let pos_first=$((args_mandatory+1))
	optso=""
	for optflag in ${@:$pos_first}; do
		if [[ $optflag == "-alloc-insert" ]]; then
			optso="$optso $baseaddr/lib/AllocInsert.so -rnd-seed=$seed"
		elif [[ $optflag == "-func-reorder" ]]; then
			optso="$baseaddr/lib/FuncReorder.so -rnd-seed=$seed"
		elif [[ $optflag == "-bb-reorder" ]]; then
			optso="$baseaddr/lib/BBReorder.so -rnd-seed=$seed"
		else
			echo "Unknown pass: $optflag"
			exit 1
		fi
		$local_opt -load $optso $optflag < $progir $irflag -o $progir
		if ! [[ -f $progir ]]; then
			echo "Optimization failed, exiting."
			exit 1
		fi
		echo "optimized ($optflag) -> $progir"
	done
fi

# Compile to target assembly
$local_llc $progir -o $progs
if ! [[ -f $progs ]]; then
	echo "Compilation failed, exiting."
	exit 1
fi
echo "compiled -> $progs"

# Assemble and Link
$local_clang $progs -o $progname $link
if ! [[ -x $progname ]]; then
	echo "Link failed, exiting."
	exit 1
fi
echo "assembled and linked -> $progname"
