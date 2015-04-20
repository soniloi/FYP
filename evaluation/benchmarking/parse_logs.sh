#!/bin/bash

grepnc='grep --color=never'

mkdir -p results

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
