/* Ignore this file, it's for my autistic way of testing. -- xolatile */

#include <xolatile/xtandard.c>

#define header                            \
"#include <stdio.h>\n"                    \
"#include <stdlib.h>\n"                   \
"#include \"../source/assembler.h\"\n"    \
"#include \"../source/assembler.c\"\n"    \
"static unsigned int array [] = { "

#define footer                                                                     \
"};\n"                                                                             \
"int main (void) {\n"                                                              \
"	unsigned int index;\n"                                                     \
"	token_array = malloc (144UL * sizeof (* token_array));\n"                  \
"	assemble ((unsigned int) (sizeof (array) / sizeof (array [0])), array);\n" \
"	for (index = 0; index < token_count; ++index)\n"                           \
"		printf (\"%02X \", token_array [index]);\n"                        \
"	free (token_array);\n"                                                     \
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

	execute ("cat x.c && gcc x.c && ./a.out && echo -- && rm a.out x.c");

	return (log_success);
}
