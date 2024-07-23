/* EAXHLA parser
 * This source file has minimal knowledge about highlevel constructs.
 * The only time it interacts with the assembler interface directly is when
 *  instructions are encountered or processing a machine code block,
 *  everything else is delegated to eaxhla.c
 */
%code requires {
    #include "eaxhla.h"
}
%{
    #include <stdio.h>
    #include <math.h>
    #include <stdbool.h>

    #include "eaxhla.h"
    #include "eaxhla.yy.h"
    #include "assembler.h"
    #include "compile.h"
    #include "debug.h"

    extern void yyfree_leftovers(void);
    extern char * yyfilename;

    void yyerror(const char * errmsg) {
        issue_error("%s near \033[1m'%s'\033[0m", errmsg, yytext);
    }
%}

%union{
    long intval;
    char * strval;
    struct {
        int type;
        int value;
    } argval;
    struct {
        unsigned long long len;
        void * data;
    } blobval;
    symbol_t varval;
    cpuregister_t regval;
}

%token MYBEGIN

%token PROGRAM END_PROGRAM
%token PROCEDURE END_PROCEDURE
%token REPEAT END_REPEAT
%token IF THEN ELSE END_IF
%token MACHINE END_MACHINE
%token LIBRARY END_LIBRARY

%token UNTIL

%token<strval> IDENTIFIER LABEL

%type<argval>   immediate
%type<intval>   memory dereference relative
%type<intval>   artimetric_block artimetric_expression artimetric_operand
%type<intval>   value
%token<intval>  LITERAL
%token<blobval> ARRAY_LITERAL

// Specifiers
%token FAST
%token UNIX WIN64

// Logic
%token ITNEQ
/*
%token ITOR ITXOR ITAND ITNOT // these are (in)conveniently instructions too
*/

// Type info
%token TIN
%token S8 S16 S32 S64
%token U8 U16 U32 U64
%type<intval> type
%type<varval> declaration
%type<varval> stored_literal

// Registers
%type<regval> register register64s register32s register16s register8s

// #placeholder<register_token_list> BEGIN
%token RAX RCX RDX RBX RSP RBP RSI RDI RG8 RG9 RG10 RG11 RG12 RG13 RG14 RG15
%token EAX ECX EDX EBX ESP EBP ESI EDI RG8D RG9D RG10D RG11D RG12D RG13D RG14D RG15D
%token AX CX DX BX SP BP SI DI R8W R9W R10W R11W R12W R13W R14W R15W
%token AL CL DL BL SPL BPL SIL DIL R8B R9B R10B R11B R12B R13B R14B R15B

// #placeholder<register_token_list> END

// Instructions
%token INOP
// #placeholder<instruction_token_list> BEGIN
%token ITADC ITADD ITAND ITCMP ITDEC ITDIV ITHLT ITIDIV ITIMUL ITINC ITJMP ITLEAVE ITLOCK ITMOV ITMUL ITNEG ITNOT ITOR ITPAUSE ITRETF ITRETN ITSAR ITSBB ITSUB ITSYSCALL ITSYSENTER ITSYSEXIT ITSYSRET ITXOR
// #placeholder<instruction_token_list> END

// Instruction-likes
%token FASTCALL
%token EXIT BREAK
%%

document: hla { fin_hla(); }
    ;

hla: %empty
    // | library  hla
    | declaration hla // tmp
    | program  hla
    | function hla
    ;

program: program_head declaration_section MYBEGIN code END_PROGRAM {
    }
    ;

program_head: program_specifier PROGRAM IDENTIFIER {
        add_scope($3);
        add_program($3);
        free($3);
    };

program_specifier: %empty
    | system_specifier
    ;

system_specifier: UNIX { system_type = UNIX; }
    | WIN64 { system_type = WIN64; }
    ;

    // XXX: end procedure thing
function: function_head declaration_section MYBEGIN code END_PROCEDURE {
        fin_procedure();
    }
    ;

function_head: function_specifier PROCEDURE IDENTIFIER {
        add_scope($3);
        add_procedure($3);
        free($3);
    }
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
        add_variable($2, $3);
        free($3);
    }
    | variable_specifier type IDENTIFIER '=' LITERAL {
        add_variable_with_value($2, $3, $5);
        free($3);
    }
    | variable_specifier type '<' value '>' IDENTIFIER {
        add_array_variable($2, $6, $4);
        free($6);
    }
    | variable_specifier type '<' value '>' IDENTIFIER '=' ARRAY_LITERAL {
        add_array_variable_with_value($2, $6, $4, $8.data, $8.len);
        free($6);
    }
    | variable_specifier type '<' '>' IDENTIFIER '=' ARRAY_LITERAL {
        add_array_variable_with_value($2, $5, $7.len, $7.data, $7.len);
        free($5);
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

immediate: LITERAL {
        $$.type  = IMM;
        $$.value = $1;
    }
    | IDENTIFIER {
        symbol_t * variable = get_variable($1);
        $$.type  = REL;
        $$.value = variable->_id;
        free($1);
    }
    ;

memory: artimetric_block
    | dereference
    ;

dereference: '[' IDENTIFIER ']' { $$ = 0; /* XXX: how the fuck do i dereference? */ }
    | '[' IDENTIFIER '+' value ']' { $$ = 0; /* XXX: how the fuck do i dereference? */ }
    | '[' IDENTIFIER '-' value ']' { $$ = 0; /* XXX: how the fuck do i dereference? */ }
    ;

relative: IDENTIFIER {
        $$ = get_relative($1)->_id;
        free($1);
    }
    ;

value: artimetric_block
    | LITERAL
    | IDENTIFIER        {
        symbol_t * var = get_variable($1);
        $$ = var->value;
        free($1);
    }
    ;

stored_literal: ARRAY_LITERAL {
        add_literal($1.data, $1.len);
    }
    ;

code: %empty
    | error   code { /*yyerrok;*/ }
    | repeat  code
    | if      code
    | call    code
    | label   code
    | machine code
    | BREAK   code
    | exit    code
    | instruction code
    ;

label: LABEL {
        add_label($1, true);
        free($1);
    }
    ;

repeat: REPEAT code END_REPEAT
    | UNTIL logic REPEAT code END_REPEAT
    ;

if: IF logic THEN code END_IF
    | IF logic THEN code ELSE code END_IF
    ;

logic: logical_operand ITAND logical_operand
    |  logical_operand ITOR  logical_operand
    |  logical_operand ITXOR logical_operand
    |  logical_operand '='   logical_operand
    |  logical_operand ITNEQ logical_operand
    |  sublogic
    |  ITNOT logic
    ;

logical_operand: sublogic
    | register
    | artimetric_block
    | dereference
    | LITERAL
    | IDENTIFIER
    ;

sublogic: '(' logic ')'
    ;

machine: MACHINE machine_code END_MACHINE
    ;

machine_code: %empty
    | machine_code LITERAL {
        append_instructions(ASMDIRIMM, D8, 1, $2);
    }
    | machine_code ARRAY_LITERAL {
        append_instructions(ASMDIRIMM, D8, $2.len);
        for (unsigned long long i = 0; i < $2.len; i++) {
            append_instructions(((char*)$2.data)[i]);
        }
    }
    | machine_code IDENTIFIER { free($2); }
    ;

call: FASTCALL IDENTIFIER arguments {
        add_fastcall($2);
        free($2);
    }
    ;

arguments: %empty
    | IDENTIFIER       arguments { free($1); }
    | LITERAL          arguments
    | register         arguments
    | artimetric_block arguments
    | stored_literal   arguments
    ;

register: register64s { $$ = $1; $$.size = D64; }
    |     register32s { $$ = $1; $$.size = D32; }
    |     register16s { $$ = $1; $$.size = D16; }
    |     register8s  { $$ = $1; $$.size = D8;  }
    ;

    // #placeholder<register_parser_rules> BEGIN
register64s: RAX { $$.number = R0; }
    | RCX   { $$.number = R1; }
    | RDX   { $$.number = R2; }
    | RBX   { $$.number = R3; }
    | RSP   { $$.number = R4; }
    | RBP   { $$.number = R5; }
    | RSI   { $$.number = R6; }
    | RDI   { $$.number = R7; }
    | RG8   { $$.number = R8; }
    | RG9   { $$.number = R9; }
    | RG10  { $$.number = R10; }
    | RG11  { $$.number = R11; }
    | RG12  { $$.number = R12; }
    | RG13  { $$.number = R13; }
    | RG14  { $$.number = R14; }
    | RG15  { $$.number = R15; }
    ;

register32s: EAX { $$.number = R0; }
    | ECX   { $$.number = R1; }
    | EDX   { $$.number = R2; }
    | EBX   { $$.number = R3; }
    | ESP   { $$.number = R4; }
    | EBP   { $$.number = R5; }
    | ESI   { $$.number = R6; }
    | EDI   { $$.number = R7; }
    | RG8D  { $$.number = R8; }
    | RG9D  { $$.number = R9; }
    | RG10D { $$.number = R10; }
    | RG11D { $$.number = R11; }
    | RG12D { $$.number = R12; }
    | RG13D { $$.number = R13; }
    | RG14D { $$.number = R14; }
    | RG15D { $$.number = R15; }
    ;

register16s: AX { $$.number = R0; }
    | CX    { $$.number = R1; }
    | DX    { $$.number = R2; }
    | BX    { $$.number = R3; }
    | SP    { $$.number = R4; }
    | BP    { $$.number = R5; }
    | SI    { $$.number = R6; }
    | DI    { $$.number = R7; }
    | R8W   { $$.number = R8; }
    | R9W   { $$.number = R9; }
    | R10W  { $$.number = R10; }
    | R11W  { $$.number = R11; }
    | R12W  { $$.number = R12; }
    | R13W  { $$.number = R13; }
    | R14W  { $$.number = R14; }
    | R15W  { $$.number = R15; }
    ;

register8s: AL { $$.number = R0; }
    | CL    { $$.number = R1; }
    | DL    { $$.number = R2; }
    | BL    { $$.number = R3; }
    | SPL   { $$.number = R4; }
    | BPL   { $$.number = R5; }
    | SIL   { $$.number = R6; }
    | DIL   { $$.number = R7; }
    | R8B   { $$.number = R8; }
    | R9B   { $$.number = R9; }
    | R10B  { $$.number = R10; }
    | R11B  { $$.number = R11; }
    | R12B  { $$.number = R12; }
    | R13B  { $$.number = R13; }
    | R14B  { $$.number = R14; }
    | R15B  { $$.number = R15; }
    ;


    // #placeholder<register_parser_rules> END

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
    | IDENTIFIER {
        $$ = get_variable($1)->value;
    }
    ;

exit: EXIT value { append_exit($2); }
    ;

    /* XXX
library: LIBRARY declaration_section MYBEGIN library_code END_LIBRARY
    ;

library_code: %empty
    | function library_code
    ;
    */

instruction: INOP { append_instructions(NOP); }
    // #placeholder<instruction_parser_rules> BEGIN
    | ITSYSCALL { append_instructions(SYSCALL); }
    | ITSYSRET { append_instructions(SYSRET); }
    | ITSYSEXIT { append_instructions(SYSEXIT); }
    | ITSYSENTER { append_instructions(SYSENTER); }
    | ITLEAVE { append_instructions(LEAVE); }
    | ITRETF { append_instructions(RETF); }
    | ITRETN { append_instructions(RETN); }
    | ITPAUSE { append_instructions(PAUSE); }
    | ITHLT { append_instructions(HLT); }
    | ITLOCK { append_instructions(LOCK); }
    | ITJMP relative { append_instructions( JMP, D32, REL, $2 ); }
    | ITINC register { append_instructions( INC, $2.size, REG, $2.number ); }
    | ITDEC register { append_instructions( DEC, $2.size, REG, $2.number ); }
    | ITNOT register { append_instructions( NOT, $2.size, REG, $2.number ); }
    | ITNEG register { append_instructions( NEG, $2.size, REG, $2.number ); }
    | ITMUL register { append_instructions( MUL, $2.size, REG, $2.number ); }
    | ITIMUL register { append_instructions( IMUL, $2.size, REG, $2.number ); }
    | ITDIV register { append_instructions( DIV, $2.size, REG, $2.number ); }
    | ITIDIV register { append_instructions( IDIV, $2.size, REG, $2.number ); }
    | ITINC memory { append_instructions( INC, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITDEC memory { append_instructions( DEC, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITNOT memory { append_instructions( NOT, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITNEG memory { append_instructions( NEG, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITMUL memory { append_instructions( MUL, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITIMUL memory { append_instructions( IMUL, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITDIV memory { append_instructions( DIV, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITIDIV memory { append_instructions( IDIV, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITADD register register { append_instructions( ADD, $2.size, REG, $2.number, REG, $3.number ); }
    | ITOR register register { append_instructions( OR, $2.size, REG, $2.number, REG, $3.number ); }
    | ITADC register register { append_instructions( ADC, $2.size, REG, $2.number, REG, $3.number ); }
    | ITSBB register register { append_instructions( SBB, $2.size, REG, $2.number, REG, $3.number ); }
    | ITAND register register { append_instructions( AND, $2.size, REG, $2.number, REG, $3.number ); }
    | ITSUB register register { append_instructions( SUB, $2.size, REG, $2.number, REG, $3.number ); }
    | ITXOR register register { append_instructions( XOR, $2.size, REG, $2.number, REG, $3.number ); }
    | ITCMP register register { append_instructions( CMP, $2.size, REG, $2.number, REG, $3.number ); }
    | ITSAR register immediate { append_instructions( SAR, $2.size, REG, $2.number, $3.type, $3.value ); }
    | ITMOV register register { append_instructions( MOV, $2.size, REG, $2.number, REG, $3.number ); }
    | ITMOV register immediate { append_instructions( MOV, $2.size, REG, $2.number, $3.type, $3.value ); }

    // #placeholder<instruction_parser_rules> END
    ;

%%
