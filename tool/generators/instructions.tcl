set instructions {
    {syscall}
    {sysret}
    {sysexit}
    {sysenter}
    {leave}
    {retf}
    {retn}
    {pause}
    {hlt}
    {lock}
    {jmp  relative}
    {je   relative}
    {jne  relative}
    {inc  register}
    {dec  register}
    {not  register}
    {neg  register}
    {mul  register}
    {imul register}
    {div  register}
    {idiv register}
    {pop  register}
    {inc  memory}
    {dec  memory}
    {not  memory}
    {neg  memory}
    {mul  memory}
    {imul memory}
    {div  memory}
    {idiv memory}
    {add register register}
    {or  register register}
    {adc register register}
    {sbb register register}
    {and register register}
    {and register immediate}
    {sub register register}
    {xor register register}
    {cmp register register}
    {cmp register immediate}
    {mov register register}
    {mov register immediate}
    {mov register memory}
    {mov memory   register}
    {sar register immediate}
}
    
proc malformed_instruction {i} {
    error [format "Instruction {%s} is malformed" [join $i ", "]]
}
