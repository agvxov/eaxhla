# HLA

## General
+ x64
+ AGPLv3

## Technologies
+ Chad C
+ flex/bison

## Train of making
file -> preprocessor -> hla -> exe

### implementation
1. flex parsing
2. bison creates partial syntax trees (since we dont optimize, we can render in relatively small chunks because not all that much context is needed)
3. xolatile magic

question:
do we burn the instruction look up table into the parser or do we depend on look ups?
the former is more work and way worse source, but slightly faster, but also gets awkward if we plan on supporting multiple arches without reimplementing

## Syntax
### Macros
+ fuck macros
+ use a preprocessor

## LATER
+ DWARF2
+ linker??
