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
%token RAX RCX RDX RBX RSP RBP RSI RDI R8 R9 R10 R11 R12 R13 R14 R15
%token EAX ECX EDX EBX ESP EBP ESI EDI R8D R9D R10D R11D R12D R13D R14D R15D
%token AX CX DX BX SP BP SI DI R8W R9W R10W R11W R12W R13W R14W R15W
%token AL CL DL BL SPL BPL SIL DIL R8B R9B R10B R11B R12B R13B R14B R15B

// #placeholder<register_token_list> END

// Instructions
%token INOP
// #placeholder<instruction_token_list> BEGIN
%token ITADC ITADD ITAND ITCBW ITCDQ ITCDQE ITCLC ITCLD ITCLI ITCLTS ITCMC ITCMP ITCPUID ITCQO ITCWD ITCWDE ITDEC ITDIV ITEMMS ITF2XM1 ITFABS ITFCHS ITFCOS ITFDECSTP ITFINCSTP ITFLD1 ITFLDL2E ITFLDL2T ITFLDLG2 ITFLDLN2 ITFLDPI ITFLDZ ITFNOP ITFPATAN ITFPREM ITFPREM1 ITFPTAN ITFRNDINT ITFSCALE ITFSIN ITFSINCOS ITFSQRT ITFTST ITFXAM ITFXTRACT ITFYL2X ITFYL2XP1 ITHLT ITIDIV ITIMUL ITINAL ITINAX ITINC ITINEAX ITINSB ITINSD ITINSW ITINT3 ITINVD ITIRETD ITIRETQ ITJE ITJMP ITJNE ITLEAVE ITLOCK ITLODSB ITLODSD ITLODSQ ITLODSW ITMOV ITMUL ITNEG ITNOP ITNOT ITOR ITOUTAL ITOUTAX ITOUTEAX ITOUTSB ITOUTSD ITOUTSW ITPAUSE ITPOP ITPOPF ITPUSHF ITRDMSR ITRDPMC ITRDTSC ITRETF ITRETN ITRSM ITSAR ITSBB ITSCASB ITSCASD ITSCASQ ITSCASW ITSTC ITSTD ITSTI ITSTOSB ITSTOSD ITSTOSQ ITSTOSW ITSUB ITSYSCALL ITSYSENTER ITSYSEXIT ITSYSRET ITUD2 ITWAIT ITWBINVD ITWRMSR ITXLATB ITXOR
// #placeholder<instruction_token_list> END

// Instruction-likes
%token FASTCALL
%token EXIT BREAK
%%

document: hla { fin_hla(); }
    ;

hla: %empty
    // | library  hla
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

dereference: '[' IDENTIFIER ']' {
        $$ = get_variable($2)->_id;
        free($2);
    }
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
    /*| repeat  code */
    /*| if      code */
    | call    code
    | label   code
    | machine code
    /*| BREAK   code*/
    | exit    code
    | instruction code
    ;

label: LABEL {
        add_label($1, true);
        free($1);
    }
    ;

    /*
repeat: REPEAT code END_REPEAT
    | UNTIL logic REPEAT code END_REPEAT
    ;

if_start: IF { add_if(); }
    ;

if_end: END_IF { fin_if(); }
    ;

if: if_start logic THEN code if_end
    | IF logic THEN code ELSE code END_IF
    ;

logic: register '=' register {
        add_logic_equals(&$1, &$3);
    }
    ;
    logical_operand ITAND logical_operand
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
    */

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
register64s: RAX { $$.number = GR0; }
    | RCX   { $$.number = GR1; }
    | RDX   { $$.number = GR2; }
    | RBX   { $$.number = GR3; }
    | RSP   { $$.number = GR4; }
    | RBP   { $$.number = GR5; }
    | RSI   { $$.number = GR6; }
    | RDI   { $$.number = GR7; }
    | R8    { $$.number = GR8; }
    | R9    { $$.number = GR9; }
    | R10   { $$.number = GR10; }
    | R11   { $$.number = GR11; }
    | R12   { $$.number = GR12; }
    | R13   { $$.number = GR13; }
    | R14   { $$.number = GR14; }
    | R15   { $$.number = GR15; }
    ;

register32s: EAX { $$.number = GR0; }
    | ECX   { $$.number = GR1; }
    | EDX   { $$.number = GR2; }
    | EBX   { $$.number = GR3; }
    | ESP   { $$.number = GR4; }
    | EBP   { $$.number = GR5; }
    | ESI   { $$.number = GR6; }
    | EDI   { $$.number = GR7; }
    | R8D   { $$.number = GR8; }
    | R9D   { $$.number = GR9; }
    | R10D  { $$.number = GR10; }
    | R11D  { $$.number = GR11; }
    | R12D  { $$.number = GR12; }
    | R13D  { $$.number = GR13; }
    | R14D  { $$.number = GR14; }
    | R15D  { $$.number = GR15; }
    ;

register16s: AX { $$.number = GR0; }
    | CX    { $$.number = GR1; }
    | DX    { $$.number = GR2; }
    | BX    { $$.number = GR3; }
    | SP    { $$.number = GR4; }
    | BP    { $$.number = GR5; }
    | SI    { $$.number = GR6; }
    | DI    { $$.number = GR7; }
    | R8W   { $$.number = GR8; }
    | R9W   { $$.number = GR9; }
    | R10W  { $$.number = GR10; }
    | R11W  { $$.number = GR11; }
    | R12W  { $$.number = GR12; }
    | R13W  { $$.number = GR13; }
    | R14W  { $$.number = GR14; }
    | R15W  { $$.number = GR15; }
    ;

register8s: AL { $$.number = GR0; }
    | CL    { $$.number = GR1; }
    | DL    { $$.number = GR2; }
    | BL    { $$.number = GR3; }
    | SPL   { $$.number = GR4; }
    | BPL   { $$.number = GR5; }
    | SIL   { $$.number = GR6; }
    | DIL   { $$.number = GR7; }
    | R8B   { $$.number = GR8; }
    | R9B   { $$.number = GR9; }
    | R10B  { $$.number = GR10; }
    | R11B  { $$.number = GR11; }
    | R12B  { $$.number = GR12; }
    | R13B  { $$.number = GR13; }
    | R14B  { $$.number = GR14; }
    | R15B  { $$.number = GR15; }
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
    | ITNOP { append_instructions(NOP); }
    | ITRETN { append_instructions(RETN); }
    | ITRETF { append_instructions(RETF); }
    | ITLEAVE { append_instructions(LEAVE); }
    | ITLOCK { append_instructions(LOCK); }
    | ITHLT { append_instructions(HLT); }
    | ITPOPF { append_instructions(POPF); }
    | ITPUSHF { append_instructions(PUSHF); }
    | ITWAIT { append_instructions(WAIT); }
    | ITCLC { append_instructions(CLC); }
    | ITCLD { append_instructions(CLD); }
    | ITCLI { append_instructions(CLI); }
    | ITSTC { append_instructions(STC); }
    | ITSTD { append_instructions(STD); }
    | ITSTI { append_instructions(STI); }
    | ITCMC { append_instructions(CMC); }
    | ITINSB { append_instructions(INSB); }
    | ITINSD { append_instructions(INSD); }
    | ITOUTSB { append_instructions(OUTSB); }
    | ITOUTSD { append_instructions(OUTSD); }
    | ITCDQ { append_instructions(CDQ); }
    | ITCWDE { append_instructions(CWDE); }
    | ITINAL { append_instructions(INAL); }
    | ITINEAX { append_instructions(INEAX); }
    | ITINT3 { append_instructions(INT3); }
    | ITIRETD { append_instructions(IRETD); }
    | ITLODSB { append_instructions(LODSB); }
    | ITLODSD { append_instructions(LODSD); }
    | ITOUTAL { append_instructions(OUTAL); }
    | ITOUTEAX { append_instructions(OUTEAX); }
    | ITSCASB { append_instructions(SCASB); }
    | ITSCASD { append_instructions(SCASD); }
    | ITSTOSB { append_instructions(STOSB); }
    | ITSTOSD { append_instructions(STOSD); }
    | ITSYSENTER { append_instructions(SYSENTER); }
    | ITSYSEXIT { append_instructions(SYSEXIT); }
    | ITSYSCALL { append_instructions(SYSCALL); }
    | ITSYSRET { append_instructions(SYSRET); }
    | ITPAUSE { append_instructions(PAUSE); }
    | ITCPUID { append_instructions(CPUID); }
    | ITEMMS { append_instructions(EMMS); }
    | ITRSM { append_instructions(RSM); }
    | ITFNOP { append_instructions(FNOP); }
    | ITFCHS { append_instructions(FCHS); }
    | ITFABS { append_instructions(FABS); }
    | ITFTST { append_instructions(FTST); }
    | ITFXAM { append_instructions(FXAM); }
    | ITFLD1 { append_instructions(FLD1); }
    | ITFLDL2T { append_instructions(FLDL2T); }
    | ITFLDL2E { append_instructions(FLDL2E); }
    | ITFLDPI { append_instructions(FLDPI); }
    | ITFLDLG2 { append_instructions(FLDLG2); }
    | ITFLDLN2 { append_instructions(FLDLN2); }
    | ITFLDZ { append_instructions(FLDZ); }
    | ITF2XM1 { append_instructions(F2XM1); }
    | ITFYL2X { append_instructions(FYL2X); }
    | ITFPTAN { append_instructions(FPTAN); }
    | ITFPATAN { append_instructions(FPATAN); }
    | ITFXTRACT { append_instructions(FXTRACT); }
    | ITFPREM1 { append_instructions(FPREM1); }
    | ITFDECSTP { append_instructions(FDECSTP); }
    | ITFINCSTP { append_instructions(FINCSTP); }
    | ITFPREM { append_instructions(FPREM); }
    | ITFYL2XP1 { append_instructions(FYL2XP1); }
    | ITFSQRT { append_instructions(FSQRT); }
    | ITFSINCOS { append_instructions(FSINCOS); }
    | ITFRNDINT { append_instructions(FRNDINT); }
    | ITFSCALE { append_instructions(FSCALE); }
    | ITFSIN { append_instructions(FSIN); }
    | ITFCOS { append_instructions(FCOS); }
    | ITINSW { append_instructions(INSW); }
    | ITOUTSW { append_instructions(OUTSW); }
    | ITCWD { append_instructions(CWD); }
    | ITCQO { append_instructions(CQO); }
    | ITCBW { append_instructions(CBW); }
    | ITCDQE { append_instructions(CDQE); }
    | ITINVD { append_instructions(INVD); }
    | ITWBINVD { append_instructions(WBINVD); }
    | ITUD2 { append_instructions(UD2); }
    | ITCLTS { append_instructions(CLTS); }
    | ITINAX { append_instructions(INAX); }
    | ITIRETQ { append_instructions(IRETQ); }
    | ITLODSW { append_instructions(LODSW); }
    | ITLODSQ { append_instructions(LODSQ); }
    | ITOUTAX { append_instructions(OUTAX); }
    | ITRDPMC { append_instructions(RDPMC); }
    | ITRDMSR { append_instructions(RDMSR); }
    | ITRDTSC { append_instructions(RDTSC); }
    | ITSCASW { append_instructions(SCASW); }
    | ITSCASQ { append_instructions(SCASQ); }
    | ITSTOSW { append_instructions(STOSW); }
    | ITSTOSQ { append_instructions(STOSQ); }
    | ITWRMSR { append_instructions(WRMSR); }
    | ITXLATB { append_instructions(XLATB); }
    | ITJMP relative { append_instructions( JMP, D32, REL, $2 ); }
    | ITJE relative { append_instructions( JE, D32, REL, $2 ); }
    | ITJNE relative { append_instructions( JNE, D32, REL, $2 ); }
    | ITINC register { append_instructions( INC, $2.size, REG, $2.number ); }
    | ITDEC register { append_instructions( DEC, $2.size, REG, $2.number ); }
    | ITNOT register { append_instructions( NOT, $2.size, REG, $2.number ); }
    | ITNEG register { append_instructions( NEG, $2.size, REG, $2.number ); }
    | ITMUL register { append_instructions( MUL, $2.size, REG, $2.number ); }
    | ITIMUL register { append_instructions( IMUL, $2.size, REG, $2.number ); }
    | ITDIV register { append_instructions( DIV, $2.size, REG, $2.number ); }
    | ITIDIV register { append_instructions( IDIV, $2.size, REG, $2.number ); }
    | ITPOP register { append_instructions( POP, $2.size, REG, $2.number ); }
    | ITINC memory { append_instructions( INC, D32, MEM, $2 ); }
    | ITDEC memory { append_instructions( DEC, D32, MEM, $2 ); }
    | ITNOT memory { append_instructions( NOT, D32, MEM, $2 ); }
    | ITNEG memory { append_instructions( NEG, D32, MEM, $2 ); }
    | ITMUL memory { append_instructions( MUL, D32, MEM, $2 ); }
    | ITIMUL memory { append_instructions( IMUL, D32, MEM, $2 ); }
    | ITDIV memory { append_instructions( DIV, D32, MEM, $2 ); }
    | ITIDIV memory { append_instructions( IDIV, D32, MEM, $2 ); }
    | ITADD register register { append_instructions( ADD, $2.size, REG, $2.number, REG, $3.number ); }
    | ITOR register register { append_instructions( OR, $2.size, REG, $2.number, REG, $3.number ); }
    | ITADC register register { append_instructions( ADC, $2.size, REG, $2.number, REG, $3.number ); }
    | ITSBB register register { append_instructions( SBB, $2.size, REG, $2.number, REG, $3.number ); }
    | ITAND register register { append_instructions( AND, $2.size, REG, $2.number, REG, $3.number ); }
    | ITAND register immediate { append_instructions( AND, $2.size, REG, $2.number, $3.type, $3.value ); }
    | ITSUB register register { append_instructions( SUB, $2.size, REG, $2.number, REG, $3.number ); }
    | ITXOR register register { append_instructions( XOR, $2.size, REG, $2.number, REG, $3.number ); }
    | ITCMP register register { append_instructions( CMP, $2.size, REG, $2.number, REG, $3.number ); }
    | ITCMP register immediate { append_instructions( CMP, $2.size, REG, $2.number, $3.type, $3.value ); }
    | ITMOV register register { append_instructions( MOV, $2.size, REG, $2.number, REG, $3.number ); }
    | ITMOV register immediate { append_instructions( MOV, $2.size, REG, $2.number, $3.type, $3.value ); }
    | ITMOV register memory { append_instructions( MOV, $2.size, REG, $2.number, MEM, $3 ); }
    | ITMOV memory register { append_instructions( MOV, D32, MEM, $2, REG, $3.number ); }
    | ITSAR register immediate { append_instructions( SAR, $2.size, REG, $2.number, $3.type, $3.value ); }

    // #placeholder<instruction_parser_rules> END
    ;

%%
