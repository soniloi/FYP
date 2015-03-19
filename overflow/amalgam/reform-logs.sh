#!/bin/bash

### Take a logfile that might be written to in any order and re-write it in order

usage="$0 <path-to-input-logfile> <number-of-versions> <path-to-output-logfile>"
vprefix='version-'
spattern='\[summary\]'
grepnc='grep --color=never'

if [[ "$#" -ne 3 ]]; then
	echo $usage
	exit 1
fi

logfilein=$1
if ! [[ -f $logfilein ]]; then
	echo $usage
	exit 1
fi

versions=$2
logfileout=$3

rm -f $logfileout
touch $logfileout

$grepnc "$spattern" $logfilein >> $logfileout
echo -e "\n\n" >> $logfileout

for i in `seq 1 $versions`; do
	label="^\[$vprefix$i\]"
	$grepnc "$label" $logfilein >> $logfileout
	echo -e "\n\n" >> $logfileout
done
