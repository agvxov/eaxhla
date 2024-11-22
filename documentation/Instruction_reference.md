# General

## The following instructions are fully supported:
```asm
    add,            or,             adc,            sbb,
    and,            sub,            xor,            cmp,
    inc,            dec,            not,            neg,
    mul,            imul,           div,            idiv,
    fadd,           fmul,           fcom,           fcomp,
    fsub,           fsubr,          fdiv,           fdivr,
    rol,            ror,            rcl,            rcr,
    sal,            shr,            shl,            sar,
    nop,            retn,           retf,           leave,
    popf,           pushf,
    syscall,        cpuid,          fnop,           fchs,
    fabs,           ftst,           fxam,           fld1,
    fldl2t,         fldl2e,         fldpi,          fldlg2,
    fldln2,         fldz,           f2xm1,          fyl2x,
    fptan,          fpatan,         fxtract,        fprem1,
    fdecstp,        fincstp,        fprem,          fyl2xp1,
    fsqrt,          fsincos,        frndint,        fscale,
    fsin,           fcos,
    enter,          call,           in,             out,
    jmp,            mov,            pop,            push,
    jo,             jno,            jb,             jae,
    je,             jne,            jbe,            ja,
    js,             jns,            jpe,            jpo,
    jl,             jge,            jle,            jg,
    cmovo,          cmovno,         cmovb,          cmovae,
    cmove,          cmovne,         cmovbe,         cmova,
    cmovs,          cmovns,         cmovpe,         cmovpo,
    cmovl,          cmovge,         cmovle,         cmovg,
    seto,           setno,          setb,           setae,
    sete,           setne,          setbe,          seta,
    sets,           setns,          setpe,          setpo,
    setl,           setge,          setle,          setg,
    bswap,          bsf,            bsr,            loop,
    loope,          loopne
```

## 2 argument instructions (t6)

### add, or, adc, sbb, and, sub, xor, cmp;

- REG REG // This means that arg1 can only be REG or MEM.
- REG MEM // And arg2 can only be REG, MEM or IMM, but:
- REG IMM // You can't use MEM as both arguments.
- MEM REG // This limitation stems from x64 encoding.
- MEM IMM // Hardware we use can't encode that.

- REG or MEM can use D8 ... D64, any size.
- IMM in this case must be maximum 32 bit.
- Examples:

```c
add rax 0x11223344 // ADD D64 REG R0 IMM 0x11223344
or  ecx 0x11223344 // OR  D32 REG R1 IMM 0x11223344
adc dx  0x1122     // ADC D16 REG R2 IMM 0x1122
sbb bl  0x11       // SBB D8  REG R3 IMM 0x11
```

## 1 argument instructions (t4)

### inc, dec, not, neg, mul, imul, div, idiv;

- REG
- MEM

- REG or MEM can use D8 ... D64, any size.
- IMM can't be used with these instructions.
- Examples:

```c
inc rsp // INC D64 REG R4
dec ebp // DEC D32 REG R5
not si  // NOT D16 REG R6
neg dil // NEG D8  REG R7
```

# Examples

```c
sysenter           // SYSENTER
sysexit            // SYSEXIT
syscall            // SYSCALL
sysret             // SYSRET
mov eax ebx        // MOV D32 REG R0 REG R3
mov eax [x]        // MOV D32 REG R0 MEM 69
mov eax 0x11223344 // MOV D32 REG R0 IMM 0x11223344
mov [x] eax        // MOV D32 MEM 69 REG R0
mov [x] 0x11223344 // MOV D32 MEM 69 IMM 0x11223344
mov eax x          // MOV D32 REG R0 REL 69
...
u32 x = 420        // ASMDIRMEM 69 ASMDIRIMM D32 420
```

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

Current list of instructions:

ADD, OR, ADC, SBB, AND, SUB, XOR, CMP, INC, DEC, NOT, NEG, MUL, IMUL, DIV, IDIV, FADD, FMUL, FCOM, FCOMP, FSUB, FSUBR, FDIV, FDIVR, ROL, ROR,
RCL, RCR, SAL, SHR, SHL, SAR, FBLD, FBSTP, FADDP, FIADDP, FCLEX, FNCLEX, FCMOVB, FCMOVE, FCMOVBE, FCMOVU, FCMOVAE, FCMOVNE, FCMOVA, FCMOVNU,
FCOMI, FCOMIP, FUCOMI, FUCOMIP, FDIVP, FIDIV, FDIVRP, FIDIVR, FFREE, FICOM, FICOMP, FILD, FINIT, FNINIT, FIST, FISTP, FISTTP, FLD, FLDCW,
FLDENV, FMULP, FIMUL, FRSTOR, FUCOM, FSAVE, FNSAVE, FST, FSTP, FSTCW, FNSTCW, FSTENV, FNSTENV, FSTSW, FNSTSW, FSUBP, FISUB, FSUBRP, FISUBR,
FUCOMPP, FUCOMP, HALT, LOCK, WAIT, LEAVE, NOP, CWDE, POPF, PUSHF, CMC, CLC, CLD, CLI, STC, STD, STI, INT3, RETN, RETF, IRETN, IRETF, SYSCALL,
SYSENTER, SYSRETN, SYSEXITN, CPUID, CDQE, RSM, UD2, EMMS, PAUSE, INVD, WBINVD, WRMSR, RDMSR, RDPMC, RDTSC, FNOP, FCHS, FSIN, FCOS, FABS, FTST,
FXAM, FLD1, FLDL2T, FLDL2E, FLDPI, FLDLG2, FLDLN2, FLDZ, F2XM1, FYL2X, FPTAN, FPATAN, FXTRACT, FPREM1, FDECSTP, FINCSTP, FPREM, FYL2XP1, FSQRT,
FSINCOS, FRNDINT, FSCALE, FCOMPP, LFENCE, MFENCE, SFENCE, RDTSCP, MONITOR, MWAIT, SYSRETF, SYSEXITF, ENTER, CALL, IN, OUT, JMP, MOV, POP, PUSH,
RETNPOP, RETFPOP, JO, JNO, JB, JAE, JE, JNE, JBE, JA, JS, JNS, JPE, JPO, JL, JGE, JLE, JG, CMOVO, CMOVNO, CMOVB, CMOVAE, CMOVE, CMOVNE, CMOVBE,
CMOVA, CMOVS, CMOVNS, CMOVPE, CMOVPO, CMOVL, CMOVGE, CMOVLE, CMOVG, SETO, SETNO, SETB, SETAE, SETE, SETNE, SETBE, SETA, SETS, SETNS, SETPE,
SETPO, SETL, SETGE, SETLE, SETG, LEA, MOVBE, TEST, XADD, XCHG, BSWAP, BT, BTS, BTR, BTC, BSF, BSR, SHLD, SHRD, LOOP, LOOPE, LOOPNE, REP, REPE,
REPNE, INS, OUTS, LODS, STOS, MOVS, CMPS, SCAS;

