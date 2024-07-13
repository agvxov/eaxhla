#include <stdio.h>
#include <stdlib.h>

#include "../source/assembler.h"
#include "../source/assembler.c"
#include "../source/unix.h"
#include "../source/unix.c"

static unsigned int array [] = {
	NOP, MOV, D32, REG, R0, IMM, 1,
	NOP, MOV, D32, REG, R7, IMM, 1,
	//~NOP, MOV, D32, REG, R6, REL, 0,
	NOP, MOV, D32, REG, R6, IMM, 0x004010DA,
	NOP, MOV, D32, REG, R2, IMM, 12,
	NOP, SYSCALL,
	NOP, MOV, D32, REG, R0, IMM, 60,
	NOP, MOV, D32, REG, R7, IMM, 60,
	NOP, SYSCALL
	//~ASMDIRMEM, 0,
	//~ASMDIRIMM, D8, 12,
	//~72, 101, 121, 111, 32, 119, 111, 114, 108, 100, 10, 0
};

int main (void) {
	FILE * file = NULL;

	text_sector_byte = malloc (1024UL * 1024UL * sizeof (* text_sector_byte));

	file = fopen ("run_me_please", "w+");

	elf_main_header (1, 1, 1, 0);
	elf_text_sector (34);
	elf_data_sector (34, 12);

	fwrite (elf_main_header_byte, 1UL, ELF_MAIN_HEADER_SIZE, file);
	fwrite (elf_text_sector_byte, 1UL, ELF_TEXT_SECTOR_SIZE, file);
	fwrite (elf_data_sector_byte, 1UL, ELF_DATA_SECTOR_SIZE, file);

	printf ("> %u\n", (unsigned int) (sizeof (array) / sizeof (array [0])));

	assemble ((unsigned int) (sizeof (array) / sizeof (array [0])), array);

	fwrite (text_sector_byte, 1UL, (size_t) text_sector_size, file);

	fwrite ("Heyo world!\n", 1UL, 12, file);

	free (text_sector_byte);

	fclose (file);

	return (0);
}

/*
7F 45 4C 46 02 01 01 03 00 00 00 00 00 00 00 00
02 00 3E 00 01 00 00 00 B0 00 40 00 00 00 00 00
40 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 40 00 38 00 02 00 40 00 00 00 00 00
01 00 00 00 05 00 00 00 00 00 00 00 00 00 00 00
00 00 40 00 00 00 00 00 00 00 40 00 00 00 00 00
D2 00 00 00 00 00 00 00 D2 00 00 00 00 00 00 00
00 10 00 00 00 00 00 00 01 00 00 00 06 00 00 00
D2 00 00 00 00 00 00 00 D2 10 40 00 00 00 00 00
D2 10 40 00 00 00 00 00 0C 00 00 00 00 00 00 00
0C 00 00 00 00 00 00 00 00 10 00 00 00 00 00 00
C0 01 00 00 00
C7 01 00 00 00
C6 00 10 40 00
C2 0C 00 00 00
0F 05
C0 3C 00 00 00
C7 3C 00 00 00
0F 05
48 65 79 6F 20 77 6F 72 6C 64 21 0A
*/
