#include <stdio.h>
#include <stdlib.h>

#include "eaxhla.h"
#include "eaxhla.yy.h"
#include "eaxhla.tab.h"
#include "compile.h"
#include "assembler.h"
#include "debug.h"

int init(void) {
    eaxhla_init();
    compile_init();
    return 0;
}

void deinit(void) {
    extern void yyfree_leftovers(void);

    yyfree_leftovers();

    eaxhla_deinit();
    compile_deinit();
}

signed main(int argc, char * argv[]) {
    if (argc < 2) {
        printf("%s: <file>\n", argv[0]);
        return 1;
    }

    #if DEBUG == 1
        yydebug = 1;
    #endif

    yyfilename = argv[1];

    yyin = fopen(yyfilename, "r");

    if (init()) {
        puts("Initialization failed");
        return 1;
    }

    yyparse();

    debug_dump_tail();

    if (!has_encountered_error) {
        compile();
    }

    if (was_instruction_array_empty) {
        issue_warning("the input did not contain any instructions");
    }

    deinit();

    return has_encountered_error;
}
