#!/bin/bash

set -xe

flex --debug --header-file=object/eaxhla.yy.h -o object/eaxhla.yy.c source/eaxhla.l
bison --debug --defines=object/eaxhla.tab.h -o object/eaxhla.tab.c source/eaxhla.y

clang -DDEBUG=1 -std=c11 -Weverything -Ilibrary/ -Iobject -Isource -c -o object/eaxhla.yy.o object/eaxhla.yy.c
clang -DDEBUG=1 -std=c11 -Weverything -Ilibrary/ -Iobject -Isource -c -o object/eaxhla.tab.o object/eaxhla.tab.c
clang -DDEBUG=1 -std=c11 -Weverything -Ilibrary/ -Iobject -Isource -c -o object/main.o source/main.c
clang -DDEBUG=1 -std=c11 -Weverything -Ilibrary/ -Iobject -Isource -c -o object/eaxhla.o source/eaxhla.c
clang -DDEBUG=1 -std=c11 -Weverything -Ilibrary/ -Iobject -Isource -c -o object/compile.o source/compile.c
clang -DDEBUG=1 -std=c11 -Weverything -Ilibrary/ -Iobject -Isource -c -o object/unix.o source/unix.c
clang -DDEBUG=1 -std=c11 -Weverything -Ilibrary/ -Iobject -Isource -c -o object/assembler.o source/assembler.c
clang -DDEBUG=1 -std=c11 -Weverything -Ilibrary/ -Iobject -Isource -o eaxhla object/*.o -lm library/tommyds/tommy.o library/sds/sds.o

exit
