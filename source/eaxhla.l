%{
    #include "eaxhla.tab.h"
%}
%option noyywrap

identifier [A-z_][A-z0-9_]*
wsnl       [ \t\r\v\f\n]

%%

program{wsnl}+{identifier}{wsnl}+begin{wsnl}+  { return PROGRAM_HEAD; }
end{wsnl}+program{wsnl}+                       { return PROGRAM_TAIL; }

xor{wsnl}+     { return XOR; }
\$rax{wsnl}+   { return RAX; }
\$rbx{wsnl}+   { return RBX; }

%%
