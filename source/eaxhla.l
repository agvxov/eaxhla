%{
    #include <stdlib.h>
    #include "eaxhla.tab.h"

    char * string_literal_buffer;
    int string_litral_buffer_size     = 0;
    int string_litral_buffer_capacity = 128;

    #define YY_USER_INIT \
        string_literal_buffer = malloc(128);
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

begin{wsnl}+ { return MYBEGIN; }

program{wsnl}+             { return PROGRAM; }
procedure{wsnl}+           { return PROCEDURE; }
loop{wsnl}+                { return TLOOP; }
if{wsnl}+                  { return IF; }
then{wsnl}+                { return THEN; }
machine{wsnl}+             { return MACHINE; }

break{wsnl}+               { return BREAK; }
exit{wsnl}+                { return EXIT;  }

end { BEGIN IN_END; }

fast{wsnl}+    { return FAST; }
unix{wsnl}+    { return UNIX; }

rax{wsnl}+   { return RAX;    }
rbx{wsnl}+   { return RBX;    }
rcx{wsnl}+   { return RCX;    }
rdx{wsnl}+   { return RDX;    }
rbp{wsnl}+   { return RBP;    }
rsp{wsnl}+   { return RSP;    }
rip{wsnl}+   { return RIP;    }
rsi{wsnl}+   { return RSI;    }
rdi{wsnl}+   { return RDI;    }
r8{wsnl}+    { return RG8;    }
r9{wsnl}+    { return RG9;    }
r10{wsnl}+   { return RG10;   }
r11{wsnl}+   { return RG11;   }
r12{wsnl}+   { return RG12;   }
r13{wsnl}+   { return RG13;   }
r14{wsnl}+   { return RG14;   }
xmm0{wsnl}+  { return RGXMM0; }
xmm1{wsnl}+  { return RGXMM1; }
xmm2{wsnl}+  { return RGXMM2; }
xmm3{wsnl}+  { return RGXMM3; }
xmm4{wsnl}+  { return RGXMM4; }
xmm5{wsnl}+  { return RGXMM5; }
xmm6{wsnl}+  { return RGXMM6; }
xmm7{wsnl}+  { return RGXMM7; }

in{wsnl}+  { return TIN; }
\=         { return '='; }

s8{wsnl}+  { return  S8; }
s16{wsnl}+ { return S16; }
s32{wsnl}+ { return S32; }
s64{wsnl}+ { return S64; }
u8{wsnl}+  { return  U8; }
u16{wsnl}+ { return U16; }
u32{wsnl}+ { return U32; }
u64{wsnl}+ { return U64; }

\"  { BEGIN IN_STRING; }

\!\=       { return NEQ; }
\!         { return TNOT; }

\/\/    { BEGIN IN_COMMENT; }
\/\*    { BEGIN IN_MULTILINE_COMMENT; }

\[|\]   { return yytext[0]; }

\+|\-|\*|\/|\%|\^|\: { return yytext[0]; }

fastcall{wsnl}+ { return FASTCALL; }

    /* --- Instrunctions begin here --- */
    /*
    xor{wsnl}+      { return TXOR; }
    inc{wsnl}+      { return TINC; }
    */
add{wsnl}+      { return IADD; }
syscall{wsnl}+  { return ISYSCALL; }
mov{wsnl}+  { return IMOV; }
xor{wsnl}+  { return IXOR; }
    /* --- Instrunctions end here --- */

<IN_END>{
program        { BEGIN INITIAL; free(scope); scope = NULL; return END_PROGRAM; }
procedure      { BEGIN INITIAL; free(scope); scope = NULL; return END_PROCEDURE; }
loop           { BEGIN INITIAL; return END_LOOP; }
if             { BEGIN INITIAL; return END_IF; }
machine        { BEGIN INITIAL; return END_MACHINE; }
{wsnl}         { ; }
.              { yyless(0); BEGIN IN_UNKNOWN_END; }
}

<IN_UNKNOWN_END>{
.*  { issue_error("unknown end-sequence \033[1m'%s'\033[0m", yytext); BEGIN INITIAL; return 0; }
}

<IN_STRING>{
    /* XXX: escapes; multiline strings will die */
\"  {
        BEGIN INITIAL;
        yylval.strval = strdup(string_literal_buffer);
        return STRING_LITERAL;
}
.   {
        if (string_litral_buffer_size + yyleng > string_litral_buffer_capacity) {
            string_litral_buffer_capacity *= 2;
            void * ignore = realloc(string_literal_buffer, string_litral_buffer_capacity);
            (void)ignore;
        }
        memcpy(string_literal_buffer + string_litral_buffer_size, yytext, yyleng);
        string_litral_buffer_size += yyleng;
    }
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

. { issue_error("scanner jammed. This is likely a bug in the program"); /* XXX some kind of emergency termination */ }

%%

void yyfree_leftovers(void) {
    if (yyin) {
        fclose(yyin);
    }

    yylex_destroy();

    free(string_literal_buffer);
}
