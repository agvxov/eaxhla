#ifndef ASSEMBLER_H
#define ASSEMBLER_H

enum {
	D8,             D16,            D32,            D64
};

enum {
	REL,            REG,            MEM,            IMM
};

enum {
	ASMDIRMEM,      ASMDIRREL,      ASMDIRIMM,      ASMDIRREP,
	ADD,            OR,             ADC,            SBB,            AND,            SUB,            XOR,            CMP,
	INC,            DEC,            NOT,            NEG,            MUL,            IMUL,           DIV,            IDIV,
	FADD,           FMUL,           FCOM,           FCOMP,          FSUB,           FSUBR,          FDIV,           FDIVR,
	NOP,            RETN,           RETF,           LEAVE,          POPF,           PUSHF,
	SYSCALL,        CPUID,          FNOP,           FCHS,           FABS,           FTST,           FXAM,           FLD1,
	FLDL2T,         FLDL2E,         FLDPI,          FLDLG2,         FLDLN2,         FLDZ,           F2XM1,          FYL2X,
	FPTAN,          FPATAN,         FXTRACT,        FPREM1,         FDECSTP,        FINCSTP,        FPREM,          FYL2XP1,
	FSQRT,          FSINCOS,        FRNDINT,        FSCALE,         FSIN,           FCOS,
	ENTER,          CALL,           IN,             OUT,            JMP,            MOV,            POP,            PUSH,
	JO,             JNO,            JB,             JAE,            JE,             JNE,            JBE,            JA,
	JS,             JNS,            JPE,            JPO,            JL,             JGE,            JLE,            JG,
	CMOVO,          CMOVNO,         CMOVB,          CMOVAE,         CMOVE,          CMOVNE,         CMOVBE,         CMOVA,
	CMOVS,          CMOVNS,         CMOVPE,         CMOVPO,         CMOVL,          CMOVGE,         CMOVLE,         CMOVG,
	BSWAP,          TEST,           XCHG,           LEA,            BSF,            BSR,
	RCL,            RCR,            ROL,            ROR,            SHL,            SHR,            SAL,            SAR,
	REP,            REPE,           REPNE,          REPZ,           REPNZ,          LOOP,           LOOPE,          LOOPNE
};

enum {
	GR0,            GR1,            GR2,            GR3,            GR4,            GR5,            GR6,            GR7,
	GR8,            GR9,            GR10,           GR11,           GR12,           GR13,           GR14,           GR15
};

extern unsigned int    text_entry_point;
extern unsigned int    text_sector_size;
extern unsigned char * text_sector_byte;

extern int was_instruction_array_empty;

extern void assemble (unsigned int count, unsigned int * array);

#endif
