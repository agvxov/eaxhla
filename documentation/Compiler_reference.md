# EAXHLA

## General
+ x86\_64
+ GPLv3-only

## Technologies
+ C99
+ TommyHash
+ Flex/Bison

## Train of translation
file -> preprocessor -> hla -> link -> exe

NOTE: the compiler front-end should be able to handle the preprocessing someway,
       but we are not making our own preprocessor. use Frexx or m4

### Implementation
1. flex parsing
2. bison creates partial syntax trees (since we dont optimize, we can render in relatively small chunks because not all that much context is needed)
3. xolatile magick

## Command line interface
```
eaxhla <options> <file>
```

options:
```
    -o | --output <file>
    -a | --architecture <architecture>
```
