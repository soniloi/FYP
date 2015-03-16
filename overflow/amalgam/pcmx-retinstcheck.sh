#!/bin/bash

### Determine the number of retired instructions in a binary's run

usage="Usage: $0 <path-to-binary> <path-to-sample-input>"
stat='retired'
counter='pcm.x'
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

cat $input | $counter $bin > $temp 2>&1
retline=`$grepnc "$stat" $temp`
retwords=($retline)
let retinsts=${retwords[2]}
if [[ ${retwords[3]} == 'G' ]]; then
	let retinsts=$(($retinsts * 1000000000))
elif [[ ${retwords[3]} == 'M' ]]; then
	let retinsts=$(($retinsts * 1000000))
elif [[ ${retwords[3]} == 'K' ]]; then
	let retinsts=$(($retinsts * 1000))
fi

rm -f $temp

echo -n $retinsts

