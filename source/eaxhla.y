%code provides {
    enum {
        STATE_DECLARE,
    };
}

%code requires {
    #include "table.h"
    // XXX this could be easily squashed later
    typedef struct {
        int is_signed;
        int size;
        long address;
        char * name; // hash tables and shit
    } static_variable;

    // struct program
    // struct function
}

%{
    #include <stdio.h>
    #include <math.h>

    #include "eaxhla.yy.h"
    #include "assembler.h"

    extern void yyfree_leftovers(void);

    void yyerror() {
        printf("\033[31mError: syntax error at line %d near '%s'.\033[0m\n", yylineno, yytext);
        yyfree_leftovers();
    }

    long new_static(int size) {
        (void)size;
        return 0;
    }
%}

%union{
    long intval;
    char * strval;
    static_variable varval;
}

/* NOTE: while naming, if my name collided with something in assembler.h i just
 *        added a 'T' prefix. this is obviously horrible and should be fixed,
 *        but that will require careful discussion with xolatile.
 *        for the time being its fine
 */

%token MYBEGIN

%token PROGRAM END_PROGRAM
%token PROCEDURE END_PROCEDURE
%token TLOOP END_LOOP
%token IF THEN ELSE END_IF
%token MACHINE END_MACHINE

%token EXIT BREAK

%token<strval> IDENTIFIER

%type<intval>  immediate
%type<intval>  artimetric_block artimetric_expression artimetric_operand
%token<intval> LITERAL
%token<strval> STRING_LITERAL

%token FAST

%token UNIX

// Logic
%token NEQ TNOT
//%token TOR TXOR // these are (in)conveniently instructions too

// Type info
%token TIN
%token S8 S16 S32 S64
%token U8 U16 U32 U64
%type<varval> type

// Registers
%type<intval> register

%token RBP RSP RIP
%token RAX RBX RCX RDX
%token RSI RDI
%token RG8 RG9 RG10 RG11 RG12 RG13 RG14 RG15
%token RGXMM0 RGXMM1 RGXMM2 RGXMM3 RGXMM4 RGXMM5 RGXMM6 RGXMM7

// Instructions
%token TADD TOR TADC TBB TXOR TAND TSUB TCMP TSYSCALL TINC
// Instruction-likes
%token FASTCALL
%%

hla: %empty
    | program  hla
    | function hla
    ;

program: program_specifier PROGRAM IDENTIFIER declaration_section MYBEGIN code END_PROGRAM {
            free($3);
        }
    ;

program_specifier: %empty
    | UNIX
    ;

function: function_specifier PROCEDURE IDENTIFIER declaration_section MYBEGIN code END_PROCEDURE {
            free($3);
        }
    ;

function_specifier: %empty
    | FAST
    ;

declaration_section: %empty
    | declaration declaration_section
    ;

declaration: origin type IDENTIFIER { $2.name = $3; /* add_var($1); */ free($3); }
    | origin type IDENTIFIER '=' immediate { $2.name = $3; /* add_var($1); */ free($3); }
    | origin type IDENTIFIER '=' STRING_LITERAL { $2.name = $3; /* add_var($1); */ free($3); free($5); }
    ;

origin: %empty
    | TIN
    ;

type: S8    { $$ = (static_variable){ .is_signed = 1, .size =  8, .address = new_static( 8), .name = NULL, }; }
    | S16   { $$ = (static_variable){ .is_signed = 1, .size = 16, .address = new_static(16), .name = NULL, }; }
    | S32   { $$ = (static_variable){ .is_signed = 1, .size = 32, .address = new_static(32), .name = NULL, }; }
    | S64   { $$ = (static_variable){ .is_signed = 1, .size = 64, .address = new_static(64), .name = NULL, }; }
    | U8    { $$ = (static_variable){ .is_signed = 0, .size =  8, .address = new_static( 8), .name = NULL, }; }
    | U16   { $$ = (static_variable){ .is_signed = 0, .size = 16, .address = new_static(16), .name = NULL, }; }
    | U32   { $$ = (static_variable){ .is_signed = 0, .size = 32, .address = new_static(32), .name = NULL, }; }
    | U64   { $$ = (static_variable){ .is_signed = 0, .size = 64, .address = new_static(64), .name = NULL, }; }
    ;

code: %empty
    | loop    code
    | if      code
    | call    code
    | machine code
    | BREAK code
    | exit  code
    | TXOR register register  code  { /* assemble_xor(size_64b, type_register_register, $2, $3); */ }
    | TXOR register immediate code  { /* assemble_xor(size_64b, type_register_register, $2, $3); */ }
    | TXOR IDENTIFIER register code { /* assemble_xor(size_64b, type_register_register, $2, $3); */ free($2); }
    | TINC register code
    | TINC IDENTIFIER code { free($2); }
    ;

loop: TLOOP code END_LOOP
    ;

if: IF logic THEN code END_IF
    | IF logic THEN code ELSE code END_IF
    ;

logic: logical_operand TAND logical_operand
    |  logical_operand TOR  logical_operand
    |  logical_operand TXOR logical_operand
    |  logical_operand '='  logical_operand
    |  logical_operand NEQ  logical_operand
    |  TNOT logic
    ;

sublogic: '(' logic ')'
    ;

logical_operand: operand
    | sublogic
    ;

operand: immediate
    | register
    | IDENTIFIER
    ;

machine: MACHINE machine_code END_MACHINE
    ;

machine_code: %empty
    | LITERAL machine_code
    | STRING_LITERAL machine_code
    | IDENTIFIER machine_code { free($1); }
    ;

call: calltype IDENTIFIER arguments { free($2); }
    ;

calltype: FASTCALL
    ;

arguments: %empty
    | IDENTIFIER arguments { free($1); }
    | STRING_LITERAL arguments { free($1); }
    | register   arguments
    | immediate  arguments
    ;

register: RAX    { $$ = R0;    }
    |     RBX    { $$ = R1;    }
    |     RCX    { $$ = R2;    }
    |     RDX    { $$ = R3;    }
    |     RSI    { $$ = R4;    }
    |     RDI    { $$ = R5;    }
    |     RBP    { $$ = R6;    }
    |     RSP    { $$ = R7;    }
    |     RG8    { $$ = R8;    }
    |     RG9    { $$ = R9;    }
    |     RG10   { $$ = R10;   }
    |     RG11   { $$ = R11;   }
    |     RG12   { $$ = R12;   }
    |     RG13   { $$ = R13;   }
    |     RG14   { $$ = R14;   }
    |     RG15   { $$ = R15;   }
    |     RGXMM0 { $$ = 0; } /* XXX */
    |     RGXMM1 { $$ = 0; }
    |     RGXMM2 { $$ = 0; }
    |     RGXMM3 { $$ = 0; }
    |     RGXMM4 { $$ = 0; }
    |     RGXMM5 { $$ = 0; }
    |     RGXMM6 { $$ = 0; }
    |     RGXMM7 { $$ = 0; }
    ;

immediate: LITERAL
    | artimetric_block
    ;

artimetric_block: '[' artimetric_expression ']' { $$ = $2; }
    ;

artimetric_expression: %empty { yyerror(); }
    | artimetric_operand
    | artimetric_expression '+' artimetric_operand { $$ = $1 + $3; }
    | artimetric_expression '-' artimetric_operand { $$ = $1 - $3; }
    | artimetric_expression '*' artimetric_operand { $$ = $1 * $3; }
    | artimetric_expression '/' artimetric_operand { $$ = $1 / $3; }
    | artimetric_expression '%' artimetric_operand { $$ = $1 % $3; }
    | artimetric_expression '^' artimetric_operand { $$ = pow($1, $3); }
    ;

artimetric_operand: immediate
    | IDENTIFIER { $$ = 0; /*XXX*/ }
    ;

exit: EXIT immediate
    | EXIT IDENTIFIER
    ;
%%
