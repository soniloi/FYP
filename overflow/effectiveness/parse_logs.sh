#!/bin/bash

grepnc='grep --color=never'

$grepnc Runner.*bb ./logs/*.log > runner-bb.log
$grepnc Runner.*func ./logs/*.log > runner-func.log
$grepnc Runner.*alloc\-insert\-4 ./logs/*.log > runner-alloc4.log
$grepnc Runner.*alloc\-insert\-6 ./logs/*.log > runner-alloc6.log

$grepnc Selector.*bb ./logs/*.log > selector-bb.log
$grepnc Selector.*func ./logs/*.log > selector-func.log
$grepnc Selector.*alloc\-insert\-4 ./logs/*.log > selector-alloc4.log
$grepnc Selector.*alloc\-insert\-6 ./logs/*.log > selector-alloc6.log

