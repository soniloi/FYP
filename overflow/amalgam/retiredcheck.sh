#!/bin/bash

### Determine the number of retired instructions in a binary's run

usage="Usage: $0 <path-to-binary> <path-to-sample-input>"
stat='rC0' # The Intel code for retired instruction event FIXME: check that the chip is Intel/provide alternative for AMD
counter='perf stat -e '
grepnc='grep --color=never'
temp='tmp'

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

cat $input | $counter $stat $bin > $temp 2>&1
retline=`$grepnc "$stat" $temp`
#echo $retline
retwords=($retline)
retinsts=`echo ${retwords[0]} | tr -d ,`

rm -f $temp

echo -n $retinsts

