set register64s {
    rax 
    rcx 
    rdx 
    rbx 
    rsp 
    rbp 
    rsi 
    rdi 
    r8 
    r9 
    r10
    r11
    r12
    r13
    r14
    r15
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
    r8d 
    r9d 
    r10d
    r11d
    r12d
    r13d
    r14d
    r15d
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
