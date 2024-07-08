#include <stdio.h>
#include <stdlib.h>

#include "../source/assembler.h"
#include "../source/assembler.c"

static unsigned int array [] = {
	ADD, D64, REG, R1, REG, R9,
	ADD, D64, REG, R1, MEM, 0,
	ADD, D64, REG, R1, IMM, 0X11223344U,
	ADD, D64, MEM, 0,  REG, R9,
	ADD, D64, MEM, 0,  IMM, 0X11223344U,
	OR,  D32, REG, R1, REG, R9,
	OR,  D32, REG, R1, MEM, 0,
	OR,  D32, REG, R1, IMM, 0X11223344U,
	OR,  D32, MEM, 0,  REG, R9,
	OR,  D32, MEM, 0,  IMM, 0X11223344U,
	ADC, D16, REG, R1, REG, R9,
	ADC, D16, REG, R1, MEM, 0,
	ADC, D16, REG, R1, IMM, 0X11223344U,
	ADC, D16, MEM, 0,  REG, R9,
	ADC, D16, MEM, 0,  IMM, 0X11223344U,
	SBB, D8,  REG, R1, REG, R9,
	SBB, D8,  REG, R1, MEM, 0,
	SBB, D8,  REG, R1, IMM, 0X11223344U,
	SBB, D8,  MEM, 0,  REG, R9,
	SBB, D8,  MEM, 0,  IMM, 0X11223344U
};

int main (void) {
	unsigned int index;

	token_array = malloc (144UL * sizeof (* token_array));

	assemble ((unsigned int) (sizeof (array) / sizeof (array [0])), array);

	printf ("> %u\n", token_count);

	for (index = 0; index < token_count; ++index) {
		printf ("%02X ", token_array [index]);
	}

	free (token_array);

	return (0);
}
