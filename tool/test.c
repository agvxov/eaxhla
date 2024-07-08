#include <stdio.h>
#include <stdlib.h>

#include "../source/assembler.h"
#include "../source/assembler.c"

static unsigned int array [] = {
	NOP, ADD, D64, REG, R1, REG, R9,
	NOP, ADD, D64, REG, R1, MEM, 0,
	NOP, ADD, D64, REG, R1, IMM, 0X11223344,
	NOP, ADD, D64, MEM, 0,  REG, R1,
	NOP, ADD, D64, MEM, 0,  IMM, 0X11223344,
	NOP, OR,  D32, REG, R1, REG, R9,
	NOP, OR,  D32, REG, R1, MEM, 0,
	NOP, OR,  D32, REG, R1, IMM, 0X11223344,
	NOP, OR,  D32, MEM, 0,  REG, R1,
	NOP, OR,  D32, MEM, 0,  IMM, 0X11223344,
	NOP, ADC, D16, REG, R1, REG, R9,
	NOP, ADC, D16, REG, R1, MEM, 0,
	NOP, ADC, D16, REG, R1, IMM, 0X1122,
	NOP, ADC, D16, MEM, 0,  REG, R1,
	NOP, ADC, D16, MEM, 0,  IMM, 0X1122,
	NOP, SBB, D8,  REG, R1, REG, R9,
	NOP, SBB, D8,  REG, R1, MEM, 0,
	NOP, SBB, D8,  REG, R1, IMM, 0X11,
	NOP, SBB, D8,  MEM, 0,  REG, R1,
	NOP, SBB, D8,  MEM, 0,  IMM, 0X11,
	NOP, AND, D64, REG, R1, REG, R9,
	NOP, AND, D64, REG, R1, MEM, 0,
	NOP, AND, D64, REG, R1, IMM, 0X11223344,
	NOP, AND, D64, MEM, 0,  REG, R1,
	NOP, AND, D64, MEM, 0,  IMM, 0X11223344,
	NOP, SUB, D32, REG, R1, REG, R9,
	NOP, SUB, D32, REG, R1, MEM, 0,
	NOP, SUB, D32, REG, R1, IMM, 0X11223344,
	NOP, SUB, D32, MEM, 0,  REG, R1,
	NOP, SUB, D32, MEM, 0,  IMM, 0X11223344,
	NOP, XOR, D16, REG, R1, REG, R9,
	NOP, XOR, D16, REG, R1, MEM, 0,
	NOP, XOR, D16, REG, R1, IMM, 0X1122,
	NOP, XOR, D16, MEM, 0,  REG, R1,
	NOP, XOR, D16, MEM, 0,  IMM, 0X1122,
	NOP, CMP, D8,  REG, R1, REG, R9,
	NOP, CMP, D8,  REG, R1, MEM, 0,
	NOP, CMP, D8,  REG, R1, IMM, 0X11,
	NOP, CMP, D8,  MEM, 0,  REG, R1,
	NOP, CMP, D8,  MEM, 0,  IMM, 0X11,
	//~NOP, D32, INC, REG, R1,
	//~NOP, D32, INC, MEM, 0,
	//~NOP, D32, DEC, REG, R1,
	//~NOP, D32, DEC, MEM, 0,
	//~NOP, D32, NOT, REG, R1,
	//~NOP, D32, NOT, MEM, 0,
	//~NOP, D32, NEG, REG, R1,
	//~NOP, D32, NEG, MEM, 0,
	//~NOP, D32, UMUL, REG, R9,
	//~NOP, D32, UMUL, MEM, 0,
	//~NOP, D32, IMUL, REG, R9,
	//~NOP, D32, IMUL, MEM, 0,
	//~NOP, D32, UDIV, REG, R9,
	//~NOP, D32, UDIV, MEM, 0,
	//~NOP, D32, IDIV, REG, R9,
	//~NOP, D32, IDIV, MEM, 0,
	//~NOP, RETN,
	//~NOP, RETF,
	//~NOP, LEAVE,
	//~NOP, LOCK,
	//~NOP, HLT,
	//~NOP, SYSENTER,
	//~NOP, SYSEXIT,
	//~NOP, SYSCALL,
	//~NOP, SYSRET,
	//~NOP, PAUSE,
	//~NOP, CPUID,
	//~NOP, JO, 0X77FFFFFF,
	//~NOP, JNO, 0X77FFFFFF,
	//~NOP, JB, 0X77FFFFFF,
	//~NOP, JAE, 0X77FFFFFF,
	//~NOP, JE, 0X77FFFFFF,
	//~NOP, JNE, 0X77FFFFFF,
	//~NOP, JBE, 0X77FFFFFF,
	//~NOP, JA, 0X77FFFFFF,
	//~NOP, JS, 0X77FFFFFF,
	//~NOP, JNS, 0X77FFFFFF,
	//~NOP, JPE, 0X77FFFFFF,
	//~NOP, JPO, 0X77FFFFFF,
	//~NOP, JL, 0X77FFFFFF,
	//~NOP, JGE, 0X77FFFFFF,
	//~NOP, JLE, 0X77FFFFFF,
	//~NOP, JG, 0X77FFFFFF,
	//~NOP, CMOVO, D64, REG, R0, REG, R1,
	//~NOP, CMOVNO,D64,  REG, R0, REG, R1,
	//~NOP, CMOVB, D64, REG, R0, REG, R1,
	//~NOP, CMOVAE, D64, REG, R0, REG, R1,
	//~NOP, CMOVE, D64, REG, R0, REG, R1,
	//~NOP, CMOVNE, D64, REG, R0, REG, R1,
	//~NOP, CMOVBE, D32, REG, R0, REG, R1,
	//~NOP, CMOVA, D32, REG, R0, REG, R1,
	//~NOP, CMOVS, D32, REG, R0, REG, R1,
	//~NOP, CMOVNS, D32, REG, R0, REG, R1,
	//~NOP, CMOVPE, D32, REG, R0, REG, R1,
	//~NOP, CMOVPO, D32, REG, R0, REG, R1,
	//~NOP, CMOVL, D32, REG, R0, REG, R1,
	//~NOP, CMOVGE, D32, REG, R0, REG, R1,
	//~NOP, CMOVLE, D16, REG, R0, REG, R1,
	//~NOP, CMOVG, D16, REG, R0, REG, R1
};

int main (void) {
	unsigned int index;

	token_array = malloc (1440UL * sizeof (* token_array));

	assemble ((unsigned int) (sizeof (array) / sizeof (array [0])), array);

	printf ("> %u\n", token_count);

	for (index = 0; index < token_count; ++index) {
		printf ("%02X ", token_array [index]);
	}

	free (token_array);

	return (0);
}

/*
XOLATILE:                               FLAT:
4C 01 C9                                4C 01 C9
48 03 0D FF FF FF FF                    48 03 0D 52 12 00 00
48 81 C2 44 33 22 11                    48 81 C1 44 33 22 11
48 01 0D FF FF FF FF                    48 01 0D 42 12 00 00
48 81 05 FF FF FF FF 44 33 22 11        48 81 05 36 12 00 00 44 33 22 11
44 09 C9                                44 09 C9
0B 0D FF FF FF FF                       0B 0D 27 12 00 00
81 CA 44 33 22 11                       81 C9 44 33 22 11
09 0D FF FF FF FF                       09 0D 19 12 00 00
81 0D FF FF FF FF 44 33 22 11           81 0D 0E 12 00 00 44 33 22 11
66 44 11 C9                             66 44 11 C9
66 13 0D FF FF FF FF                    66 13 0D FF 11 00 00
66 81 D2 22 11                          66 81 D1 22 11
66 11 0D FF FF FF FF                    66 11 0D F1 11 00 00
66 81 15 FF FF FF FF 22 11              66 81 15 E7 11 00 00 22 11
44 18 C9                                44 18 C9
1A 0D FF FF FF FF                       1A 0D DB 11 00 00
80 D9 11                                80 D9 11
18 0D FF FF FF FF                       18 0D D0 11 00 00
80 1C FF FF FF FF 11                    80 1D C8 11 00 00 11
4C 21 C9                                4C 21 C9
48 23 0D FF FF FF FF                    48 23 0D C3 11 00 00
48 81 E2 44 33 22 11                    48 81 E1 44 33 22 11
48 21 0D FF FF FF FF                    48 21 0D B3 11 00 00
48 81 25 FF FF FF FF 44 33 22 11        48 81 25 A7 11 00 00 44 33 22 11
44 29 C9                                44 29 C9
2B 0D FF FF FF FF                       2B 0D 98 11 00 00
81 EA 44 33 22 11                       81 E9 44 33 22 11
29 0D FF FF FF FF                       29 0D 8A 11 00 00
81 2D FF FF FF FF 44 33 22 11           81 2D 7F 11 00 00 44 33 22 11
66 44 31 C9                             66 44 31 C9
66 33 0D FF FF FF FF                    66 33 0D 70 11 00 00
66 81 F2 22 11                          66 81 F1 22 11
66 31 0D FF FF FF FF                    66 31 0D 62 11 00 00
66 81 35 FF FF FF FF 22 11              66 81 35 58 11 00 00 22 11
44 38 C9                                44 38 C9
3A 0D FF FF FF FF                       3A 0D 4C 11 00 00
80 F9 11                                80 F9 11
38 0D FF FF FF FF                       38 0D 41 11 00 00
80 3C FF FF FF FF 11                    ****************************************

FASM:
90 4C 01 C9
90 48 03 0D 52 12 00 00
90 48 81 C1 44 33 22 11
90 48 01 0D 42 12 00 00
90 48 81 05 36 12 00 00 44 33 22 11
90 44 09 C9
90 0B 0D 27 12 00 00
90 81 C9 44 33 22 11
90 09 0D 19 12 00 00
90 81 0D 0E 12 00 00 44 33 22 11
90 66 44 11 C9
90 66 13 0D FF 11 00 00
90 66 81 D1 22 11
90 66 11 0D F1 11 00 00
90 66 81 15 E7 11 00 00 22 11
90 44 18 C9
90 1A 0D DB 11 00 00
90 80 D9 11
90 18 0D D0 11 00 00
90 80 1D C8 11 00 00 11
90 4C 21 C9
90 48 23 0D C3 11 00 00
90 48 81 E1 44 33 22 11
90 48 21 0D B3 11 00 00
90 48 81 25 A7 11 00 00 44 33 22 11
90 44 29 C9
90 2B 0D 98 11 00 00
90 81 E9 44 33 22 11
90 29 0D 8A 11 00 00
90 81 2D 7F 11 00 00 44 33 22 11
90 66 44 31 C9
90 66 33 0D 70 11 00 00
90 66 81 F1 22 11
90 66 31 0D 62 11 00 00
90 66 81 35 58 11 00 00 22 11
90 44 38 C9
90 3A 0D 4C 11 00 00
90 80 F9 11
90 38 0D 41 11 00 00
*/
