%{
    #include <stdlib.h>
    #include "sds/sds.h"
    #include "eaxhla.tab.h"

    sds string_literal_buffer;

    #define YY_USER_INIT \
        string_literal_buffer = sdsnew("");
%}
%option noyywrap

identifier [A-Za-z_][A-Za-z0-9_]*
wsnl       [ \t\r\v\f\n]

hex        [0123456789abcdef]
uhex       [0123456789ABCDEF]

%x IN_COMMENT IN_MULTILINE_COMMENT
%x IN_STRING
%x IN_END IN_UNKNOWN_END
%x IN_ARTIMETRIC_BLOCK

%option nodefault
%option yylineno
%option nounput noinput
%%

{wsnl}*  { ; }

begin        { return MYBEGIN; }

program                    { return PROGRAM; }
procedure                  { return PROCEDURE; }
repeat                     { return REPEAT; }
if                         { return IF; }
then                       { return THEN; }
machine                    { return MACHINE; }
library                    { return LIBRARY; }

break                      { return BREAK; }
until                      { return UNTIL; }
exit                       { return EXIT;  }

end { BEGIN IN_END; }

fast           { return FAST; }
unix           { return UNIX; }

    /* #placeholder<register_scanner_instructions> END
    */

in         { return TIN; }
\=         { return '='; }

s8         { return  S8; }
s16        { return S16; }
s32        { return S32; }
s64        { return S64; }
u8         { return  U8; }
u16        { return U16; }
u32        { return U32; }
u64        { return U64; }

\"  { BEGIN IN_STRING; }

\!\=       { return ITNEQ; }
\!         { return ITNOT; }

(\/\/)|\#  { BEGIN IN_COMMENT; }
\/\*       { BEGIN IN_MULTILINE_COMMENT; }

\[|\]|\{|\}|\+|\-|\*|\/|\%|\^|\:|\<|\> {
        return yytext[0];
    }

fastcall { return FASTCALL; }

    /* --- Registers begin here --- */
    /* #placeholder<register_scanner_instructions> BEGIN
    */
rax       { return RAX; }
rcx       { return RCX; }
rdx       { return RDX; }
rbx       { return RBX; }
rsp       { return RSP; }
rbp       { return RBP; }
rsi       { return RSI; }
rdi       { return RDI; }
rg8       { return RG8; }
rg9       { return RG9; }
rg10      { return RG10; }
rg11      { return RG11; }
rg12      { return RG12; }
rg13      { return RG13; }
rg14      { return RG14; }
rg15      { return RG15; }
eax       { return EAX; }
ecx       { return ECX; }
edx       { return EDX; }
ebx       { return EBX; }
esp       { return ESP; }
ebp       { return EBP; }
esi       { return ESI; }
edi       { return EDI; }
rg8d      { return RG8D; }
rg9d      { return RG9D; }
rg10d     { return RG10D; }
rg11d     { return RG11D; }
rg12d     { return RG12D; }
rg13d     { return RG13D; }
rg14d     { return RG14D; }
rg15d     { return RG15D; }
ax        { return AX; }
cx        { return CX; }
dx        { return DX; }
bx        { return BX; }
sp        { return SP; }
bp        { return BP; }
si        { return SI; }
di        { return DI; }
r8w       { return R8W; }
r9w       { return R9W; }
r10w      { return R10W; }
r11w      { return R11W; }
r12w      { return R12W; }
r13w      { return R13W; }
r14w      { return R14W; }
r15w      { return R15W; }
al        { return AL; }
cl        { return CL; }
dl        { return DL; }
bl        { return BL; }
spl       { return SPL; }
bpl       { return BPL; }
sil       { return SIL; }
dil       { return DIL; }
r8b       { return R8B; }
r9b       { return R9B; }
r10b      { return R10B; }
r11b      { return R11B; }
r12b      { return R12B; }
r13b      { return R13B; }
r14b      { return R14B; }
r15b      { return R15B; }

    /* #placeholder<register_scanner_instructions> END
    */
    /* --- Registers end here --- */

    /* --- Instrunctions begin here --- */
nop    { return INOP; }
    /* #placeholder<instruction_scanner_instructions> BEGIN
    */
adc             { return ITADC; }
add             { return ITADD; }
and             { return ITAND; }
cmp             { return ITCMP; }
dec             { return ITDEC; }
div             { return ITDIV; }
hlt             { return ITHLT; }
idiv            { return ITIDIV; }
imul            { return ITIMUL; }
inc             { return ITINC; }
leave           { return ITLEAVE; }
lock            { return ITLOCK; }
mov             { return ITMOV; }
mul             { return ITMUL; }
neg             { return ITNEG; }
not             { return ITNOT; }
or              { return ITOR; }
pause           { return ITPAUSE; }
retf            { return ITRETF; }
retn            { return ITRETN; }
sar             { return ITSAR; }
sbb             { return ITSBB; }
sub             { return ITSUB; }
syscall         { return ITSYSCALL; }
sysenter        { return ITSYSENTER; }
sysexit         { return ITSYSEXIT; }
sysret          { return ITSYSRET; }
xor             { return ITXOR; }

    /* #placeholder<instruction_scanner_instructions> END
    */
    /* --- Instrunctions end here --- */

<IN_END>{
program        { BEGIN INITIAL; return END_PROGRAM; }
procedure      { BEGIN INITIAL; return END_PROCEDURE; }
repeat         { BEGIN INITIAL; return END_REPEAT; }
if             { BEGIN INITIAL; return END_IF; }
machine        { BEGIN INITIAL; return END_MACHINE; }
library        { BEGIN INITIAL; return END_LIBRARY; }
{wsnl}         { ; }
.              { yyless(0); BEGIN IN_UNKNOWN_END; }
}

<IN_UNKNOWN_END>{
.*  { issue_error("unknown end-sequence \033[1m'%s'\033[0m", yytext); BEGIN INITIAL; return 0; }
}

<IN_STRING>{
    /* XXX: multiline strings will die */
    /* XXX: the first WORD_SIZE_IN_BYTES bytes should be 0'd */
    /* XXX: i wanted short strings to be literals;
            this however clashes with with the sanity of machine blocks;
            those should be moved to '' (exactly like in Holy C)
    */
\"  {
        BEGIN INITIAL;
        yylval.blobval.data = malloc(sdslen(string_literal_buffer));
        memcpy(yylval.blobval.data, string_literal_buffer, sdslen(string_literal_buffer));
        yylval.blobval.len = sdslen(string_literal_buffer);

        string_literal_buffer[0] = '\0';
        sdsupdatelen(string_literal_buffer);

        return ARRAY_LITERAL;
}
\\n   { string_literal_buffer = sdscat(string_literal_buffer, "\n");}
\\a   { string_literal_buffer = sdscat(string_literal_buffer, "\a"); }
\\b   { string_literal_buffer = sdscat(string_literal_buffer, "\b"); }
\\f   { string_literal_buffer = sdscat(string_literal_buffer, "\f"); }
\\r   { string_literal_buffer = sdscat(string_literal_buffer, "\r"); }
\\t   { string_literal_buffer = sdscat(string_literal_buffer, "\t"); }
\\v   { string_literal_buffer = sdscat(string_literal_buffer, "\v"); }
\\\"  { string_literal_buffer = sdscat(string_literal_buffer, "\""); }
\\\\  { string_literal_buffer = sdscat(string_literal_buffer, "\\"); }
\\0   { string_literal_buffer = sdscatlen(string_literal_buffer, "\0", 1); }
\\x({hex}|{uhex})+  {
        long v = strtol(yytext + 2, NULL, 16);
        if (v >  255
        ||  v < -255) {
            issue_warning("hex escapes are char sized. '%ld' will be truncated", v);
        }
        string_literal_buffer = sdscatlen(string_literal_buffer, &v, 1);
}
\n    {
        issue_error("string was never terminated");

        yylval.blobval.data = strdup("");
        yylval.blobval.len  = 0;

        string_literal_buffer[0] = '\0';
        sdsupdatelen(string_literal_buffer);

        return ARRAY_LITERAL;
      }
.     { string_literal_buffer = sdscat(string_literal_buffer, yytext); }
}

<IN_COMMENT>{
\n      { BEGIN INITIAL; }
.*      { ; }
<<EOF>> { yytext = strdup("<EOF>"); issue_error("unterminated comment"); yyterminate(); }
}

<IN_MULTILINE_COMMENT>{
\*\/    { BEGIN INITIAL; }
.|\n    { ; }
<<EOF>> { yytext = strdup("<EOF>"); issue_error("unterminated comment"); yyterminate(); }
}

<INITIAL,IN_ARTIMETRIC_BLOCK>{
-?[[:digit:]]+  {
              yylval.intval = strtol(yytext, NULL, 10);
              return LITERAL;
          }

0b[01]+   {
              yylval.intval = strtol(yytext + 2, NULL, 2);
              return LITERAL;
          }

0x{hex}+  {
              yylval.intval = strtol(yytext + 2, NULL, 16);
              return LITERAL;
          }

0x{uhex}+ {
              yylval.intval = strtol(yytext + 2, NULL, 16);
              return LITERAL;
          }
}

<IN_ARTIMETRIC_BLOCK>{
<<EOF>> { yytext = strdup("<EOF>"); issue_error("unterminated artimetric block"); yyterminate(); }
}

{identifier}  { yylval.strval = strdup(yytext); return IDENTIFIER; }
{identifier}: { yylval.strval = strdup(yytext); return LABEL; }

. { issue_error("unknown symbol \033[1m'%c'\033[0m", yytext[0]); }

%%

void yyfree_leftovers(void) {
    if (yyin) {
        fclose(yyin);
    }

    yylex_destroy();

    sdsfree(string_literal_buffer);
}
