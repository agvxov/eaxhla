#    XXX
#
#    |       RGXMM0 { $$.number = 0; } /* XXX */
#    |       RGXMM1 { $$.number = 0; }
#    |       RGXMM2 { $$.number = 0; }
#    |       RGXMM3 { $$.number = 0; }
#    |       RGXMM4 { $$.number = 0; }
#    |       RGXMM5 { $$.number = 0; }
#    |       RGXMM6 { $$.number = 0; }
#    |       RGXMM7 { $$.number = 0; }

set register64s {
    rax 
    rcx 
    rdx 
    rbx 
    rsp 
    rbp 
    rsi 
    rdi 
    rg8 
    rg9 
    rg10
    rg11
    rg12
    rg13
    rg14
    rg15
}

set register32s {
    eax  
    ecx  
    edx  
    ebx  
    esp  
    ebp  
    esi  
    edi  
    rg8d 
    rg9d 
    rg10d
    rg11d
    rg12d
    rg13d
    rg14d
    rg15d
}

set register16s {
    ax
    cx
    dx
    bx
    sp
    bp
    si
    di
    r8w
    r9w
    r10w
    r11w
    r12w
    r13w
    r14w
    r15w
}

set register8ls {
    al
    cl
    dl
    bl
    spl
    bpl
    sil
    dil
    r8b
    r9b
    r10b
    r11b
    r12b
    r13b
    r14b
    r15b
}

set registers [dict create]
dict set registers 64 $register64s
dict set registers 32 $register32s
dict set registers 16 $register16s
dict set registers  8 $register8ls
