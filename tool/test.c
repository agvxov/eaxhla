#include <stdio.h>
#include <stdlib.h>

#include "../source/assembler.h"
#include "../source/assembler.c"

static unsigned int array [] = {
	#include "include.txt"
};

int main (void) {
	unsigned int index;

	text_sector_byte = malloc (1024UL * 1024UL * sizeof (* text_sector_byte));

	assemble ((unsigned int) (sizeof (array) / sizeof (array [0])), array);

	for (index = 0; index < text_sector_size; ++index) {
		printf ("%c%02X", (text_sector_byte [index] == 0x90) ? '\n' : ' ', text_sector_byte [index]);
	}

	free (text_sector_byte);

	return (0);
}
