/* Ignore this file, it's for my autistic way of testing. -- xolatile */

#include <xolatile/xtandard.c>

int main (int argc, char * * argv) {
	int file, i;

	file = file_open ("qfat.asm", O_RDWR | O_CREAT | O_TRUNC);

	file_echo (file, "format ELF64 executable 3\n");
	file_echo (file, "segment readable executable\n");
	file_echo (file, "entry $\n");
	file_echo (file, "fff:\n");
	file_echo (file, "nop\n");

	for (i = 1; i < argc; ++i) {
		if (argv [i] [0] == '+') {
			file_echo (file, "\nnop\n");
			continue;
		}

		file_echo (file, " ");
		file_echo (file, argv [i]);
	}

	file_echo (file, "\nnop\n");
	file_echo (file, "segment readable writable\n");
	file_echo (file, "nnn:\n");
	file_echo (file, "x1: db 11h\n");
	file_echo (file, "x2: dw 1122h\n");
	file_echo (file, "x4: dd 11223344h\n");
	file_echo (file, "x8: dq 1122334455667788h\n");

	file = file_close (file);

	execute ("fasm qfat.asm qfat.obj && xop qfat.obj && rm qfat.asm qfat.obj");

	return (log_success);
}
