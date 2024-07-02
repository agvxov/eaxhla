# HLA

## General
+ x86_64
+ GPLv3-only

## Technologies
+ Chad C, C99
+ Flex/Bison

## Train of making
file -> preprocessor -> as -> link -> exe

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

## Syntax
### Macros
+ fuck macros
+ use a preprocessor
### logic
+ only evaulated in _logical blocks_
#### logical blocks
+ if
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

### labels
```C
my_label:

```

### Come back to later
+ `register`
+ `volatile`
+ `extern`
+ `static`

### Assembly Example
```C
program example
begin
    if a is 1 then
        // ...
    else
        // ...
    end if
end program
```

## LATER
+ DWARF2
+ linker??
