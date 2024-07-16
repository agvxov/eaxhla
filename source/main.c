#include <stdio.h>
#include <stdlib.h>

#include "eaxhla.h"
#include "eaxhla.yy.h"
#include "eaxhla.tab.h"
#include "compile.h"
#include "assembler.h"
#include "debug.h"

void deinit(void) {
    extern void yyfree_leftovers(void);

    yyfree_leftovers();

    eaxhla_destroy();

	free (text_sector_byte);
	free (token_array);
}

signed main(int argc, char * argv[]) {
    if (argc < 2) {
        printf("%s: <file>\n", argv[0]);
        return 1;
    }

	text_sector_byte = calloc (1440UL, sizeof (* text_sector_byte));
	token_array      = calloc (1440UL, sizeof (* token_array));

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

    debug_dump_variables();

    if (!has_encountered_error) {
        compile();
    }

    if (was_instruction_array_empty) {
        issue_warning("the input did not contain any instructions");
    }

    deinit();

    return has_encountered_error;
}
