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
3. xolatile magic

## Command line interface
```
eaxhla <options> <file>
```

options:
```
    -o | --output <file>
    -a | --architecture <architecture>
```
## Keywords -- XOLATILE EDIT PLEASE SOMEONE SANE REFACTOR LATER
- Not supporting legacy 8-bit register access ah, ch, dh, bh, all other accessing options are available.
- Please keep in mind that rCx and rDx go before rBx, order matters a lot, assembler trusts that user didn't make a mistake.
- Register accessing process (these are 64 keywords for registers):
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
- For legacy and conventional reasons, we should however adopt stupid decisions made before we were born.

## Instructions
As for core instructions we must support, no matter what, here's the list:
```
mov                               <REG/MEM> <REG/MEM/IMM> -- immediate value can be up to 64 bits.
add or adc sbb and sub xor cmp    <REG/MEM> <REG/MEM/IMM> -- immediate value can be up to 32 bits.
inc dec not neg mul imul div idiv <REG/MEM>               -- no immediates allowed here.
jmp jCC cmovCC                    <>        <>            -- conditional instructions, important!
enter leave pop push                                      -- stack focused instructions.
sysenter sysexit syscall sysret                           -- kernel/system focused instructions.
in out nop call bswap sal sar shr shl rol ror xchg loop   -- ease-of-use focused instructions.
```
- ? ANON: Note that we can use 'loope' or 'loopne' instructions, since 'loop' is our keyword, but it can cause confusion...
- ? Keep in mind that for most of these instructions both destination and source can't be "memory addresses".
- Here is what's available to use, it's in the list below, there are 5 combinations, for 2 argument instructions.
```
ins REG REG -- mov, add, cmp, xor, and
ins REG MEM -- ^
ins REG IMM -- ^
ins MEM REG -- ^
ins MEM IMM -- ^
ins REG     -- inc, dec, not, div, mul
ins MEM     -- ^
ins         -- syscall, pause, hlt, ret, leave
```
- ANON & EMIL: I leave other, HLA-related keywords to you guys, these above are important if we want powerful language.
- I'll implement a lot more instructions into assembler, but you can choose what to directly support by the HLA!
- For example, I'll have a procedure to generate machine code for 'loop' instruction, you can simply not use it when doing HLA->ASM.

## Types
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
#### The following instructions are fully supported:
```asm
    ; XXX fillin
```

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
