#ifndef ASSEMBLER_H

enum {
	D8,               D16,              D32,              D64
};

enum {
	REL,              REG,              MEM,              IMM
};

enum {
	ASMDIRMEM,        ASMDIRREL,        ASMDIRIMM,        ASMDIRREP,
	ADD,              OR,               ADC,              SBB,
	AND,              SUB,              XOR,              CMP,
	INC,              DEC,              NOT,              NEG,
	MUL,              IMUL,             DIV,              IDIV,
	NOP,              RETN,             RETF,             LEAVE,
	LOCK,             HLT,              POPF,             PUSHF,
	WAIT,
	SYSENTER,         SYSEXIT,          SYSCALL,          SYSRET,
	PAUSE,            CPUID,            EMMS,             RSM,
	FNOP,             FCHS,             FABS,             FTST,
	FXAM,             FLD1,             FLDL2T,           FLDL2E,
	FLDPI,            FLDLG2,           FLDLN2,           FLDZ,
	F2XM1,            FYL2X,            FPTAN,            FPATAN,
	FXTRACT,          FPREM1,           FDECSTP,          FINCSTP,
	FPREM,            FYL2XP1,          FSQRT,            FSINCOS,
	FRNDINT,          FSCALE,           FSIN,             FCOS,
	ENTER,            CALL,             IN,               OUT,
	JMP,
	JO,               JNO,              JB,               JAE,
	JE,               JNE,              JBE,              JA,
	JS,               JNS,              JPE,              JPO,
	JL,               JGE,              JLE,              JG,
	MOV,
	CMOVO,            CMOVNO,           CMOVB,            CMOVAE,
	CMOVE,            CMOVNE,           CMOVBE,           CMOVA,
	CMOVS,            CMOVNS,           CMOVPE,           CMOVPO,
	CMOVL,            CMOVGE,           CMOVLE,           CMOVG,
	PUSH,             POP,              BSWAP,            TEST,
	RCL,              RCR,              ROL,              ROR,
	SHL,              SHR,              SAL,              SAR,
	REP,              REPE,             REPNE,            REPZ,
	LOOP,             LOOPE,            LOOPNE,           MOVBE,
	XADD,             XCHG,             LEA,              POPCNT,
	INTI,             BSF,              BSR,              BOUND,
	FADD,             FMUL,             FCOM,             FCOMP,
	FSUB,             FSUBR,            FDIV,             FDIVR
};

enum {
	R0,               R1,               R2,               R3,
	R4,               R5,               R6,               R7,
	R8,               R9,               R10,              R11,
	R12,              R13,              R14,              R15
};

extern unsigned int    text_entry_point;
extern unsigned int    text_sector_size;
extern unsigned char * text_sector_byte;

extern int was_instruction_array_empty;

extern void assemble (unsigned int count, unsigned int * array);

#define ASSEMBLER_H
#endif
