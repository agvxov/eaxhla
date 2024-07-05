%{
    #include "eaxhla.tab.h"
%}
%option noyywrap

identifier [A-z_][A-z0-9_]*
wsnl       [ \t\r\v\f\n]

%option yylineno
%option nounput noinput
%%

program{wsnl}+{identifier}{wsnl}+begin{wsnl}+  { return PROGRAM_HEAD; }
end{wsnl}+program{wsnl}+                       { return PROGRAM_TAIL; }

xor{wsnl}+     { return XOR; }
\$rax{wsnl}+   { return RAX; }
\$rbx{wsnl}+   { return RBX; }

%%

void yyfree_leftovers(void) {
    // XXX: wip
    for (int i = 0; i < yy_buffer_stack_max; i++) {
        free(yy_buffer_stack[i]);
    }
}
