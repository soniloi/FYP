#!/bin/bash

grepnc='grep --color=never'

mkdir -p results

if false; then
$grepnc Single.*bb ./logs/*.log > results/single-bb.log
$grepnc Single.*func ./logs/*.log > results/single-func.log
$grepnc Single.*alloc\-insert\-4 ./logs/*.log > results/single-alloc4.log
$grepnc Single.*alloc\-insert\-6 ./logs/*.log > results/single-alloc6.log

$grepnc Aggregate.*bb ./logs/*.log > results/aggregate-bb.log
$grepnc Aggregate.*func ./logs/*.log > results/aggregate-func.log
$grepnc Aggregate.*alloc\-insert\-4 ./logs/*.log > results/aggregate-alloc4.log
$grepnc Aggregate.*alloc\-insert\-6 ./logs/*.log > results/aggregate-alloc6.log

$grepnc Single.*retired ./logs/*.log > results/single-retired.log
$grepnc Single.*size ./logs/*.log > results/single-size.log
$grepnc Single.*heap ./logs/*.log > results/single-heap.log
fi

$grepnc Aggregate.*retired ./logs/*.log > results/aggregate-retired.log
$grepnc "\-alloc\-insert\-4" results/aggregate-retired.log > results/aggregate-retired-alloc4.log
$grepnc "\-alloc\-insert\-6" results/aggregate-retired.log > results/aggregate-retired-alloc6.log
$grepnc "\-func\-reorder" results/aggregate-retired.log > results/aggregate-retired-func.log
$grepnc "\-bb\-reorder" results/aggregate-retired.log > results/aggregate-retired-bb.log

$grepnc Aggregate.*size ./logs/*.log > results/aggregate-size.log
$grepnc "\-alloc\-insert\-4" results/aggregate-size.log > results/aggregate-size-alloc4.log
$grepnc "\-alloc\-insert\-6" results/aggregate-size.log > results/aggregate-size-alloc6.log
$grepnc "\-func\-reorder" results/aggregate-size.log > results/aggregate-size-func.log
$grepnc "\-bb\-reorder" results/aggregate-size.log > results/aggregate-size-bb.log

$grepnc Aggregate.*heap ./logs/*.log > results/aggregate-heap.log
$grepnc "\-alloc\-insert\-4" results/aggregate-heap.log > results/aggregate-heap-alloc4.log
$grepnc "\-alloc\-insert\-6" results/aggregate-heap.log > results/aggregate-heap-alloc6.log
$grepnc "\-func\-reorder" results/aggregate-heap.log > results/aggregate-heap-func.log
$grepnc "\-bb\-reorder" results/aggregate-heap.log > results/aggregate-heap-bb.log
