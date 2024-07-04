# HLA

## General
+ x86_64
+ GPLv3-only

## Technologies
+ Chad C, C99
+ Flex/Bison

## Train of translation
file -> preprocessor -> as -> link -> exe

NOTE: the compiler front-end should be able to handle the preprocessing someway,
       but we are not making our own preprocessor. use Frexx or m4

### implementation
1. flex parsing
2. bison creates partial syntax trees (since we dont optimize, we can render in relatively small chunks because not all that much context is needed)
3. xolatile magic

## Types
```
<prefix><size>
```

prefixes:
+ s - signed
+ u - unsigned

sizes:
+ 8
+ 16
+ 32
+ 64

## Syntax
### Macros
+ fuck macros
+ use a preprocessor
### Asm
+ no ',' argument deliteters
+ optional "[]" argument parenthesizing
### Machine code
```
machine
    // literal values
end machine
```
All literal values (string or numeric) is copied as machine code
### Logic
+ only evaulated in _logical blocks_
#### logical blocks
+ if-then-else-end-if
#### operators
+ =
+ >
+ <
+ <=
+ >=
+ !=
+ not
+ and
+ or
+ xor
### Functions
```
<qualifyiers>
<type> <name>
    <declarations>
begin
    <code>
end <type>
```

qualifier:
+ fast -> use the fastcall calling convention
+ ? stack -> place all arguments on the stack
### labels
```C
my_label:
```

### Come back to later
+ `register`
+ `volatile`
+ `extern`
+ `static`

## LATER
+ DWARF2
+ linker??
