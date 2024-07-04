#include <stdio.h>
#include <stdlib.h>

#include "../source/assembler.h"
#include "../source/assembler.c"

int main (void) {
	unsigned int index;

	output_array = malloc (144UL * sizeof (* output_array));

	build_regular (ADC, D64, REG, R1, REG, R2);
	build_regular (ADC, D32, REG, R1, MEM, 12);
	build_regular (ADC, D16, MEM, 12, REG, R10);
	build_regular (ADC, D8, REG, R3, IMM, 0X77);
	build_special_1 (LOCK);
	build_special_2 (PAUSE);

	for (index = 0; index < output_count; ++index) {
		printf ("%02X \n", output_array [index]);
	}

	free (output_array);

	return (0);
}
