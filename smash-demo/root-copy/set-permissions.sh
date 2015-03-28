#!/bin/bash

if [[ $# -ne 2 ]]; then
	echo "Usage: $0 <path-to-binary> <group-to-allow>"
	exit 1
fi

binname=$1
group=$2

chown root:$group $binname
chmod 0700 $binname
chmod +s,g+x $binname
