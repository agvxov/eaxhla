#include <xolatile/xtandard.h>

#define header                                                             \
"#include <stdio.h>\n"                                                     \
"#include <stdlib.h>\n"                                                    \
"#include \"/home/xolatile/Git/eaxcc/source/assembler.h\"\n" /* LNAO L: */ \
"#include \"/home/xolatile/Git/eaxcc/source/assembler.c\"\n" /* LNAO L: */ \
"static unsigned int array [] = { "

#define footer                                                                     \
"};\n"                                                                             \
"int main (void) {\n"                                                              \
"	unsigned int index;\n"                                                     \
"	text_sector_byte = calloc (1024ul, 1ul);\n"                                \
"	assemble ((unsigned int) (sizeof (array) / sizeof (array [0])), array);\n" \
"	for (index = 0; index < text_sector_size; ++index)\n"                      \
"		printf (\"%02X \", text_sector_byte [index]);\n"                   \
"	free (text_sector_byte);\n"                                                \
"	return (0);\n"                                                             \
"}\n"

int main (int argc, char * * argv) {
	int file, i;

	file = file_open ("x.c", O_RDWR | O_CREAT | O_TRUNC);

	file_echo (file, header);

	for (i = 1; i < argc; ++i) {
		file_echo (file, argv [i]);
		file_echo (file, ", ");
	}

	file_echo (file, footer);

	file = file_close (file);

	execute ("gcc -g x.c && ./a.out && echo -- && rm a.out x.c");

	return (log_success);
}
