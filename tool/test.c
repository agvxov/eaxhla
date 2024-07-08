#include <stdio.h>
#include <stdlib.h>

#include "../source/assembler.h"
#include "../source/assembler.c"

static unsigned int array [] = {
	#include "include.txt"
};

int main (void) {
	unsigned int index;

	token_array = malloc (1024UL * 1024UL * sizeof (* token_array));

	assemble ((unsigned int) (sizeof (array) / sizeof (array [0])), array);

	for (index = 0; index < token_count; ++index) {
		printf ("%c%02X", (token_array [index] == 0x90) ? '\n' : ' ', token_array [index]);
	}

	free (token_array);

	return (0);
}
