#include <stdio.h>
#include <stdlib.h>

#include "../source/assembler.h"
#include "../source/assembler.c"

int main (void) {
	token_array = malloc (144UL * sizeof (* token_array));

	assemble (OPERATION_ADD_F, SIZE_64B,
	          TYPE_REGISTER, OPERAND_REGISTER_1,
	          TYPE_REGISTER, OPERAND_REGISTER_2);

	assemble (OPERATION_ADD_F, SIZE_32B,
	          TYPE_REGISTER, OPERAND_REGISTER_1,
	          TYPE_VARIABLE, 12);

	assemble (OPERATION_ADD_F, SIZE_16B,
	          TYPE_VARIABLE, 12,
	          TYPE_REGISTER, OPERAND_REGISTER_A);

	assemble (OPERATION_ADD_F, SIZE_8B,
	          TYPE_REGISTER, OPERAND_REGISTER_3,
	          TYPE_CONSTANT, 0X77);

	for (int index = 0; index < token_count; ++index) {
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
