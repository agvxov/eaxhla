set instructions {
    {nop}
    {retn}
    {retf}
    {leave}
    {popf}
    {pushf}
    {syscall}
    {cpuid}
    {fnop}
    {fchs}
    {fabs}
    {ftst}
    {fxam}
    {fld1}
    {fldl2t}
    {fldl2e}
    {fldpi}
    {fldlg2}
    {fldln2}
    {fldz}
    {f2xm1}
    {fyl2x}
    {fptan}
    {fpatan}
    {fxtract}
    {fprem1}
    {fdecstp}
    {fincstp}
    {fprem}
    {fyl2xp1}
    {fsqrt}
    {fsincos}
    {frndint}
    {fscale}
    {fsin}
    {fcos}
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
    {sub register register}
    {xor register register}
    {cmp register register}
    {add register immediate}
    {or  register immediate}
    {adc register immediate}
    {sbb register immediate}
    {and register immediate}
    {sub register immediate}
    {xor register immediate}
    {cmp register immediate}
    {add register memory}
    {or  register memory}
    {adc register memory}
    {sbb register memory}
    {and register memory}
    {sub register memory}
    {xor register memory}
    {cmp register memory}
    {add memory register}
    {or  memory register}
    {adc memory register}
    {sbb memory register}
    {and memory register}
    {sub memory register}
    {xor memory register}
    {cmp memory register}
    {add memory immediate}
    {or  memory immediate}
    {adc memory immediate}
    {sbb memory immediate}
    {and memory immediate}
    {sub memory immediate}
    {xor memory immediate}
    {cmp memory immediate}
    {mov register register}
    {mov register immediate}
    {mov register memory}
    {mov memory   register}
    {mov memory   immediate}
}

proc malformed_instruction {i} {
    error [format "Instruction {%s} is malformed" [join $i ", "]]
}
