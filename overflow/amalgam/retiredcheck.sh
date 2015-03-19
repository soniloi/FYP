#!/bin/bash

### Determine the number of retired instructions in a binary's run

usage="Usage: $0 <path-to-binary> <path-to-sample-input>"
stat='rC0' # The Intel code for retired instruction event FIXME: check that the chip is Intel/provide alternative for AMD
counter='perf stat -e '
grepnc='grep --color=never'

if [[ $# -ne 2 ]]; then
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
temp="$bindir/rettempp"
data1="$bindir/data1.dat"
data2="$bindir/data2.dat"
data1saved="$data1.saved"
data2saved="$data2.saved"

# We need the datafiles not to be there; segmentation fault of itself causes a difference in retired instructions
if [[ -f "$data2" ]]; then
	mv $data2 $data2saved
fi

if [[ -f "$data1" ]]; then
	mv $data1 $data1saved
fi


cat $input | $counter $stat $bin > $temp 2>&1
retline=`$grepnc "$stat" $temp`
#echo $retline
retwords=($retline)
retinsts=`echo ${retwords[0]} | tr -d ,`

# Clean up
rm -f $temp

if [[ -f "$data1saved" ]]; then
	mv $data1saved $data1
fi

if [[ -f "$data2saved" ]]; then
	mv $data2saved $data2
fi

echo -n $retinsts

