%code provides {
    enum {
        STATE_DECLARE,
    };
}

%code requires {
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

    #include "eaxhla.yy.h"
    #include "assembler.h"

    extern void yyfree_leftovers(void);

    void yyerror() {
        printf("\033[31mError: syntax error at line %d near '%s'.\033[0m\n", yylineno, yytext);
        yyfree_leftovers();
    }

    extern void set_state(int state);

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
%token IF THEN END_IF
%token BREAK

%token<strval> IDENTIFIER

%token<intval> LITERAL
%token<strval> STRING_LITERAL

%token FAST

%token UNIX

// Type info
%token TIN
%token S8 S16 S32 S64
%token U8 U16 U32 U64
%type<varval> type

// Registers
%type<intval> register

%token RAX RBX RCX RDX
%token RBP RSP RIP

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
    | origin type IDENTIFIER '=' LITERAL { $2.name = $3; /* add_var($1); */ free($3); }
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
    | loop  code
    | if    code
    | call  code
    | BREAK code
    | TXOR register register  code  { /* assemble_xor(size_64b, type_register_register, $2, $3); */ }
    | TXOR register immediate code  { /* assemble_xor(size_64b, type_register_register, $2, $3); */ }
    | TXOR IDENTIFIER register code { /* assemble_xor(size_64b, type_register_register, $2, $3); */ free($2); }
    | TINC register code
    | TINC IDENTIFIER code { free($2); }
    ;

loop: TLOOP code END_LOOP
    ;

if: IF logic THEN code END_IF
    ;

logic: %empty /* XXX */
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

register: RAX   { $$ = R0; }
    |     RBX   { $$ = R1; }
    ;

immediate: LITERAL
    ;
%%
