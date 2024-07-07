#include <stdio.h>
#include <stdlib.h>

static unsigned int * t_array = NULL;
static unsigned int   t_count = 0;

extern void append_token (int t);
extern void append_instruction_t6 (int t6, int w, int d, int r, int s, int i);
extern void append_instruction_t1 (int t1);

static void dump (const char * file_name);

#include "eaxhla.h"
#include "eaxhla.yy.h"
#include "eaxhla.tab.h"
#include "assembler.h"

extern void yyfree_leftovers(void);

char * yyfilename;

signed main(int argc, char * argv[]) {
    if (argc < 2) {
        printf("%s: <file>\n", argv[0]);
        return 1;
    }

	token_array = calloc (1440UL, sizeof (* token_array));
	t_array     = calloc (1440UL, sizeof (* t_array));

    #if DEBUG == 1
        yydebug = 1;
    #endif

    yyfilename = argv[1];

    yyin = fopen(yyfilename, "r");

    if (eaxhla_init()) {
        puts("Initialization failed");
        return 1;
    }

    yyparse();

	assemble (t_count, t_array);

	dump ("test_me_please");

    yyfree_leftovers();

    eaxhla_destroy();

	free (token_array);
	free (t_array);

    return 0;
}

void append_token (int t) {
	t_array [t_count] = t;
	t_count += 1;
}

void append_instruction_t6 (int t6, int w, int d, int r, int s, int i) {
	append_token (t6); // operation
	append_token (w);  // width
	append_token (d);  // destination
	append_token (r);  // register
	append_token (s);  // source
	append_token (i);  // immediate
}

void append_instruction_t1 (int t1) {
	append_token (t1); // operation
}

void dump (const char * file_name) {
/* DO NOT TRY THIS AT HOME! */
#define hackery \
"\x7F\x45\x4C\x46\x02\x01\x01\x03"\
"\x00\x00\x00\x00\x00\x00\x00\x00"\
"\x02\x00\x3E\x00\x01\x00\x00\x00"\
"\xB0\x00\x40\x00\x00\x00\x00\x00"\
"\x40\x00\x00\x00\x00\x00\x00\x00"\
"\x00\x00\x00\x00\x00\x00\x00\x00"\
"\x00\x00\x00\x00\x40\x00\x38\x00"\
"\x02\x00\x40\x00\x00\x00\x00\x00"\
"\x01\x00\x00\x00\x05\x00\x00\x00"\
"\x00\x00\x00\x00\x00\x00\x00\x00"\
"\x00\x00\x40\x00\x00\x00\x00\x00"\
"\x00\x00\x40\x00\x00\x00\x00\x00"\
"\xC5\x00\x00\x00\x00\x00\x00\x00"\
"\xC5\x00\x00\x00\x00\x00\x00\x00"\
"\x00\x10\x00\x00\x00\x00\x00\x00"\
"\x01\x00\x00\x00\x06\x00\x00\x00"\
"\xC5\x00\x00\x00\x00\x00\x00\x00"\
"\xC5\x10\x40\x00\x00\x00\x00\x00"\
"\xC5\x10\x40\x00\x00\x00\x00\x00"\
"\x05\x00\x00\x00\x00\x00\x00\x00"\
"\x05\x00\x00\x00\x00\x00\x00\x00"\
"\x00\x10\x00\x00\x00\x00\x00\x00"

	char meme [1024] = "";
	FILE * file = fopen (file_name, "w");

	fwrite (hackery, 1UL, 64UL + 2UL * 56UL, file);
	fwrite (token_array, sizeof (* token_array), (size_t) token_count, file);
	fwrite ("heyo world!\n", 1UL, 12UL, file);

	snprintf (meme, 1023UL, "chmod +x %s", file_name);

	system (meme);

	fclose (file);
}
