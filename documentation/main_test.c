#include <stdio.h>
#include <stdlib.h>

#include "../source/assembler.h"
#include "../source/assembler.c"

static unsigned int array [29] = {
	ADC, D64, REG, R1, REG, R2,
	ADC, D32, REG, R1, MEM, 12,
	ADC, D16, MEM, 12, REG, R10,
	ADC, D8, REG, R3, IMM, 0X77,
	INC, D16, REG, R0, LOCK
};

int main (void) {
	unsigned int index;
	token_array = malloc (144UL * sizeof (* token_array));

	assemble (29, array);

	printf ("> %u\n", token_count);

	for (index = 0; index < token_count; ++index) {
		printf ("%02X \n", token_array [index]);
	}

	free (token_array);

	return (0);
}
