#!/bin/bash

## Smash user copy of the eggshell

set -x

ps -o ppid=
whoami
./eggshell data1.dat data2.dat
