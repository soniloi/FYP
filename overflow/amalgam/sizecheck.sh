#!/bin/bash

### Return the size of a file in bytes

usage="Usage: $0 <path-to-binary>"
sizer="du -b"

if [[ "$#" -ne 1 ]]; then
	echo $usage
	exit 1
fi

bin=$1
if ! [[ -f $bin ]]; then
	echo $usage
	exit 1
fi

sizebinline=`$sizer $bin`
sizebin=($sizebinline)
echo -n ${sizebin[0]}
