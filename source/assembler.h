#ifndef ASSEMBLER_H
#define ASSEMBLER_H

typedef enum {
	D64,              D32,              D16,              D8,
} size_index;

typedef enum {
	NIL,              REG,              MEM,              IMM,
} type_index;

typedef enum {
	ADD,              OR,               ADC,              SBB,
	AND,              SUB,              XOR,              CMP,
	UMUL,             UDIV,             IMUL,             IDIV,
	INC,              DEC,              NOT,              NEG,
	ENTER,            LEAVE,            CALL,             RET,
	SYSENTER,         SYSEXIT,          SYSCALL,          SYSRET,
	JMP,              JPE,              JS,               JPO,
	JE,               JNE,              JZ,               JNZ,
	JA,               JNA,              JB,               JNB,
	MOV,              CMOVPE,           CMOVS,            CMOVPO,
	CMOVE,            CMOVNE,           CMOVZ,            CMOVNZ,
	CMOVA,            CMOVNA,           CMOVB,            CMOVNB,
	LOCK,             HLT,              IN,               OUT,
	PUSH,             POP,              BSWAP,            TEST,
	RCL,              RCR,              ROL,              ROR,
	SHL,              SHR,              SAL,              SAR,
	REP,              REPE,             REPNE,            REPZ,
	LOOP,             LOOPE,            LOOPNE,           PAUSE,
	XADD,             XCHG,             LEA,              POPCNT,
	INTI,             BSF,              BSR,              BOUND,
	FADD,             FSUB,             FMUL,             FDIV,
	FNOP,             FXAM,             FABS,             FSCALE,
	FSIN,             FCOS,             FSQRT,            FCHS,
	FXCH,             FREM,             FLDPI,            FLDZ,
	CPUID,            MOVBE,
} operation_index;

typedef enum {
	R0,               R1,               R2,               R3,
	R4,               R5,               R6,               R7,
	R8,               R9,               R10,              R11,
	R12,              R13,              R14,              R15,
} operand_index;

#endif
