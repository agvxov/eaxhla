set instructions {
    {nop}
    {retn}
    {retf}
    {leave}
    {lock}
    {hlt}
    {popf}
    {pushf}
    {wait}
    {clc}
    {cld}
    {cli}
    {stc}
    {std}
    {sti}
    {cmc}
    {insb}
    {insd}
    {outsb}
    {outsd}
    {cdq}
    {cwde}
    {inal}
    {ineax}
    {int3}
    {iretd}
    {lodsb}
    {lodsd}
    {outal}
    {outeax}
    {scasb}
    {scasd}
    {stosb}
    {stosd}
    {sysenter}
    {sysexit}
    {syscall}
    {sysret}
    {pause}
    {cpuid}
    {emms}
    {rsm}
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
    {insw}
    {outsw}
    {cwd}
    {cqo}
    {cbw}
    {cdqe}
    {invd}
    {wbinvd}
    {ud2}
    {clts}
    {inax}
    {iretq}
    {lodsw}
    {lodsq}
    {outax}
    {rdpmc}
    {rdmsr}
    {rdtsc}
    {scasw}
    {scasq}
    {stosw}
    {stosq}
    {wrmsr}
    {xlatb}
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
