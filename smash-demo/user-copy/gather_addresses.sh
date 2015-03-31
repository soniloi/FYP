#!/bin/bash

## Gather the addresses needed to smash eggshell's stack

set -x

objdump -x eggshell | grep --color=always spawn_shell
objdump --dynamic-reloc eggshell | grep --color=always _IO_putc
execaddrline=(`objdump --dynamic-reloc eggshell | grep execl`)
gdb -q eggshell -ex "x/1gx 0x$execaddrline" -ex "quit"
