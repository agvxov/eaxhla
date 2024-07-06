#include <stdio.h>

#include "eaxhla.h"
#include "eaxhla.yy.h"
#include "eaxhla.tab.h"

extern void yyfree_leftovers(void);

char * yyfilename;

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

    if (eaxhla_init()) {
        puts("Initialization failed");
        return 1;
    }

    yyparse();

    yyfree_leftovers();

    eaxhla_destroy();

    return 0;
}
