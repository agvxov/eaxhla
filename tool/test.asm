format ELF64 executable 3

segment readable executable

entry $

	nop
	nop
	nop

	fff:

	nop
	add rcx, r9
	nop
	add rcx, [x8]
	nop
	add rcx, 11223344h
	nop
	add [x8], rcx
	nop
	add qword[x8], 11223344h
	nop
	or  ecx, r9d
	nop
	or  ecx, [x4]
	nop
	or  ecx, 11223344h
	nop
	or  [x4], ecx
	nop
	or  dword[x4], 11223344h
	nop
	adc cx, r9w
	nop
	adc cx, [x2]
	nop
	adc cx, 1122h
	nop
	adc [x2], cx
	nop
	adc word[x2], 1122h
	nop
	sbb cl, r9b
	nop
	sbb cl, [x1]
	nop
	sbb cl, 11h
	nop
	sbb [x1], cl
	nop
	sbb byte[x1], 11h
	nop
	and rcx, r9
	nop
	and rcx, [x8]
	nop
	and rcx, 11223344h
	nop
	and [x8], rcx
	nop
	and qword[x8], 11223344h
	nop
	sub ecx, r9d
	nop
	sub ecx, [x4]
	nop
	sub ecx, 11223344h
	nop
	sub [x4], ecx
	nop
	sub dword[x4], 11223344h
	nop
	xor cx, r9w
	nop
	xor cx, [x2]
	nop
	xor cx, 1122h
	nop
	xor [x2], cx
	nop
	xor word[x2], 1122h
	nop
	cmp cl, r9b
	nop
	cmp cl, [x1]
	nop
	cmp cl, 11h
	nop
	cmp [x1], cl
	nop
	cmp byte[x1], 11h
	nop
	inc rcx
	nop
	inc qword[x8]
	nop
	inc ecx
	nop
	inc dword[x4]
	nop
	inc cx
	nop
	inc word[x2]
	nop
	inc cl
	nop
	inc byte[x1]
	nop
	div r9
	nop
	div qword[x8]
	nop
	div r9d
	nop
	div dword[x4]
	nop
	div r9w
	nop
	div word[x2]
	nop
	div r9b
	nop
	div byte[x1]
	nop
	ret
	nop
	leave
	nop
	lock
	nop
	hlt
	nop
	sysenter
	nop
	sysexit
	nop
	syscall
	nop
	sysret
	nop
	pause
	nop
	cpuid
	nop
	jo fff
	nop
	jno fff
	nop
	jb fff
	nop
	jae fff
	nop
	je fff
	nop
	jne fff
	nop
	jbe fff
	nop
	ja fff
	nop
	js fff
	nop
	jns fff
	nop
	jpe fff
	nop
	jpo fff
	nop
	jl fff
	nop
	jge fff
	nop
	jle fff
	nop
	jg fff
	nop
	cmovo rax, rcx
	nop
	cmovno rax, rcx
	nop
	cmovb rax, rcx
	nop
	cmovae rax, rcx
	nop
	cmove rax, rcx
	nop
	cmovne rax, rcx
	nop
	cmovbe rax, rcx
	nop
	cmova rax, rcx
	nop
	cmovs rax, rcx
	nop
	cmovns rax, rcx
	nop
	cmovpe rax, rcx
	nop
	cmovpo rax, rcx
	nop
	cmovl rax, rcx
	nop
	cmovge rax, rcx
	nop
	cmovle rax, rcx
	nop
	cmovg rax, rcx
	;~;mov
	;~;jmp

	nop

	nop
	nop
	nop

segment readable writable

x1: db 011h
x2: dw 01122h
x4: dd 011223344h
x8: dq 01122334455667788h
