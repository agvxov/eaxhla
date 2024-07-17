#!/bin/sh
# this script requires git.
cd "$(dirname "$(readlink -f "$0")")"

git clone https://github.com/amadvance/tommyds/ --depth=1
make -C tommyds

git clone https://github.com/jcorporation/sds.git --depth=1 
cc -O0 -ggdb -fno-inline -Wall -Wextra -Wpedantic  -DDEBUG -c -o sds/sds.o sds/sds.c
