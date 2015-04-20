#!/bin/bash

if [[ "$#" -ne 1 ]]; then
	echo "Usage: $0 <path-to-Debug+Asserts>"
	exit 1
fi

daa=$1
total_versions=64
runs_per_technique=10
readings_per_run=4
seed_initial=17

RANDOM=$seed_initial

for i in $(seq 1 $total_versions); do
	seed=$RANDOM
	echo $seed
	./Runner.py $daa $i $runs_per_technique $readings_per_run $seed > "logs/v$i.log" 2>&1 &
done
wait
