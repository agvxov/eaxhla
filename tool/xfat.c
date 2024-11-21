#define header                                                             \
"#include <stdio.h>\n"                                                     \
"#include <stdlib.h>\n"                                                    \
"#define ARENA_IMPLEMENTATION\n"                                           \
"#include \"/home/xolatile/Git/eaxcc/source/assembler.c\"\n" /* LNAO L: */ \
"static unsigned int array [] = { "

#define footer                                                                     \
"};\n"                                                                             \
"int main (void) {\n"                                                              \
"	unsigned int index;\n"                                                     \
"	text_sector_byte = calloc (1024ul, 1ul);\n"                                \
"	assemble ((unsigned int) (sizeof (array) / sizeof (array [0])), array);\n" \
"	for (index = 0; index < text_sector_size; ++index)\n"                      \
"		printf (\"%02X \", (unsigned char)text_sector_byte [index]);\n"    \
"	free (text_sector_byte);\n"                                                \
"	return (0);\n"                                                             \
"}\n"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char * * argv) {
	int i;

	FILE * file = fopen ("x.c", "w");

	fwrite (header, 1, sizeof (header) - 1, file);

	for (i = 1; i < argc; ++i) {
		fwrite (argv [i], 1, strlen (argv [i]), file);
		fwrite (", ",     1, 2,                 file);
	}

	fwrite (footer, 1, sizeof (footer) - 1, file);

	fclose (file);

	system ("gcc x.c && ./a.out && echo -- && rm a.out x.c");

	return (0);
}
