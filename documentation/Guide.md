# Source files

## main.c
Responsible for dispatching initialization, deinitialization and the compiler.
Only deals with the highest level of abstractions and kept clean.

## eaxhla.l|y|c|h
Flex/Bison scanner/parser respectively. The C source contains definitions which
store the abstract state and or required to construct it.

`eaxhla.l` is strictly limited to raising
errors which sabotage scanning.
(Unterminated / partial tokens)

`eaxhla.y` is strictly limited to raising
syntax errors.

`eaxhla.c` is strictly limited to raising
logical errors

## compile.c|h
Responsible for transforming the abstract state of eaxhla.c to something that
can be understood by the assembler, dispatching it and creating the executable.

## assembler.c|h
Creates machine code from an array of tokens.

## debug.h
Defines various debug output functions or nop alternatives for them in
non-debug builds. Must be kept in symmetrical ballance regarding
`#if DEBUG ==` `1` and `!1`.

# Builds
We like Gnu Make.
// X: I hate it.

Debug builds might be create by defining `DEBUG` as 1. The Makefile respects
`DEBUG` in the environment.

Some default values are determined as appropriate for the compiling system.
