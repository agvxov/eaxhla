#include <stdio.h>
#include <stdlib.h>

#include "../source/assembler.h"
#include "../source/assembler.c"

#define COUNT (29+24)

static unsigned int array [COUNT] = {
	ADC, D64, REG, R1, REG, R2,
	ADC, D32, REG, R1, MEM, 12,
	ADC, D16, MEM, 12, REG, R10,
	ADC, D8, REG, R3, IMM, 0X77,
	INC, D16, REG, R0, LOCK,
	CMOVG, D64, REG, R1, REG, R1,
	CMOVG, D64, REG, R1, REG, R9,
	CMOVG, D64, REG, R9, REG, R1,
	CMOVG, D64, REG, R9, REG, R9
};

int main (void) {
	unsigned int index;
	token_array = malloc (144UL * sizeof (* token_array));

	assemble (COUNT, array);

	printf ("> %u\n", token_count);

	for (index = 0; index < token_count; ++index) {
		printf ("%02X \n", token_array [index]);
	}

	free (token_array);

	return (0);
}
