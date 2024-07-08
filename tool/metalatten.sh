#!/bin/bash

set -xe

gcc -o flatten flatten.c
gcc -o xlatten xlatten.c

./flatten > flatten.asm
./xlatten > include.txt

fasm flatten.asm flatten.exe
./nop flatten.exe > flatten.txt

gcc -o test test.c
./test > xlatten.txt

exit
