#!/bin/bash

### Check whether running a binary with specific given payloads overflows the call stack
### Program does nothing useful; it is just started and then exits; it has been
###  arranged so that the stack will be smashed on exit
### If no overflow occurs (or if the attempt causes a segmentation fault), then 
###  there will be no output

usage="Usage: $0 <path-to-binary>"
grepnc="grep --color=never -q"
pattern_smash="process [0-9][0-9]* is executing new program: /bin/bash"
smashed=0

if [[ "$#" -ne 1 ]]; then
	echo $usage
	exit 1
fi

bin=$1
if ! [[ -x $bin ]]; then
	echo $usage
	exit 1
fi

logfile="run.log"

gdb $bin -ex run > $logfile 2>&1 &
wait
if $grepnc "$pattern_smash" $logfile; then
	smashed=1
else
	smashed=0
fi

# Clean up
rm -f $logfile

# Print result
echo $smashed
