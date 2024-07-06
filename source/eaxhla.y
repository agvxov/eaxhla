%code requires {
    #include "eaxhla.h"
}
%{
    #include <stdio.h>
    #include <math.h>

    #include "eaxhla.yy.h"
    #include "assembler.h"
    #include "eaxhla.h"

    extern void yyfree_leftovers(void);
    extern char * yyfilename;

    void yyerror(const char * errmsg) {
        issue_error("%s near \033[1m'%s'\033[0m", errmsg, yytext);
    }

    long new_static(int size) { // XXX might not be required
        (void)size;
        return 0;
    }
%}

%union{
    long intval;
    char * strval;
    variable_t varval;
}

/* XXX NOTE: while naming, if my name collided with something in assembler.h i just
 *   added a 'T' prefix. this is obviously horrible and should be fixed,
 *   but that will require careful discussion with xolatile.
 *   for the time being its fine
 */

%token MYBEGIN

%token PROGRAM END_PROGRAM
%token PROCEDURE END_PROCEDURE
%token TLOOP END_LOOP
%token IF THEN ELSE END_IF
%token MACHINE END_MACHINE

%token<strval> IDENTIFIER LABEL

%type<intval>  immediate
%type<intval>  artimetric_block artimetric_expression artimetric_operand
%token<intval> LITERAL
%token<strval> STRING_LITERAL

// Specifiers
%token FAST
%token UNIX

// Logic
%token NEQ TNOT
/*
%token TOR TXOR // these are (in)conveniently instructions too
*/

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
%token EXIT BREAK
%%

hla: %empty            // { issue_warning("empty file, noting to be done."); }
    | program  hla
    | function hla
    ;

program: program_head declaration_section MYBEGIN code END_PROGRAM
    ;

program_head: program_specifier PROGRAM IDENTIFIER {
        if (is_program_found) {
            issue_error("only 1 entry point is allowed and a program block was already found");
            YYERROR;
        }
        is_program_found = 1;
        scope = $3;
    };

program_specifier: %empty
    | UNIX
    ;

function: function_head declaration_section MYBEGIN code END_PROCEDURE
    ;

function_head: function_specifier PROCEDURE IDENTIFIER { scope = $3; }
    ;

function_specifier: %empty
    | FAST
    ;

declaration_section: %empty
    | declaration declaration_section
    | error declaration_section { yyerrok; }
    ;

declaration:
      variable_specifier type IDENTIFIER {
        $2.name = make_scoped_name(scope, $3);
        add_variable($2);
    }
    | variable_specifier type IDENTIFIER '=' immediate {
        $2.name = make_scoped_name(scope, $3);
        $2.value = $5;
        add_variable($2);
    }
    | variable_specifier type IDENTIFIER '=' STRING_LITERAL {
        $2.name = make_scoped_name(scope, $3);
        $2.value = $5;
        add_variable($2);
    }
    ;

variable_specifier: %empty
    | TIN
    ;

type: S8    { $$ = (variable_t){ .size =  8, .address = new_static( 8), .value = 0 }; }
    | S16   { $$ = (variable_t){ .size = 16, .address = new_static(16), .value = 0 }; }
    | S32   { $$ = (variable_t){ .size = 32, .address = new_static(32), .value = 0 }; }
    | S64   { $$ = (variable_t){ .size = 64, .address = new_static(64), .value = 0 }; }
    | U8    { $$ = (variable_t){ .size =  8, .address = new_static( 8), .value = 0 }; }
    | U16   { $$ = (variable_t){ .size = 16, .address = new_static(16), .value = 0 }; }
    | U32   { $$ = (variable_t){ .size = 32, .address = new_static(32), .value = 0 }; }
    | U64   { $$ = (variable_t){ .size = 64, .address = new_static(64), .value = 0 }; }
    ;

code: %empty
    | error   code { yyerrok; }
    | loop    code
    | if      code
    | call    code
    | LABEL   code { free($1); }
    | machine code
    | BREAK   code
    | exit    code
    | TXOR register register   code  { /* assemble_xor(size_64b, type_register_register, $2, $3); */ }
    | TXOR register immediate  code  { /* assemble_xor(size_64b, type_register_register, $2, $3); */ }
    | TXOR IDENTIFIER register code { /* assemble_xor(size_64b, type_register_register, $2, $3); */ free($2); }
    | TINC register            code
    | TINC IDENTIFIER          code { free($2); }
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

logical_operand: sublogic
    | register
    | immediate
    | IDENTIFIER
    ;

sublogic: '(' logic ')'
    ;

memory: immediate
    | IDENTIFIER
    ;

machine: MACHINE machine_code END_MACHINE
    ;

machine_code: %empty
    | LITERAL        machine_code
    | STRING_LITERAL machine_code
    | IDENTIFIER     machine_code { free($1); }
    ;

call: calltype IDENTIFIER arguments { free($2); }
    ;

calltype: FASTCALL
    ;

arguments: %empty
    | IDENTIFIER     arguments { free($1); }
    | STRING_LITERAL arguments { free($1); }
    | register       arguments
    | immediate      arguments
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

artimetric_expression: %empty { $$ = 0; }
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
    | EXIT IDENTIFIER { free($2); }
    ;
%%
