#include <stdio.h>
#include <stdlib.h>

#include "../source/assembler.h"
#include "../source/assembler.c"

int main (void) {
	token_array = malloc (144UL * sizeof (* token_array));

	assemble (ADC, D64, REG, R1, REG, R2);
	assemble (ADC, D32, REG, R1, MEM, 12);
	assemble (ADC, D16, MEM, 12, REG, R0);
	assemble (ADC, D8, REG, R3, IMM, 0X77);

	for (unsigned int index = 0; index < token_count; ++index) {
		printf ("%02X \n", token_array [index]);
	}

	free (token_array);

	return (0);
}

//~xor rcx rdx
//~48 31 D1
//~WORKS AS EXPECTED!

//~90 48 11 D1
//~90 13 0D 14 10 00 00
//~90 66 44 11 15 09 10 00 00
//~90 80 D3 77
