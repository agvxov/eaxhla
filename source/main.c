#include <stdio.h>

#include "eaxhla.yy.h"
#include "eaxhla.tab.h"

signed main(int argc, char * argv[]) {
    if (argc < 2) {
        printf("%s <file>", argv[0]);
    }

    yyin = fopen(argv[1], "r");
    yyparse();

    return 0;
}
