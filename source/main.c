#include <stdio.h>

#include "eaxhla.yy.h"
#include "eaxhla.tab.h"

signed main(int argc, char * argv[]) {
    if (argc < 2) {
        printf("%s: <file>\n", argv[0]);
    }

    #if DEBUG == 1
        yydebug = 1;
    #endif
    yyin = fopen(argv[1], "r");
    yyparse();

    return 0;
}
