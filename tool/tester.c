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
	ASMDIRIMM, D8, 12, 72, 101, 121, 111, 32, 119, 111, 114, 108, 100, 33, 10,
	ASMDIRMEM, 1,
	ASMDIRIMM, D8, 20, 67, 121, 97, 97, 32, 102, 117, 99, 107, 105, 110,
		103, 32, 119, 111, 114, 108, 100, 33, 10,
	ASMDIRMEM, 2,
	ASMDIRIMM, D8, 12, 77, 101, 109, 101, 32, 119, 111, 114, 108, 100, 33, 10
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
