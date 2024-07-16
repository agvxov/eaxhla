# HLA

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

### implementation
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

## Syntax

### Macros
+ fuck macros
+ use a preprocessor

### Comments
```c
    // single line comment
    /* multi
       line
       comment */
```
HLA uses C/C++ comments,
but C comments cannot be multilined.
Nested multiline comments are still not allowed.

### Asm
+ no ',' argument deliteters

For the specifics of the supported instructions consult
[Instruction\_reference.md](Instruction\_reference.md).

#### Registers
```
| NUM | QWORD  | DWORD  | WORD   | BYTE   |
| NUM | 64 BIT | 32 BIT | 16 BIT |  8 BIT |
|  0  |  rax   |  eax   |  ax    |  al    |
|  1  |  rcx   |  ecx   |  cx    |  cl    |
|  2  |  rdx   |  edx   |  dx    |  dl    |
|  3  |  rbx   |  ebx   |  bx    |  bl    |
|  4  |  rsp   |  esp   |  sp    |  spl   |
|  5  |  rbp   |  ebp   |  bp    |  bpl   |
|  6  |  rsi   |  esi   |  si    |  sil   |
|  7  |  rdi   |  edi   |  di    |  dil   |
|  8  |  r8    |  r8d   |  r8w   |  r8b   |
|  9  |  r9    |  r9d   |  r9w   |  r9b   |
|  A  |  r10   |  r10d  |  r10w  |  r10b  |
|  B  |  r11   |  r11d  |  r11w  |  r11b  |
|  C  |  r12   |  r12d  |  r12w  |  r12b  |
|  D  |  r13   |  r13d  |  r13w  |  r13b  |
|  E  |  r14   |  r14d  |  r14w  |  r14b  |
|  F  |  r15   |  r15d  |  r15w  |  r15b  |
```


### Types
```
<int prefix><int size>
<float prefix><float size>
```

int prefixes:
+ s - signed
+ u - unsigned

int sizes:
+ 8
+ 16
+ 32
+ 64

float prefixes:
+ f (guarantee IEEE)

float sizes:
+ 32
+ 64
+ 80?
+ 128?

All of these types would be generically available unless disabled by some compiler option.
All of the traditional types would be enabled by default or require said flag to become usable,
this hypothetical flag (`-ftraditional-types`) would then need implementation and support.
In the future traditional types could be enabled by default along with a flag like
(`-fno-terry-types`) to disable EAXCC standard typing.
If these types are ever disabled by default
then we'll utilize the flag (`-fterry-types`) to enable them.
// but why emil? are we planning to support C type names?
  // if so, why are they not listed above?
  // if not, does that mean no variables an be declared?


### Machine code
```
machine
    // literal values
end machine
```
All literal values (string or numeric) are copied as machine code.

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

type:
+ procedure
+ ? function

### labels
```C
my_label:
```
Labels act like variables,
but should not be dereferenced.
Feel free to use them inside jump instructions.

### Come back to later
+ `register`
+ `volatile`
+ `extern`
+ `static`

## LATER
+ DWARF2
  - ask xolatile very nicely
+ linker?
  - support ld (thereby mold/gold) for speed reasons
