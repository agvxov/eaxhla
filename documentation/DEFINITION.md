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

### eh
```C
program examle
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
