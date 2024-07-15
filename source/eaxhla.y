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
%}

%union{
    long intval;
    char * strval;
    struct {
        unsigned long long len;
        void * data;
    } blobval;
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
%token REPEAT END_REPEAT
%token IF THEN ELSE END_IF
%token MACHINE END_MACHINE
%token LIBRARY END_LIBRARY

%token UNTIL

%token<strval> IDENTIFIER LABEL

%type<intval>   immediate
%type<intval>   memory dereference
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
%type<varval> anon_variable

// Registers
%type<regval> register register64s register32s register16s register8s

// #placeholder<register_token_list> BEGIN
%token RAX RBX RCX RDX RSI RDI RBP RSP RG8 RG9 RG10 RG11 RG12 RG13 RG14 RG15
%token EAX EBX ECX EDX ESI EDI EBP ESP RG8D RG9D RG10D RG11D RG12D RG13D RG14D RG15D
%token AX BX CX DX SI DI BP SP R8W R9W R10W R11W R12W R13W R14W R15W
%token AL BL CL DL SIL DIL BPL SPL R8B R9B R10B R11B R12B R13B R14B R15B

// #placeholder<register_token_list> END

// Instructions
%token INOP
// #placeholder<instruction_token_list> BEGIN
%token ITADC ITADD ITAND ITCMP ITDEC ITDIV ITHLT ITIDIV ITIMUL ITINC ITLEAVE ITLOCK ITMOV ITMUL ITNEG ITNOT ITOR ITPAUSE ITRETF ITRETN ITSAR ITSBB ITSUB ITSYSCALL ITSYSENTER ITSYSEXIT ITSYSRET ITXOR
// #placeholder<instruction_token_list> END

// Instruction-likes
%token FASTCALL
%token EXIT BREAK
%%

hla: %empty            // { issue_warning("empty file, noting to be done."); }
    // | library  hla
    | declaration hla // tmp
    | program  hla
    | function hla
    ;

program: program_head declaration_section MYBEGIN code END_PROGRAM { scope = NULL; }
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

    // XXX: end procedure thing
function: function_head declaration_section MYBEGIN code END_PROCEDURE { scope = NULL; }
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
        $$.elements = 1;
        add_variable($$);
    }
    | variable_specifier type IDENTIFIER '=' LITERAL {
        $$.name = make_scoped_name(scope, $3);
        if (!can_fit($2, $5)) {
            issue_warning("the value \033[1m'%lld'\033[0m will overflow in assignement", $5);
        }
        $$.elements = 1;
        $$.value = $5;
        add_variable($$);
    }
    | variable_specifier type '<' value '>' IDENTIFIER {
        if (validate_array_size($4)) {
            break;
        }
        $$.name = make_scoped_name(scope, $6);
        $$.elements = $4;
        add_variable($$);
    }
    | variable_specifier type '<' value '>' IDENTIFIER '=' ARRAY_LITERAL {
        if (validate_array_size($4)) {
            break;
        }
        if ($4 < $8.len) {
            issue_warning("you are a nigger");
        }
        $$.name = make_scoped_name(scope, $6);
        $$.elements = $4;
        $$.array_value = $8.data;
        add_variable($$);
    }
    | variable_specifier type '<' '>' IDENTIFIER '=' ARRAY_LITERAL {
        $$.name = make_scoped_name(scope, $5);
        $$.elements = $7.len;
        $$.array_value = $7.data;
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
    | IDENTIFIER
    ;

dereference: '[' IDENTIFIER '+' value  ']' { $$ = 0; /* XXX: how the fuck do i dereference? */ }
    | '[' IDENTIFIER '-' value  ']' { $$ = 0; /* XXX: how the fuck do i dereference? */ }
    ;

value: artimetric_block
    | LITERAL
    | IDENTIFIER        {
        char * varname = make_scoped_name(scope, $1);
        variable_t * var = get_variable(varname);
        $$ = var->value;
        free(var);
    }
    ;

anon_variable: ARRAY_LITERAL {
        $$.array_value = $1.data; 
        $$.elements    = $1.len;
        int ignore = asprintf(&$$.name, "_anon_%llu", anon_variable_counter++);
        (void)ignore;
        add_variable($$);
    }
    ;

code: %empty
    | error   code { /*yyerrok;*/ }
    | repeat  code
    | if      code
    | call    code
    | LABEL   code { /* XXX */ free($1); }
    | machine code
    | BREAK   code
    | exit    code
    | instruction code
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
    | anon_variable    arguments
    ;

register: register64s { $$ = $1; $$.size = D64; }
    |     register32s { $$ = $1; $$.size = D32; }
    |     register16s { $$ = $1; $$.size = D16; }
    |     register8s  { $$ = $1; $$.size = D8;  }
    ;

    // #placeholder<register_parser_rules> BEGIN
register64s: RAX { $$.number = R0; }
    | RBX   { $$.number = R1; }
    | RCX   { $$.number = R2; }
    | RDX   { $$.number = R3; }
    | RSI   { $$.number = R4; }
    | RDI   { $$.number = R5; }
    | RBP   { $$.number = R6; }
    | RSP   { $$.number = R7; }
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
    | EBX   { $$.number = R1; }
    | ECX   { $$.number = R2; }
    | EDX   { $$.number = R3; }
    | ESI   { $$.number = R4; }
    | EDI   { $$.number = R5; }
    | EBP   { $$.number = R6; }
    | ESP   { $$.number = R7; }
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
    | BX    { $$.number = R1; }
    | CX    { $$.number = R2; }
    | DX    { $$.number = R3; }
    | SI    { $$.number = R4; }
    | DI    { $$.number = R5; }
    | BP    { $$.number = R6; }
    | SP    { $$.number = R7; }
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
    | BL    { $$.number = R1; }
    | CL    { $$.number = R2; }
    | DL    { $$.number = R3; }
    | SIL   { $$.number = R4; }
    | DIL   { $$.number = R5; }
    | BPL   { $$.number = R6; }
    | SPL   { $$.number = R7; }
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
    | IDENTIFIER { $$ = 0; /*XXX*/ }
    ;

exit: EXIT value
    ;

    /* XXX
library: LIBRARY declaration_section MYBEGIN library_code END_LIBRARY
    ;

library_code: %empty
    | function library_code
    ;
    */

instruction: INOP { append_instruction_t1(NOP); }
    // #placeholder<instruction_parser_rules> BEGIN
    | ITSYSCALL { append_instruction_t1(SYSCALL); }
    | ITSYSRET { append_instruction_t1(SYSRET); }
    | ITSYSEXIT { append_instruction_t1(SYSEXIT); }
    | ITSYSENTER { append_instruction_t1(SYSENTER); }
    | ITLEAVE { append_instruction_t1(LEAVE); }
    | ITRETF { append_instruction_t1(RETF); }
    | ITRETN { append_instruction_t1(RETN); }
    | ITPAUSE { append_instruction_t1(PAUSE); }
    | ITHLT { append_instruction_t1(HLT); }
    | ITLOCK { append_instruction_t1(LOCK); }
    | ITINC register { append_instruction_t4( INC, $2.size, REG, $2.number ); }
    | ITDEC register { append_instruction_t4( DEC, $2.size, REG, $2.number ); }
    | ITNOT register { append_instruction_t4( NOT, $2.size, REG, $2.number ); }
    | ITNEG register { append_instruction_t4( NEG, $2.size, REG, $2.number ); }
    | ITMUL register { append_instruction_t4( MUL, $2.size, REG, $2.number ); }
    | ITIMUL register { append_instruction_t4( IMUL, $2.size, REG, $2.number ); }
    | ITDIV register { append_instruction_t4( DIV, $2.size, REG, $2.number ); }
    | ITIDIV register { append_instruction_t4( IDIV, $2.size, REG, $2.number ); }
    | ITINC memory { append_instruction_t4( INC, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITDEC memory { append_instruction_t4( DEC, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITNOT memory { append_instruction_t4( NOT, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITNEG memory { append_instruction_t4( NEG, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITMUL memory { append_instruction_t4( MUL, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITIMUL memory { append_instruction_t4( IMUL, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITDIV memory { append_instruction_t4( DIV, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITIDIV memory { append_instruction_t4( IDIV, 0 /* ??? */, MEM, 0 /* ??? */ ); }
    | ITADD register register { append_instruction_t6( ADD, $2.size, REG, $2.number, REG, $3.number ); }
    | ITOR register register { append_instruction_t6( OR, $2.size, REG, $2.number, REG, $3.number ); }
    | ITADC register register { append_instruction_t6( ADC, $2.size, REG, $2.number, REG, $3.number ); }
    | ITSBB register register { append_instruction_t6( SBB, $2.size, REG, $2.number, REG, $3.number ); }
    | ITAND register register { append_instruction_t6( AND, $2.size, REG, $2.number, REG, $3.number ); }
    | ITSUB register register { append_instruction_t6( SUB, $2.size, REG, $2.number, REG, $3.number ); }
    | ITXOR register register { append_instruction_t6( XOR, $2.size, REG, $2.number, REG, $3.number ); }
    | ITCMP register register { append_instruction_t6( CMP, $2.size, REG, $2.number, REG, $3.number ); }
    | ITSAR register immediate { append_instruction_t6( SAR, $2.size, REG, $2.number, IMM, (int)$3 ); }
    | ITMOV register register { append_instruction_t6( MOV, $2.size, REG, $2.number, REG, $3.number ); }
    | ITMOV register immediate { append_instruction_t6( MOV, $2.size, REG, $2.number, IMM, (int)$3 ); }

    // #placeholder<instruction_parser_rules> END
    ;

%%
