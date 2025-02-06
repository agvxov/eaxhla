#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define JNPE (JPO) /// X: INTERFACE SHOULDN'T CHANGE!
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

typedef enum {
    D8,             D16,            D32,            D64,
    D80,            D128,           D256,           D512,
    SIZE_END
} size_code_t;

typedef enum {
    REL,            REG,            MEM,            IMM,
    //~DER,            INT,            BCF,            BCD,
    OPERAND_END
} operand_code_t;

typedef enum {
    ASMDIRMEM,      ASMDIRREL,      ASMDIRIMM,      ASMDIRREP,
    ADD,            OR,             ADC,            SBB,
    AND,            SUB,            XOR,            CMP,
    INC,            DEC,            NOT,            NEG,
    MUL,            IMUL,           DIV,            IDIV,
    ROL,            ROR,            RCL,            RCR,
    SAL,            SHR,            SHL,            SAR,
    FADD,           FMUL,           FCOM,           FCOMP,
    FSUB,           FSUBR,          FDIV,           FDIVR,
    FCMOVB,         FCMOVE,         FCMOVBE,        FCMOVU,
    FCMOVAE,        FCMOVNE,        FCMOVA,         FCMOVNU,
    /**/
    //~FBLD,           FBSTP,
    /**/
    //~FADDP,          FIADD,
    //~FMULP,          FIMUL,
    //~ fcompare
    //~FCOMI,          FCOMIP,         FUCOMI,         FUCOMIP, // 2-3
    //~FICOM,          FICOMP,         FUCOM,          FUCOMP,
    //~ fcompare
    //~FSUBP,          FISUB,
    //~FSUBRP,         FISUBR,
    //~FDIVP,          FIDIV,
    //~FDIVRP,         FIDIVR,
    /**/
    //~FFREE,
    //~FILD,
    //~FIST,           FISTP,
    //~FISTTP,         FLD,            FLDCW,          FLDENV,
    //~FRSTOR,
    //~FSAVE,          FNSAVE,
    //~FST,            FSTP,
    //~FSTCW,          FNSTCW,
    //~FSTENV,         FNSTENV,
    //~FSTSW,          FNSTSW,
    /**/
    NOP,            CWDE,           POPF,           PUSHF,
    HALT,           LOCK,           WAIT,           LEAVE,
    CMC,            CLC,            CLD,            CLI,
    STC,            STD,            STI,            RETN,
    RETF,
    /**/
    SYSCALL,        SYSENTER,       SYSRETN,        SYSEXITN,
    CPUID,          CDQE,           RSM,            UD2,
    EMMS,           PAUSE,          INVD,           WBINVD,
    WRMSR,          RDMSR,          RDPMC,          RDTSC,
    FNOP,           FCHS,           FSIN,           FCOS,
    FABS,           FTST,           FXAM,           FLD1,
    FLDL2T,         FLDL2E,         FLDPI,          FLDLG2,
    FLDLN2,         FLDZ,           F2XM1,          FYL2X,
    FPTAN,          FPATAN,         FXTRACT,        FPREM1,
    FDECSTP,        FINCSTP,        FPREM,          FYL2XP1,
    FSQRT,          FSINCOS,        FRNDINT,        FSCALE,
    FCOMPP,         FUCOMPP,        FNINIT,         FNCLEX,
    /**/
    MONITOR,        MWAIT,          SYSRETF,        SYSEXITF,
    LFENCE,         MFENCE,         SFENCE,         RDTSCP,
    FINIT,          FCLEX,
    /**/
    ENTER,          CALL,           IN,             OUT,
    JMP,            MOV,            POP,            PUSH,
    //~RETNPOP,        RETFPOP,
    // C2 [2B] CA [2B]
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
    //~LEA,            MOVBE,
    //~TEST,           XADD,           XCHG,
    BT,             BTS,            BTR,            BTC,
    BSF,            BSR,            BSWAP,
    //~SHLD,           SHRD,
    LOOP,           LOOPE,          LOOPNE,
    OPERATION_END
    //~REP,            REPE,           REPNE,
    //~INS,            OUTS,           LODS,           STOS,
    //~MOVS,           CMPS,           SCAS
    // The REP prefix can be added to the INS, OUTS, MOVS, LODS, and STOS instructions.
    // The REPE and REPNE prefixes can be added to the CMPS and SCAS instructions.
} operation_code_t;

typedef enum {
    GR0,            GR1,            GR2,            GR3,
    GR4,            GR5,            GR6,            GR7,
    GR8,            GR9,            GR10,           GR11,
    GR12,           GR13,           GR14,           GR15,
    GR_END
} general_register_code_t;

typedef enum {
    FR0,            FR1,            FR2,            FR3,
    FR4,            FR5,            FR6,            FR7,
    FR_END
} float_register_code_t;

typedef enum {
    VR0,            VR1,            VR2,            VR3,
    VR4,            VR5,            VR6,            VR7,
    VR8,            VR9,            VR10,           VR11,
    VR12,           VR13,           VR14,           VR15,
    VR16,           VR17,           VR18,           VR19,
    VR20,           VR21,           VR22,           VR23,
    VR24,           VR25,           VR26,           VR27,
    VR28,           VR29,           VR30,           VR31,
    VR_END
} vector_register_code_t; // We don't use them currently.

extern uint32_t   main_entry_point; /// X: REMOVE GLOBAL VARIABLES?
extern uint32_t   text_sector_size;
extern uint8_t  * text_sector_byte;
extern uint32_t   data_sector_size; // This is unused, and it should be used...
extern uint8_t  * data_sector_byte; // This is unused, and it should be used...

extern bool was_instruction_array_empty;

extern bool assemble(      uint32_t            count,
                     const uint32_t * restrict array);

#endif
