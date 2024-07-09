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

begin        { return MYBEGIN; }

program                    { return PROGRAM; }
procedure                  { return PROCEDURE; }
repeat                     { return REPEAT; }
if                         { return IF; }
then                       { return THEN; }
machine                    { return MACHINE; }

break                      { return BREAK; }
exit                       { return EXIT;  }

end { BEGIN IN_END; }

fast           { return FAST; }
unix           { return UNIX; }

rax          { return RAX;    }
rbx          { return RBX;    }
rcx          { return RCX;    }
rdx          { return RDX;    }
rbp          { return RBP;    }
rsp          { return RSP;    }
rip          { return RIP;    }
rsi          { return RSI;    }
rdi          { return RDI;    }
r8           { return RG8;    }
r9           { return RG9;    }
r10          { return RG10;   }
r11          { return RG11;   }
r12          { return RG12;   }
r13          { return RG13;   }
r14          { return RG14;   }

xmm0         { return RGXMM0; }
xmm1         { return RGXMM1; }
xmm2         { return RGXMM2; }
xmm3         { return RGXMM3; }
xmm4         { return RGXMM4; }
xmm5         { return RGXMM5; }
xmm6         { return RGXMM6; }
xmm7         { return RGXMM7; }

eax          { return EAX;    }
ebx          { return EBX;    }
ecx          { return ECX;    }
edx          { return EDX;    }
ebp          { return EBP;    }
esp          { return ESP;    }
eip          { return EIP;    }
esi          { return ESI;    }
edi          { return EDI;    }
r8d          { return RG8D;   }
r9d          { return RG9D;   }
r10d         { return RG10D;  }
r11d         { return RG11D;  }
r12d         { return RG12D;  }
r13d         { return RG13D;  }
r14d         { return RG14D;  }

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

\!\=       { return NEQ; }
\!         { return TNOT; }

\/\/    { BEGIN IN_COMMENT; }
\/\*    { BEGIN IN_MULTILINE_COMMENT; }

\[|\]|\{|\}|\+|\-|\*|\/|\%|\^|\:|\<|\> {
        return yytext[0];
    }

fastcall { return FASTCALL; }

    /* --- Instrunctions begin here --- */
    /*
    xor{wsnl}+      { return TXOR; }
    inc{wsnl}+      { return TINC; }
    */
add      { return IADD; }
syscall  { return ISYSCALL; }
mov      { return IMOV; }
xor      { return IXOR; }
    /* #placeholder<scanner_instructions> COLLAPSED
    */
    /* --- Instrunctions end here --- */

<IN_END>{
program        { BEGIN INITIAL; free(scope); scope = NULL; return END_PROGRAM; }
procedure      { BEGIN INITIAL; free(scope); scope = NULL; return END_PROCEDURE; }
repeat         { BEGIN INITIAL; return END_REPEAT; }
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
        if (yyleng <= WORD_SIZE_IN_BYTES) {
            memcpy(&yylval.intval, yytext, WORD_SIZE_IN_BYTES);
            return LITERAL;
        }

        yylval.strval = strdup(string_literal_buffer);
        return ARRAY_LITERAL;
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

. { issue_error("unknown symbol \033[1m'%c'\033[0m", yytext[0]); }

%%

void yyfree_leftovers(void) {
    if (yyin) {
        fclose(yyin);
    }

    yylex_destroy();

    free(string_literal_buffer);
}
