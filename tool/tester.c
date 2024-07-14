#include <stdio.h>
#include <stdlib.h>

#include "../source/assembler.h"
#include "../source/assembler.c"
#include "../source/unix.h"
#include "../source/unix.c"

static unsigned int array [] = {
	NOP, MOV, D32, REG, R0, IMM, 1,
	NOP, MOV, D32, REG, R7, IMM, 1,
	NOP, MOV, D32, REG, R6, REL, 0,
	NOP, MOV, D32, REG, R2, IMM, 12,
	NOP, SYSCALL,
	NOP, MOV, D32, REG, R0, IMM, 1,
	NOP, MOV, D32, REG, R7, IMM, 1,
	NOP, MOV, D32, REG, R6, REL, 1,
	NOP, MOV, D32, REG, R2, IMM, 20,
	NOP, SYSCALL,
	NOP, MOV, D32, REG, R0, IMM, 1,
	NOP, MOV, D32, REG, R7, IMM, 1,
	NOP, MOV, D32, REG, R6, REL, 2,
	NOP, MOV, D32, REG, R2, IMM, 12,
	NOP, SYSCALL,
	NOP, MOV, D32, REG, R0, IMM, 60,
	NOP, MOV, D32, REG, R7, IMM, 60,
	NOP, SYSCALL,
	NOP,
	ASMDIRMEM, 0,
	ASMDIRIMM, D8, 72, /* ARRAY SUPPORT */
	ASMDIRIMM, D8, 101,
	ASMDIRIMM, D8, 121,
	ASMDIRIMM, D8, 111,
	ASMDIRIMM, D8, 32,
	ASMDIRIMM, D8, 119,
	ASMDIRIMM, D8, 111,
	ASMDIRIMM, D8, 114,
	ASMDIRIMM, D8, 108,
	ASMDIRIMM, D8, 100,
	ASMDIRIMM, D8, 33,
	ASMDIRIMM, D8, 10,
	ASMDIRMEM, 1,
	ASMDIRIMM, D8, 67,
	ASMDIRIMM, D8, 121,
	ASMDIRIMM, D8, 97,
	ASMDIRIMM, D8, 97,
	ASMDIRIMM, D8, 32,
	ASMDIRIMM, D8, 102,
	ASMDIRIMM, D8, 117,
	ASMDIRIMM, D8, 99,
	ASMDIRIMM, D8, 107,
	ASMDIRIMM, D8, 105,
	ASMDIRIMM, D8, 110,
	ASMDIRIMM, D8, 103,
	ASMDIRIMM, D8, 32,
	ASMDIRIMM, D8, 119,
	ASMDIRIMM, D8, 111,
	ASMDIRIMM, D8, 114,
	ASMDIRIMM, D8, 108,
	ASMDIRIMM, D8, 100,
	ASMDIRIMM, D8, 33,
	ASMDIRIMM, D8, 10,
	ASMDIRMEM, 2,
	ASMDIRIMM, D8, 77,
	ASMDIRIMM, D8, 101,
	ASMDIRIMM, D8, 109,
	ASMDIRIMM, D8, 101,
	ASMDIRIMM, D8, 32,
	ASMDIRIMM, D8, 119,
	ASMDIRIMM, D8, 111,
	ASMDIRIMM, D8, 114,
	ASMDIRIMM, D8, 108,
	ASMDIRIMM, D8, 100,
	ASMDIRIMM, D8, 33,
	ASMDIRIMM, D8, 10
};

int main (void) {
	FILE * file = NULL;

	text_sector_byte = malloc (1024UL * 1024UL * sizeof (* text_sector_byte));

	file = fopen ("run_me_please", "w+");

	assemble ((unsigned int) (sizeof (array) / sizeof (array [0])), array);

	elf_main_header (1, 1, 1, 0);
	elf_text_sector (text_sector_size);
	elf_data_sector (text_sector_size, 36+8);

	fwrite (elf_main_header_byte, 1UL, ELF_MAIN_HEADER_SIZE, file);
	fwrite (elf_text_sector_byte, 1UL, ELF_TEXT_SECTOR_SIZE, file);
	fwrite (elf_data_sector_byte, 1UL, ELF_DATA_SECTOR_SIZE, file);

	printf ("> %u\n", (unsigned int) (sizeof (array) / sizeof (array [0])));

	fwrite (text_sector_byte, 1UL, (size_t) text_sector_size, file);

	free (text_sector_byte);

	fclose (file);

	return (0);
}
