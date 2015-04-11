#!/bin/bash

### Check the heap usage of a binary

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
data1="$bindir/data1.dat"
data2="$bindir/data2.dat"
data1saved="$data1.saved"
data2saved="$data2.saved"

# We need the datafiles not to be there; if the program overflows the stack, then valgrind will not provide a report
if [[ -f "$data2" ]]; then
	mv $data2 $data2saved
fi

if [[ -f "$data1" ]]; then
	mv $data1 $data1saved
fi

com=`cat $input | $checker "--log-file=$logfilebin" $bin > /dev/null 2>&1`
$com 2>/dev/null
heapline=`$grepnc "total heap usage" $logfilebin`
heapwords=($heapline)
heapuse=`echo -n ${heapwords[8]} | tr -d ,`

# Clean up
rm -f $logfilebin

if [[ -f "$data1saved" ]]; then
	mv $data1saved $data1
fi

if [[ -f "$data2saved" ]]; then
	mv $data2saved $data2
fi

echo $heapuse

