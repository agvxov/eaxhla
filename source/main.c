#include <stdio.h>

#include "eaxhla.yy.h"
#include "eaxhla.tab.h"

extern void yyfree_leftovers(void);

signed main(int argc, char * argv[]) {
    if (argc < 2) {
        printf("%s: <file>\n", argv[0]);
    }

    #if DEBUG == 1
        yydebug = 1;
    #endif

    yyin = fopen(argv[1], "r");
    yyparse();

    yyfree_leftovers();

    return 0;
}
