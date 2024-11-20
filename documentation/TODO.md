+ terrible infinit error on ../debug/comment\_as\_space.eax

- X: Implement other utility instructions.
- X: Implement BMI1 and BMI2 into assembler.
- X: Write 'cpuid' tool to check for extensions.
- X: Check out AES and SHA integration.

Intel/AMD x86-64 CPU ISA Extensions:

0) BMI1 -- I'm fine with these.
    A) andn, bextr, blsi, blsmsk, blsr, tzcnt, lzcnt (?);
1) BMI2 -- Most CPUs have these too.
    A) bzhi, mulx, pdep, pext, rorx, sarx, shrx, shlx;
2) MPX -- Mostly useless instructions, probable attack vector, not publicly disclosed tho.
3) ADX -- Only two instructions, but my CPU doesn't support it, so no.
4) SMAP -- This was added to make the glowie backdoors easier to use.
5) MMX -- Useless remnaint of the past as far as I understand...
6) SHA -- This might be useful, I'd like to see if I can implement it.
7) AES -- Same case, even tho I don't know if anyone would ever use this.
8) RTM/HLE -- This has history of being buggy and Incel disabling it via uops update.
9) SSE1 -- It would be good to have it.
    A) movss, movaps, movups, movlps, movhps, movlhps, movhlps, movmskps;
    B) addss, subss, mulss, divss, rcpss, sqrtss, maxss, minss, rsqrtss;
    C) addps, subps, mulps, divps, rcpps, sqrtps, maxps, minps, rsqrtps, andps, orps, xorps, andnps;
    D) cmpss, comiss, ucomiss, cmpps, shufps, unpckhps, unpcklps;
    E) cvtsi2ss, cvtss2si, cvttss2si, cvtpi2ps, cvtps2pi, cvttps2pi;
    F) pmulhuw, psadbw, pavgb, pavgw, pmaxub, pminub, pmaxsw, pminsw, pextrw, pinsrw, pmovmskb, pshufw;
10) SSE2 -- This doesn't introduces fully new instructions? I might make it...
11) SSE3 -- What was the fucking point, all instructions are garbage...
12) SSE4 -- Definitely not, it has several iterations, and not SIMD-only, fuck that.
13) AVX1 -- Probably not going to implement it, but it can be useful.
    A) vbroadcastss, vbroadcastsd, vbroadcastf128, vinsertf128, vextractf128, vmaskmovps, vmaskmovpd;
    B) vpermilps, vpermilpd, vperm2f128, vtestps, vtestpd, vzeroall, vzeroupper;
14) AVX2 -- Is it worth it? My CPU supports it, we'll see, tho I hate AVX.
    A) vpbroadcastb, vpbroadcastw, vpbroadcastd, vpbroadcastq;
    B) vgatherdpd, vgatherqpd, vgatherdps, vgatherqps, vpgatherdd, vpgatherdq, vpgatherqd, vpgatherqq;
    C) vpmaskmovd, vpmaskmovq, vpermps, vpermd, vpermpd, vpermq, vperm2i128;
    D) vpblendd, vpsllvd, vpsllvq, vpsrlvd, vpsrlvq, vpsravd;
15) AVX5 -- Fuck this.
15) AVX10 -- I'd rather die.
