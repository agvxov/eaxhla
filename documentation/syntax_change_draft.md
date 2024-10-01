## Problem
    + we have a serious problem expressing high level statements inline

## Fix ideas
    + blocks
    + lisp on crack

## Lisp on crack
+ this solution also allows for inline machine code
```Lisp
(set a (
    (add
        (long_ass_function_call 10)
        (even_longer_function_call 12)
    )
))
```
NOTE: prefixmaxxing is retarded

we want to be able to separate to statements
```eax
a = (
        long_ass_function_call 10
        +
        even_longer_function_call 12
    )
# means
a = ((long_ass_function_call 10) + (even_longer_function_call))
# or
#  (which is obviously ugly as fuck,
#   but must be valid work
#   unless you explicitly prohibit ';' in parens)
a = (long_ass_function_call 10; + even_longer_function_call 12)
# ------------------
# ------------------
# ------------------
nop; mov eax 60
mov eax,
    60
# same as
(nop) (mov eax 60)
(mov
    60
    eax
)
# --- NOTE: this is valid TCL :>
printf "%d: %s" \
       10 \
       "something"
# it does have a no trailing . allowed problem tho
# so what if '|' is an explicit terminator?
printf "%s and %s" \
       "Alpha" \
       "Beta" \
       ;
# now the two arguments are easy to edit (swap)
```
