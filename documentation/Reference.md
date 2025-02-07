# HLA

## Premise
+ ASM, but slightly more readable

## Macros
+ fuck macros
+ use a preprocessor

## Comments
```c
    // single line comment
    /* multi
       line
       comment */
```
HLA uses C/C++ comments,
but C comments cannot be multilined.
Nested multiline comments are still not allowed.
```sh
    # single line comment
```
HLA also supports script like comments


## Asm
```
<instruction> <arg>*
```

+ no ',' argument deliteters

For the specifics of the supported instructions consult
[Instruction\_reference.md](Instruction\_reference.md).

### Registers
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
// X: Legacy registers ah, ch, dh, bh aren't supported.

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
+ 80
+ 128?
+ 256?
+ 512?

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


## Machine code
```
machine
    // literal values
end machine
```
All literal values (string or numeric) are copied as machine code.

## Logic
```
if <logical-block> then
[else if <logic> then]+
[otherwise]
end if
```

Logic is only evaulated in _logical blocks_

### operators
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
// X: Since we're in assembly, overflow, sign and parity can be added!

## Functions
```
<qualifyiers>
<type> <name>
    <declarations>
begin
    <code>
end <type>
```

qualifier:
+ fast  -> use the fastcall calling convention
+ stack -> place all arguments on the stack; this implies varargs

type:
+ procedure
+ ? function -> X: Return value is in accumulator register anyway.

### calls
```
<call-type> <function> <args>+ [end call]
```

call types:
+ fastcall    -> call using the fastcall convention
+ stackcall   -> call pushing each argument on the stack
+ ccall       -> use the C calling convention
+ fortrancall -> use the fortran calling convention
+ inlinecall  -> copy the function definition; use the calling convention the function is marked with

During a vararg call, the phrase `end call` must be used to signal the end of the argument list

The special token `_` can be used to pass "nothing".
This, in all sane contexes means leaving a registers value unchanged.

## labels
```C
my_label:
```
Labels act like variables, but should not be dereferenced.
Feel free to use them inside jump instructions.
// X: Is there flex way to verify when they are used...?
// X: I don't mind adding integers and labels tho, it's based.
// X: A label is just a number calculated at compile time.

## Come back to later
+ `register`
+ `volatile`
+ `extern`
+ `static`
// X: We don't really need 'register' or 'volatile'.
// X: Only either of 'static' or 'extern' should be added.
// X: If it's not declared as one, then it must be the other.
// X: Same as 'const' qualifier in C, there's no 'var' one.

### LATER
+ DWARF2
  - ask xolatile very nicely
  // X: Will do it, would work well with Valgrind.
+ linker?
  - support ld (thereby mold/gold) for speed reasons
  // X: This is sadly very useful...
  // X: It'll enable us to use C libraries with EAXHLA.
