%{
    #include <stdlib.h>
    #include "eaxhla.tab.h"
    #include "table.h"

    char * string_literal_buffer;
    int string_litral_buffer_size     = 0;
    int string_litral_buffer_capacity = 128;

    #define YY_USER_INIT \
        string_literal_buffer = malloc(128);

    void yyfree_leftovers(void);
    void yyerror(void);
%}
%option noyywrap

identifier [A-Za-z_][A-Za-z0-9_]*
wsnl       [ \t\r\v\f\n]

hex        [0123456789abcdef]
uhex       [0123456789ABCDEF]

%x IN_COMMENT IN_MULTILINE_COMMENT
%x IN_STRING
%x IN_END

    // XXX: ?!
%x IN_DECLARE
%x IN_UGH

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
break{wsnl}+               { return BREAK; }

end { BEGIN IN_END; }

<IN_END>{
program        { BEGIN INITIAL; return END_PROGRAM; }
procedure      { BEGIN INITIAL; return END_PROCEDURE; }
loop           { BEGIN INITIAL; return END_LOOP; }
if             { BEGIN INITIAL; return END_IF; }
{wsnl}         { ; }
.              { yyerror(); }
}

fast{wsnl}+    { return FAST; }
unix{wsnl}+    { return UNIX; }

\$rax{wsnl}+   { return RAX; }
\$rbx{wsnl}+   { return RBX; }
\$rcx{wsnl}+   { return RCX; }
\$rdx{wsnl}+   { return RDX; }
\$rbp{wsnl}+   { return RBP; }
\$rsp{wsnl}+   { return RSP; }
\$rip{wsnl}+   { return RIP; }

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

-?[[:digit:]]+  {
                    yylval.intval = strtol(yytext, NULL, 10);
                    return LITERAL;
}

0b[01]+             {
                        yylval.intval = strtol(yytext, NULL, 2);
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

\"  { BEGIN IN_STRING; }

<IN_STRING>{
    /* XXX: escapes */
\"  {
        BEGIN INITIAL;
        yylval.strval = strdup(string_literal_buffer);
        puts(string_literal_buffer);
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

xor{wsnl}+     { return TXOR; }
inc{wsnl}+     { return TINC; }

fastcall{wsnl}+ { return FASTCALL; }

\/\/    { BEGIN IN_COMMENT; }
\/\*    { BEGIN IN_MULTILINE_COMMENT; }

<IN_COMMENT>{
\n      { BEGIN INITIAL; }
.*      { ; }
}

<IN_MULTILINE_COMMENT>{
\*\/    { BEGIN INITIAL; }
.|\n    { ; }
}

\[      { BEGIN IN_UGH; }

<IN_UGH>{
\]      { BEGIN INITIAL; yylval.strval = strdup(yytext); return IDENTIFIER; }
    /* XXX! */
.|\n    { ; }
}

{identifier} { yylval.strval = strdup(yytext); return IDENTIFIER; }

. { yyerror(); }

%%

void set_state(int state) {
    switch (state) {
        case STATE_DECLARE: {
            BEGIN IN_DECLARE;
        } break;
    }
}

void yyfree_leftovers(void) {
    if (yyin) {
        fclose(yyin);
    }

    yylex_destroy();

    free(string_literal_buffer);
}
