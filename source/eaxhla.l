%{
    #include <stdlib.h>
    #include "sds/sds.h"
    #include "eaxhla.h"
    #include "eaxhla.tab.h"

    sds string_literal_buffer;

    long char_literal_buffer = 0;

    #define YY_USER_INIT \
        string_literal_buffer = sdsnew("");
%}
%option noyywrap

identifier [A-Za-z_][A-Za-z0-9_]*
wsnl       [ \t\r\v\f\n]

hex        [0123456789abcdef]
uhex       [0123456789ABCDEF]

%x IN_COMMENT IN_MULTILINE_COMMENT
%x IN_CHAR IN_STRING
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

in         { return TIN; }

s8         { return  S8; }
s16        { return S16; }
s32        { return S32; }
s64        { return S64; }
u8         { return  U8; }
u16        { return U16; }
u32        { return U32; }
u64        { return U64; }

\'  { BEGIN IN_CHAR; }
\"  { BEGIN IN_STRING; }

\!\=       { return ITNEQ; }
\!         { return ITNOT; }

(\/\/)|\#  { BEGIN IN_COMMENT; }
\/\*       { BEGIN IN_MULTILINE_COMMENT; }

\[|\]|\{|\}|\+|\-|\*|\/|\%|\^|\:|\<|\>|\= {
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
r8        { return R8; }
r9        { return R9; }
r10       { return R10; }
r11       { return R11; }
r12       { return R12; }
r13       { return R13; }
r14       { return R14; }
r15       { return R15; }
eax       { return EAX; }
ecx       { return ECX; }
edx       { return EDX; }
ebx       { return EBX; }
esp       { return ESP; }
ebp       { return EBP; }
esi       { return ESI; }
edi       { return EDI; }
r8d       { return R8D; }
r9d       { return R9D; }
r10d      { return R10D; }
r11d      { return R11D; }
r12d      { return R12D; }
r13d      { return R13D; }
r14d      { return R14D; }
r15d      { return R15D; }
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
cbw             { return ITCBW; }
cdq             { return ITCDQ; }
cdqe            { return ITCDQE; }
clc             { return ITCLC; }
cld             { return ITCLD; }
cli             { return ITCLI; }
clts            { return ITCLTS; }
cmc             { return ITCMC; }
cmp             { return ITCMP; }
cpuid           { return ITCPUID; }
cqo             { return ITCQO; }
cwd             { return ITCWD; }
cwde            { return ITCWDE; }
dec             { return ITDEC; }
div             { return ITDIV; }
emms            { return ITEMMS; }
f2xm1           { return ITF2XM1; }
fabs            { return ITFABS; }
fchs            { return ITFCHS; }
fcos            { return ITFCOS; }
fdecstp         { return ITFDECSTP; }
fincstp         { return ITFINCSTP; }
fld1            { return ITFLD1; }
fldl2e          { return ITFLDL2E; }
fldl2t          { return ITFLDL2T; }
fldlg2          { return ITFLDLG2; }
fldln2          { return ITFLDLN2; }
fldpi           { return ITFLDPI; }
fldz            { return ITFLDZ; }
fnop            { return ITFNOP; }
fpatan          { return ITFPATAN; }
fprem           { return ITFPREM; }
fprem1          { return ITFPREM1; }
fptan           { return ITFPTAN; }
frndint         { return ITFRNDINT; }
fscale          { return ITFSCALE; }
fsin            { return ITFSIN; }
fsincos         { return ITFSINCOS; }
fsqrt           { return ITFSQRT; }
ftst            { return ITFTST; }
fxam            { return ITFXAM; }
fxtract         { return ITFXTRACT; }
fyl2x           { return ITFYL2X; }
fyl2xp1         { return ITFYL2XP1; }
hlt             { return ITHLT; }
idiv            { return ITIDIV; }
imul            { return ITIMUL; }
inal            { return ITINAL; }
inax            { return ITINAX; }
inc             { return ITINC; }
ineax           { return ITINEAX; }
insb            { return ITINSB; }
insd            { return ITINSD; }
insw            { return ITINSW; }
int3            { return ITINT3; }
invd            { return ITINVD; }
iretd           { return ITIRETD; }
iretq           { return ITIRETQ; }
je              { return ITJE; }
jmp             { return ITJMP; }
jne             { return ITJNE; }
leave           { return ITLEAVE; }
lock            { return ITLOCK; }
lodsb           { return ITLODSB; }
lodsd           { return ITLODSD; }
lodsq           { return ITLODSQ; }
lodsw           { return ITLODSW; }
mov             { return ITMOV; }
mul             { return ITMUL; }
neg             { return ITNEG; }
nop             { return ITNOP; }
not             { return ITNOT; }
or              { return ITOR; }
outal           { return ITOUTAL; }
outax           { return ITOUTAX; }
outeax          { return ITOUTEAX; }
outsb           { return ITOUTSB; }
outsd           { return ITOUTSD; }
outsw           { return ITOUTSW; }
pause           { return ITPAUSE; }
pop             { return ITPOP; }
popf            { return ITPOPF; }
pushf           { return ITPUSHF; }
rdmsr           { return ITRDMSR; }
rdpmc           { return ITRDPMC; }
rdtsc           { return ITRDTSC; }
retf            { return ITRETF; }
retn            { return ITRETN; }
rsm             { return ITRSM; }
sar             { return ITSAR; }
sbb             { return ITSBB; }
scasb           { return ITSCASB; }
scasd           { return ITSCASD; }
scasq           { return ITSCASQ; }
scasw           { return ITSCASW; }
stc             { return ITSTC; }
std             { return ITSTD; }
sti             { return ITSTI; }
stosb           { return ITSTOSB; }
stosd           { return ITSTOSD; }
stosq           { return ITSTOSQ; }
stosw           { return ITSTOSW; }
sub             { return ITSUB; }
syscall         { return ITSYSCALL; }
sysenter        { return ITSYSENTER; }
sysexit         { return ITSYSEXIT; }
sysret          { return ITSYSRET; }
ud2             { return ITUD2; }
wait            { return ITWAIT; }
wbinvd          { return ITWBINVD; }
wrmsr           { return ITWRMSR; }
xlatb           { return ITXLATB; }
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

<IN_CHAR>{
    /* XXX: i wanted short strings to be literals;
            this however clashes with with the sanity of machine blocks;
            those should be moved to '' (exactly like in Holy C)
    */
    /* XXX: THIS CODE IS TOTAL DEATH
    */
\n  { char_literal_buffer = '\n'; }
\'  {
        BEGIN INITIAL;
        yylval.intval = char_literal_buffer;
        char_literal_buffer = 0;
        return LITERAL;
}
.   { char_literal_buffer = yytext[0]; }
\n  { issue_error("unterminated character literal sequence"); yyterminate(); }
}

<IN_STRING>{
    /* XXX: the first WORD_SIZE_IN_BYTES bytes should be 0'd */
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
<<EOF>> { issue_error("unterminated comment"); yyterminate(); }
}

<IN_MULTILINE_COMMENT>{
\*\/    { BEGIN INITIAL; }
.|\n    { ; }
<<EOF>> { issue_error("unterminated comment"); yyterminate(); }
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
<<EOF>> { issue_error("unterminated artimetric block"); yyterminate(); }
}

{identifier}  { yylval.strval = strdup(yytext); return IDENTIFIER; }
{identifier}: {
                yytext[yyleng-1] = '\0';
                yylval.strval = strdup(yytext);
                return LABEL;
              }

. { issue_error("unknown symbol \033[1m'%c'\033[0m", yytext[0]); }

%%

void yyfree_leftovers(void) {
    if (yyin) {
        fclose(yyin);
    }

    yylex_destroy();

    sdsfree(string_literal_buffer);
}
