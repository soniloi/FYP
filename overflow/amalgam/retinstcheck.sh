#!/bin/bash

### Determine the number of retired instructions in a binary's run

usage="Usage: $0 <path-to-sample-input> <path-to-binary>"
stat='retired'
counter='pcm.x'
grepnc='grep --color=never'
temp='tmp'

if [[ $# -ne 2 ]]; then
	echo $usage
	exit 1
fi

input=$1
bin=$2
if ! [[ -f $input ]] || ! [[ -x $bin ]]; then
	echo $usage
	exit 1
fi

cat $input | $counter $bin > $temp 2>&1
retline=`$grepnc "$stat" $temp`
retwords=($retline)
let retinsts=${retwords[2]}
if [[ ${retwords[3]} == 'M' ]]; then
	let retinsts=$(($retinsts * 1000000))
fi

rm -f $temp

echo $retinsts

