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
    cpuregister_t regval;
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
%type<intval>  memory dereference
%type<intval>  artimetric_block artimetric_expression artimetric_operand
%token<intval> LITERAL
%token<strval> ARRAY_LITERAL

// Specifiers
%token FAST
%token UNIX WIN64

// Logic
%token NEQ TNOT
/*
%token TOR TXOR // these are (in)conveniently instructions too
*/

// Type info
%token TIN
%token S8 S16 S32 S64
%token U8 U16 U32 U64
%type<intval> type
%type<varval> declaration

// Registers
%type<regval> register register64 register32

%token RBP RSP RIP
%token RAX RBX RCX RDX
%token RSI RDI
%token RG8 RG9 RG10 RG11 RG12 RG13 RG14 RG15
%token RGXMM0 RGXMM1 RGXMM2 RGXMM3 RGXMM4 RGXMM5 RGXMM6 RGXMM7

%token EBP ESP EIP
%token EAX EBX ECX EDX
%token ESI EDI
%token RG8D RG9D RG10D RG11D RG12D RG13D RG14D RG15D

// Instructions
%token TADD TOR TADC TBB TXOR TAND TSUB TCMP TSYSCALL TINC
%token INOP // better temp prefix?
%token IADD
%token ISYSCALL
%token IMOV
%token IXOR

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
    | system_specifier
    ;

system_specifier: UNIX { system_type = UNIX; }
    | WIN64 { system_type = WIN64; }
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
        $$.name = make_scoped_name(scope, $3);
        add_variable($$);
    }
    | variable_specifier type IDENTIFIER '=' LITERAL {
        $$.name = make_scoped_name(scope, $3);
        if (!can_fit($2, $5)) {
            issue_warning("the value \033[1m'%lld'\033[0m will overflow in assignement", $5);
        }
        $$.value = $5;
        add_variable($$);
    }
    ;

variable_specifier: %empty
    | TIN
    ;

type: S8    { $$ =  S8; }
    | S16   { $$ = S16; }
    | S32   { $$ = S32; }
    | S64   { $$ = S64; }
    | U8    { $$ =  U8; }
    | U16   { $$ = U16; }
    | U32   { $$ = U32; }
    | U64   { $$ = U64; }
    ;

immediate: LITERAL
    | IDENTIFIER { $$ = 0; /* XXX: how the fuck do i dereference? */}
    ;

memory: artimetric_block
    | dereference
    ;

dereference: '[' IDENTIFIER '+' value  ']' { $$ = 0; /* XXX: how the fuck do i dereference? */ }
    | '[' IDENTIFIER '-' value  ']' { $$ = 0; /* XXX: how the fuck do i dereference? */ }
    ;

value: artimetric_block
    | LITERAL
    | IDENTIFIER
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
    | instruction code
    ;

instruction: INOP { ; }
    /*
    | TXOR register register   code  { /* assemble_xor(size_64b, type_register_register, $2, $3); * / }
    | TXOR register immediate  code  { /* assemble_xor(size_64b, type_register_register, $2, $3); * / }
    | TXOR IDENTIFIER register code { /* assemble_xor(size_64b, type_register_register, $2, $3); * / free($2); }
    | TINC register            code
    | TINC IDENTIFIER          code { free($2); }
    */
    | IADD register register
    | IADD register immediate
    | IADD register memory
    | ISYSCALL { append_instruction_t1 (SYSCALL); }
    | IMOV register register
    | IMOV memory   register
    | IMOV register memory
    | IMOV register immediate { append_instruction_t6 (MOV, $2.size, REG, $2.number, IMM, (int) $3); }
    | IMOV memory   immediate
    | IXOR register register
    | IXOR register memory
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
    |  sublogic
    |  TNOT logic
    ;

logical_operand: sublogic
    | register
    | artimetric_block
    | LITERAL
    | IDENTIFIER
    ;

sublogic: '(' logic ')'
    ;

machine: MACHINE machine_code END_MACHINE
    ;

machine_code: %empty
    | LITERAL        machine_code
    | ARRAY_LITERAL  machine_code
    | IDENTIFIER     machine_code { free($1); }
    ;

call: calltype IDENTIFIER arguments { free($2); }
    ;

calltype: FASTCALL
    ;

arguments: %empty
    | IDENTIFIER       arguments { free($1); }
    | LITERAL          arguments
    | register         arguments
    | artimetric_block arguments
    ;

register: register64 { $$ = $1; $$.size = D64; }
    |     register32 { $$ = $1; $$.size = D32; }
    ;

register64: RAX    { $$.number = R0;    }
    |       RBX    { $$.number = R1;    }
    |       RCX    { $$.number = R2;    }
    |       RDX    { $$.number = R3;    }
    |       RSI    { $$.number = R4;    }
    |       RDI    { $$.number = R5;    }
    |       RBP    { $$.number = R6;    }
    |       RSP    { $$.number = R7;    }
    |       RG8    { $$.number = R8;    }
    |       RG9    { $$.number = R9;    }
    |       RG10   { $$.number = R10;   }
    |       RG11   { $$.number = R11;   }
    |       RG12   { $$.number = R12;   }
    |       RG13   { $$.number = R13;   }
    |       RG14   { $$.number = R14;   }
    |       RG15   { $$.number = R15;   }
    |       RGXMM0 { $$.number = 0; } /* XXX */
    |       RGXMM1 { $$.number = 0; }
    |       RGXMM2 { $$.number = 0; }
    |       RGXMM3 { $$.number = 0; }
    |       RGXMM4 { $$.number = 0; }
    |       RGXMM5 { $$.number = 0; }
    |       RGXMM6 { $$.number = 0; }
    |       RGXMM7 { $$.number = 0; }
    ;

register32: EAX    { $$.number = R0;  }
    |       EBX    { $$.number = R3;  }
    |       ECX    { $$.number = R1;  }
    |       EDX    { $$.number = R2;  }
    |       ESI    { $$.number = R6;  }
    |       EDI    { $$.number = R7;  }
    |       EBP    { $$.number = R5;  }
    |       ESP    { $$.number = R4;  }
    |       RG8D   { $$.number = R8;  }
    |       RG9D   { $$.number = R9;  }
    |       RG10D  { $$.number = R10; }
    |       RG11D  { $$.number = R11; }
    |       RG12D  { $$.number = R12; }
    |       RG13D  { $$.number = R13; }
    |       RG14D  { $$.number = R14; }
    |       RG15D  { $$.number = R15; }
    ;

artimetric_block: '{' artimetric_expression '}' { $$ = $2; }
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

artimetric_operand: LITERAL
    | IDENTIFIER { $$ = 0; /*XXX*/ }
    ;

exit: EXIT value
    ;
%%
