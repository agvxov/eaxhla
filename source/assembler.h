#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#define JNPE (JPO)
#define JNPO (JPE)
#define JNB  (JAE)
#define JNBE (JA)
#define JNA  (JBE)
#define JNAE (JB)
#define JNL  (JGE)
#define JNLE (JG)
#define JNG  (JLE)
#define JNGE (JL)

#define CMOVNPE (CMOVPO)
#define CMOVNPO (CMOVPE)
#define CMOVNB  (CMOVAE)
#define CMOVNBE (CMOVA)
#define CMOVNA  (CMOVBE)
#define CMOVNAE (CMOVB)
#define CMOVNL  (CMOVGE)
#define CMOVNLE (CMOVG)
#define CMOVNG  (CMOVLE)
#define CMOVNGE (CMOVL)

#define SETNPE (SETPO)
#define SETNPO (SETPE)
#define SETNB  (SETAE)
#define SETNBE (SETA)
#define SETNA  (SETBE)
#define SETNAE (SETB)
#define SETNL  (SETGE)
#define SETNLE (SETG)
#define SETNG  (SETLE)
#define SETNGE (SETL)

enum {
    D8,             D16,            D32,            D64,
    D80,            D128,           D256,           D512
};

enum {
    REL,            REG,            MEM,            IMM
};

enum {
    ASMDIRMEM,      ASMDIRREL,      ASMDIRIMM,      ASMDIRREP,
    ADD,            OR,             ADC,            SBB,
    AND,            SUB,            XOR,            CMP,
    INC,            DEC,            NOT,            NEG,
    MUL,            IMUL,           DIV,            IDIV,
    FADD,           FMUL,           FCOM,           FCOMP,
    FSUB,           FSUBR,          FDIV,           FDIVR,
    ROL,            ROR,            RCL,            RCR,
    SAL,            SHR,            SHL,            SAR,
    NOP,            RETN,           RETF,           LEAVE,
    POPF,           PUSHF,
    SYSCALL,        CPUID,          FNOP,           FCHS,
    FABS,           FTST,           FXAM,           FLD1,
    FLDL2T,         FLDL2E,         FLDPI,          FLDLG2,
    FLDLN2,         FLDZ,           F2XM1,          FYL2X,
    FPTAN,          FPATAN,         FXTRACT,        FPREM1,
    FDECSTP,        FINCSTP,        FPREM,          FYL2XP1,
    FSQRT,          FSINCOS,        FRNDINT,        FSCALE,
    FSIN,           FCOS,
    ENTER,          CALL,           IN,             OUT,
    JMP,            MOV,            POP,            PUSH,
    JO,             JNO,            JB,             JAE,
    JE,             JNE,            JBE,            JA,
    JS,             JNS,            JPE,            JPO,
    JL,             JGE,            JLE,            JG,
    CMOVO,          CMOVNO,         CMOVB,          CMOVAE,
    CMOVE,          CMOVNE,         CMOVBE,         CMOVA,
    CMOVS,          CMOVNS,         CMOVPE,         CMOVPO,
    CMOVL,          CMOVGE,         CMOVLE,         CMOVG,
    SETO,           SETNO,          SETB,           SETAE,
    SETE,           SETNE,          SETBE,          SETA,
    SETS,           SETNS,          SETPE,          SETPO,
    SETL,           SETGE,          SETLE,          SETG,
    LEA,            CBW,            CWD,            CDQ,
    BT,             BTC,            BTR,            BTS,
    CMC,            CLC,            CLD,            CLI,
    MOVBE,          STC,            STD,            STI,
    TEST,           UD2,            XADD,           XCHG,
    BSWAP,          BSF,            BSR,
    LOOP,           LOOPE,          LOOPNE,
    REP,            REPE,           REPNE,
    INS,            OUTS,           LODS,           STOS,
    MOVS,           CMPS,           SCAS
    // The REP prefix can be added to the INS, OUTS, MOVS, LODS, and STOS instructions.
    // The REPE and REPNE prefixes can be added to the CMPS and SCAS instructions.
    // > CPUID, CMPXCHG, MONITOR, INT, IRET, HLT, LFENCE, MFENCE, SFENCE, MWAIT, PAUSE, SHLD, SHRD,
};

enum {
    GR0,            GR1,            GR2,            GR3,
    GR4,            GR5,            GR6,            GR7,
    GR8,            GR9,            GR10,           GR11,
    GR12,           GR13,           GR14,           GR15
};

enum {
    FR0,            FR1,            FR2,            FR3,
    FR4,            FR5,            FR6,            FR7
};

enum {
    VR0,            VR1,            VR2,            VR3,
    VR4,            VR5,            VR6,            VR7,
    VR8,            VR9,            VR10,           VR11,
    VR12,           VR13,           VR14,           VR15,
    VR16,           VR17,           VR18,           VR19,
    VR20,           VR21,           VR22,           VR23,
    VR24,           VR25,           VR26,           VR27,
    VR28,           VR29,           VR30,           VR31
}; // We don't use them currently.

extern int    main_entry_point;
extern int    text_sector_size;
extern char * text_sector_byte;
extern int    data_sector_size; // This is unused, and it should be used...
extern char * data_sector_byte; // This is unused, and it should be used...

extern int was_instruction_array_empty;

extern int assemble (      int            count,
                     const int * restrict array);

#endif
