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
	NOP, MOV, D32, REG, R2, IMM, 12,
	NOP, SYSCALL,
	NOP, MOV, D32, REG, R0, IMM, 60,
	NOP, MOV, D32, REG, R7, IMM, 60,
	NOP, SYSCALL,
	NOP,
	ASMDIRMEM, 0,
	ASMDIRIMM, D8, 72,
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

	elf_main_header (1, 1, 1, 0);
	elf_text_sector (70);
	elf_data_sector (70, 24);

	fwrite (elf_main_header_byte, 1UL, ELF_MAIN_HEADER_SIZE, file);
	fwrite (elf_text_sector_byte, 1UL, ELF_TEXT_SECTOR_SIZE, file);
	fwrite (elf_data_sector_byte, 1UL, ELF_DATA_SECTOR_SIZE, file);

	printf ("> %u\n", (unsigned int) (sizeof (array) / sizeof (array [0])));

	assemble ((unsigned int) (sizeof (array) / sizeof (array [0])), array);

	fwrite (text_sector_byte, 1UL, (size_t) text_sector_size, file);

	//~fwrite ("Heyo world!\n", 1UL, 12, file);

	free (text_sector_byte);

	fclose (file);

	return (0);
}

/*
7F_45_4C_46_02_01_01_03_00_00_00_00_00_00_00_00_
02_00_3E_00_01_00_00_00_B0_00_40_00_00_00_00_00_
40_00_00_00_00_00_00_00_00_00_00_00_00_00_00_00_
00_00_00_00_40_00_38_00_02_00_40_00_00_00_00_00_
01_00_00_00_05_00_00_00_00_00_00_00_00_00_00_00_
00_00_40_00_00_00_00_00_00_00_40_00_00_00_00_00_
D2_00_00_00_00_00_00_00_D2_00_00_00_00_00_00_00_
00_10_00_00_00_00_00_00_01_00_00_00_06_00_00_00_
D2_00_00_00_00_00_00_00_D2_10_40_00_00_00_00_00_
D2_10_40_00_00_00_00_00_0C_00_00_00_00_00_00_00_
0C_00_00_00_00_00_00_00_00_10_00_00_00_00_00_00_
90_B8_01_00_00_00_
90_BF_01_00_00_00_
90_BE_00_40_10_2A_
90_BA_0C_00_00_00_
90_0F_05_
90_B8_3C_00_00_00_
90_BF_3C_00_00_00_
90_0F_05_

--------------------------------------------------------------------------------
GOOD
--------------------------------------------------------------------------------
7F 45 4C 46 02 01 01 03 00 00 00 00 00 00 00 00
02 00 3E 00 01 00 00 00 B0 00 40 00 00 00 00 00
40 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 40 00 38 00 02 00 40 00 00 00 00 00
01 00 00 00 05 00 00 00 00 00 00 00 00 00 00 00
00 00 40 00 00 00 00 00 00 00 40 00 00 00 00 00
DA 00 00 00 00 00 00 00 DA 00 00 00 00 00 00 00
00 10 00 00 00 00 00 00 01 00 00 00 06 00 00 00
DA 00 00 00 00 00 00 00 DA 10 40 00 00 00 00 00
DA 10 40 00 00 00 00 00 0C 00 00 00 00 00 00 00
0C 00 00 00 00 00 00 00 00 10 00 00 00 00 00 00
90 B8 01 00 00 00
90 BF 01 00 00 00
90 BE DA 10 40 00
90 BA 0C 00 00 00
90 0F 05
90 B8 3C 00 00 00
90 BF 3C 00 00 00
90 0F 05 48 65 79 6F 20 77 6F 72 6C 64 21 0A
*/
