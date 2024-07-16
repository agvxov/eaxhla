#include <stdio.h>
#include <stdlib.h>

unsigned int * t_array = NULL;
unsigned int   t_count = 0;

static void dump (const char * file_name);
void dump_variables();

#include "eaxhla.h"
#include "eaxhla.yy.h"
#include "eaxhla.tab.h"
#include "assembler.h"
#include "unix.h"
#include "debug.h"

void deinit(void) {
    extern void yyfree_leftovers(void);

    yyfree_leftovers();

    eaxhla_destroy();

	free (text_sector_byte);
	free (t_array);
}

char * yyfilename;

signed main(int argc, char * argv[]) {
    if (argc < 2) {
        printf("%s: <file>\n", argv[0]);
        return 1;
    }

	text_sector_byte = calloc (1440UL, sizeof (* text_sector_byte));
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

    if (!has_encountered_error) {
        dump_variables_to_assembler();
        assemble (t_count, t_array);
        debug_puts("Dumping output...");
        dump ("a.out");
    }

    if (was_instruction_array_empty) {
        issue_warning("the input did not contain any instructions");
    }

    deinit();

    return has_encountered_error;
}

void dump (const char * file_name) {
	elf_main_header (1, 1, 1, 0);
	elf_text_sector (text_sector_size);
	elf_data_sector (text_sector_size, 12);

	char meme [1024] = "";
	FILE * file = fopen (file_name, "w");

	fwrite (elf_main_header_byte, 1UL, ELF_MAIN_HEADER_SIZE, file);
	fwrite (elf_text_sector_byte, 1UL, ELF_TEXT_SECTOR_SIZE, file);
	fwrite (elf_data_sector_byte, 1UL, ELF_DATA_SECTOR_SIZE, file);

	//text
	fwrite (text_sector_byte, sizeof (* text_sector_byte),
	        (size_t) text_sector_size, file);

	// data
	fwrite ("heyo world!\n", 1UL, 12UL, file);

	snprintf (meme, 1023UL, "chmod +x %s", file_name);

	system (meme);

	fclose (file);
}
