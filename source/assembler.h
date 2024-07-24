#ifndef ASSEMBLER_H
#define ASSEMBLER_H

enum {
	D8,  // 8-bit
	D16, // 16-bit
	D32, // 32-bit
	D64  // 64-bit
};

enum {
	REL, // Relative
	REG, // Register
	MEM, // Memory
	IMM  // Immediate
};

enum {
	ASMDIRMEM, // Insert memory address directive.
	ASMDIRREL, // Insert relative address directive.
	ASMDIRIMM, // Insert immediate literal directive.
	ASMDIRREP, // Repeat directive.
	ADD,       // Add registers or memory addresses.
	OR,        // Bit-or registers or memory addresses.
	ADC,       // Add-carry registers or memory addresses.
	SBB,       // Subtract-borrow registers or memory addresses.
	AND,       // Bit-and registers or memory addresses.
	SUB,       // Subtract registers or memory addresses.
	XOR,       // Bit-xor registers or memory addresses.
	CMP,       // Compare registers or memory addresses.
	INC,       // Increment register or memory address.
	DEC,       // Decrement register or memory address.
	NOT,       // Bit-not register or memory address.
	NEG,       // Negate register or memory address.
	MUL,       // Multiply register or memory address.
	IMUL,      // Sign-multiply register or memory address.
	DIV,       // Divide register or memory address.
	IDIV,      // Sign-divide register or memory address.
	NOP,       // No operation, skip a cycle.
	RETN,      // Near return to calling procedure.
	RETF,      // Far return to calling procedure.
	LEAVE,     // Release the stack frame set up by enter.
	LOCK,      // Assert CPU lock signal during execution.
	HLT,       // Stop execution and set CPU to halt state.
	POPF,      // Pop stack into flags register (16/64 bit).
	PUSHF,     // Push flags register onto stack (16/64 bit).
	WAIT,      // Check for and handle pending float exceptions.
	CLC,       // Clear carry flag.
	CLD,       // Clear direction flag.
	CLI,       // Clear interrupt flag.
	STC,       // Set carry flag.
	STD,       // Set direction flag.
	STI,       // Set interrupt flag.
	CMC,       // Complement carry flag.
	INSB,      // Input 1 byte from IO port at DX to ES:(E)DI or RDI.
	INSD,      // Input 4 bytes from IO port at DX to ES:(E)DI or RDI.
	OUTSB,     // Output 1 byte from DS:(E)SI or RSI to IO port at DX.
	OUTSD,     // Output 4 bytes from DS:(E)SI or RSI to IO port at DX.
	CDQ,       // Sign-extend EAX into EDX:EAX.
	CWDE,      // Sign-extend AX into EAX.
	INAL,
	INEAX,
	INT3,
	IRETD,
	LODSB,
	LODSD,
	OUTAL,
	OUTEAX,
	SCASB,
	SCASD,
	STOSB,
	STOSD,
	SYSENTER,  // Execute fast call to ring 0 system procedure.
	SYSEXIT,   // Execute fast return to ring 3 userland procedure.
	SYSCALL,   // Execute OS system call handler at kernel.
	SYSRET,    // Execute return from previous system call.
	PAUSE,     // Hint CPU for usage of spin-wait loop.
	CPUID,     // Request CPU identification and information.
	EMMS,      // Empty MMX technology state (x87 FPU).
	RSM,       // Resume execution from system management mode.
	FNOP,      // No operation, skip cycle (x87 FPU).
	FCHS,      // Complement sign of st0.
	FABS,      // Replace st0 with its absolute value.
	FTST,      // Compare st0 with 0.0 literal.
	FXAM,      // Classify value or literal in st0.
	FLD1,      // Push 1.0 literal to FPU stack.
	FLDL2T,    // Push log2 (10) literal to FPU stack.
	FLDL2E,    // Push log2 (e) literal to FPU stack.
	FLDPI,     // Push pi literal to FPU stack.
	FLDLG2,    // Push log10 (2) literal to FPU stack.
	FLDLN2,    // Push loge (2) literal to FPU stack.
	FLDZ,      // Push 0.0 literal to FPU stack.
	F2XM1,     // Replace st0 with 2.0**st0 - 1.0.
	FYL2X,     // Multiply st1 with log2 (st0), pop FPU stack.
	FPTAN,     // Compute tan (st0), push 1.0 to FPU stack.
	FPATAN,    // Compute arctan (st1 / st0), pop FPU stack.
	FXTRACT,   // Separate st0 into FP elements, push FPU stack.
	FPREM1,    // Replace st0 with st0 % st1, round up.
	FDECSTP,   // Decrement FPU stack pointer.
	FINCSTP,   // Increment FPU stack pointer.
	FPREM,     // Replace st0 with st0 % st1.
	FYL2XP1,   // Multiply st1 with log2 (st0) + 1.0, pop FPU stack.
	FSQRT,     // Compute square root of st0.
	FSINCOS,   // Compure sin (st0), push cos (st0) to FPU stack.
	FRNDINT,   // Round st0 to integer value.
	FSCALE,    // Scale st0 by st1.
	FSIN,      // Compute sin (st0).
	FCOS,      // Compure cos (st0).
	INSW,
	OUTSW,
	CWD,
	CQO,
	CBW,
	CDQE,
	INVD,
	WBINVD,
	UD2,
	CLTS,
	INAX,
	IRETQ,
	LODSW,
	LODSQ,
	OUTAX,
	RDPMC,
	RDMSR,
	RDTSC,
	SCASW,
	SCASQ,
	STOSW,
	STOSQ,
	WRMSR,
	XLATB,
	ENTER,     // Create a stack frame for procedure call.
	CALL,      // Call near, relative or absolute procedure.
	IN,        // Input byte from 8-bit IO port to r0.
	OUT,       // Output byte from r0 to 8-bit IO port.
	JMP,       // Jump near, far, relative or absolute offset.
	JO,        // Conditional jump on overflow flag set.
	JNO,       // Conditional jump on not overflow flag set.
	JB,        // Conditional jump on below flag set.
	JAE,       // Conditional jump on above or equal flag set.
	JE,        // Conditional jump on equal flag set.
	JNE,       // Conditional jump on not equal flag set.
	JBE,       // Conditional jump on below or equal flag set.
	JA,        // Conditional jump on above flag set.
	JS,        // Conditional jump on negative flag set.
	JNS,       // Conditional jump on positive flag set.
	JPE,       // Conditional jump on parity even flag set.
	JPO,       // Conditional jump on parity odd flag set.
	JL,        // Conditional jump on less flag set.
	JGE,       // Conditional jump on greater or equal flag set.
	JLE,       // Conditional jump on less or equal flag set.
	JG,        // Conditional jump on greater flag set.
	MOV,       // Move registers or memory addresses.
	CMOVO,     // Conditional move on overflow flag set.
	CMOVNO,    // Conditional move on not overflow flag set.
	CMOVB,     // Conditional move on below flag set.
	CMOVAE,    // Conditional move on above or equal flag set.
	CMOVE,     // Conditional move on equal flag set.
	CMOVNE,    // Conditional move on not equal flag set.
	CMOVBE,    // Conditional move on below or equal flag set.
	CMOVA,     // Conditional move on above flag set.
	CMOVS,     // Conditional move on negative flag set.
	CMOVNS,    // Conditional move on positive flag set.
	CMOVPE,    // Conditional move on parity even flag set.
	CMOVPO,    // Conditional move on parity odd flag set.
	CMOVL,     // Conditional move on less flag set.
	CMOVGE,    // Conditional move on greater or equal flag set.
	CMOVLE,    // Conditional move on less or equal flag set.
	CMOVG,     // Conditional move on greater flag set.
	PUSH,      // Push a value to stack.
	POP,       // Pop a value from stack.
	BSWAP,     // Swap bytes according to little-endian rules.
	TEST,      // And registers or memory addresses, set flags.
	RCL,       // Rotate-carry bits left, repeat r1 times.
	RCR,       // Rotate-carry bits right, repeat r1 times.
	ROL,       // Rotate bits left, repeat r1 times.
	ROR,       // Rotate bits right, repeat r1 times.
	SHL,       // Shift bits left, repeat r1 times.
	SHR,       // Sign-shift bits right, repeat r1 times.
	SAL,       // Shift bits left, repeat r1 times.
	SAR,       // Shift bits right, repeat r1 times.
	REP,       // Repeat string instruction.
	REPE,      // Repeat string instruction while equal.
	REPNE,     // Repeat string instruction while not equal.
	REPZ,      // Repeat string instruction while zero.
	REPNZ,     // Repeat string instruction while not zero.
	LOOP,      // Decrement r1, jump near if not zero.
	LOOPE,     // Decrement r1, jump near if not zero and zflag.
	LOOPNE,    // Decrement r1, jump near if not zero and not zflag.
	MOVBE,     // Swap bytes and move register or memory address.
	XADD,      // Exchange and add register and memory address.
	XCHG,      // Exchange registers or memory addresses.
	LEA,       // Load effective address to register.
	POPCNT,    // Return count of bits set to 1 to register.
	BSF,       // Bit scan forward on register or memory address.
	BSR,       // Bit scan reverse on register or memory address.
	FADD,      // Add literal or FPU stack with st0.
	FMUL,      // Multiply literal or FPU stack with st0.
	FCOM,      // Cmopare literal or FPU stack with st0.
	FCOMP,     // Cmopare literal or FPU stack with st0 and pop.
	FSUB,      // Subtract literal or FPU stack with st0.
	FSUBR,     // Subtract reverse literal or FPU stack with st0.
	FDIV,      // Divide literal or FPU stack with st0.
	FDIVR      // Divide reverse literal or FPU stack with st0.
};

enum {
	GR0,  // Register: rax, eax, ax, al;
	GR1,  // Register: rcx, ecx, cx, cl;
	GR2,  // Register: rdx, edx, dx, dl;
	GR3,  // Register: rbx, ebx, bx, bl;
	GR4,  // Register: rsp, esp, sp, spl;
	GR5,  // Register: rbp, ebp, bp, bpl;
	GR6,  // Register: rsi, esi, si, sil;
	GR7,  // Register: rdi, edi, di, dil;
	GR8,  // Register: r8, r8d, r8w, r8b;
	GR9,  // Register: r9, r9d, r9w, r9b;
	GR10, // Register: r10, r10d, r10w, r10b;
	GR11, // Register: r11, r11d, r11w, r11b;
	GR12, // Register: r12, r12d, r12w, r12b;
	GR13, // Register: r13, r13d, r13w, r13b;
	GR14, // Register: r14, r14d, r14w, r14b;
	GR15  // Register: r15, r15d, r15w, r15b;
};

extern unsigned int    text_entry_point;
extern unsigned int    text_sector_size;
extern unsigned char * text_sector_byte;

extern int was_instruction_array_empty;

extern void assemble (unsigned int count, unsigned int * array);

#endif
