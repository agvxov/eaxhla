%{
    #include "eaxhla.tab.h"
%}
%option noyywrap

identifier [A-z_][A-z0-9_]*
wsnl       [ \t\r\v\f\n]

hex        [0123456789abcdef]
uhex       [0123456789ABCDEF]

%x IN_COMMENT IN_MULTILINE_COMMENT

    // XXX: ?!
%x IN_DECLARE

%option nodefault
%option yylineno
%option nounput noinput
%%

{wsnl}*  { ; }

begin{wsnl}+ { return MYBEGIN; }

program{wsnl}+             { return PROGRAM; }
end{wsnl}+program{wsnl}+   { return END_PROGRAM; }
procedure{wsnl}+           { return PROCEDURE; }
end{wsnl}+procedure{wsnl}+ { return END_PROCEDURE; }
loop{wsnl}+                { return TLOOP; }
end{wsnl}+loop{wsnl}+      { return END_LOOP; }
if{wsnl}+                  { return IF; }
then{wsnl}+                { return THEN; }
end{wsnl}+if{wsnl}+        { return END_IF; }

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

s8{wsnl}+  { return  S8; }
s16{wsnl}+ { return S16; }
s32{wsnl}+ { return S32; }
s64{wsnl}+ { return S64; }
u8{wsnl}+  { return  U8; }
u16{wsnl}+ { return U16; }
u32{wsnl}+ { return U32; }
u64{wsnl}+ { return U64; }

0b[01]+             {
                        return LITERAL;
                    }

0x{hex}+  {
              sscanf(yytext + 2, "%x", &yylval.intval);
              return LITERAL;
          }

0x{uhex}+ {
              sscanf(yytext + 2, "%X", &yylval.intval);
              return LITERAL;
          }

xor{wsnl}+     { return TXOR; }

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

{identifier}{wsnl}+ { yylval.strval = strdup(yytext); return IDENTIFIER; }

%%

void set_state(int state) {
    switch (state) {
        case STATE_DECLARE: {
            BEGIN IN_DECLARE;
        } break;
    }
}

void yyfree_leftovers(void) {
    // XXX: wip
    for (size_t i = 0; i < yy_buffer_stack_max; i++) {
        free(yy_buffer_stack[i]);
    }
}
