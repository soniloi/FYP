#!/bin/bash

### Check the heap usage of a binary, given a specific task

usage="Usage: $0 <path-to-binary> <path-to-sample-sql>"
checker="valgrind"
heapmsg='"total heap usage"'
grepnc="grep --color=never"

if ! [[ "$#" -eq 2 ]]; then
	echo $usage
	exit 1
fi

bin=$1
input=$2
if ! [[ -x $bin ]] || ! [[ -f $input ]]; then
	echo $usage
	exit 1
fi

bindir=`dirname $bin`
logfilebin="$bindir/heapcheck.log"

com=`cat $input | $checker "--log-file=$logfilebin" $bin > /dev/null 2>&1`
$com 2>/dev/null
heapline=`$grepnc "total heap usage" $logfilebin`
heapwords=($heapline)
heapuse=`echo -n ${heapwords[8]} | tr -d ,`

# Clean up
rm -f $logfilebin

echo $heapuse

