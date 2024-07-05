%{
    #include <stdio.h>

    #include "eaxhla.yy.h"
    #include "assembler.h"

    void yyerror() {
        printf("\033[31mError: syntax error at line %d.\033[0m\n", yylineno);
    }

%}

%union{
    int intval;
}

%token PROGRAM_HEAD PROGRAM_TAIL
%token RAX RBX
%token XOR

%type<intval> register

%%
program: PROGRAM_HEAD code PROGRAM_TAIL { puts("this is in fact a program"); }
    ;

code: %empty
    | XOR register register { assemble_xor(size_64b, type_register_register, $2, $3); }
    ;

register: RAX   { $$ = 0x01; }
    |     RBX   { $$ = 0x02; }
    ;
%%
