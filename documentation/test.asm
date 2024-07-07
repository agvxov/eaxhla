format ELF64 executable 3

segment readable executable
entry $

	nop
	nop
	nop

	add rcx, r9
	add rcx, [x8]
	add rcx, 11223344h
	add [x8], rcx
	add qword[x8], 11223344h
	or  ecx, r9d
	or  ecx, [x4]
	or  ecx, 11223344h
	or  [x4], ecx
	or  dword[x4], 11223344h
	adc cx, r9w
	adc cx, [x2]
	adc cx, 1122h
	adc [x2], cx
	adc word[x2], 1122h
	sbb cl, r9b
	sbb cl, [x1]
	sbb cl, 11h
	sbb [x1], cl
	sbb byte[x1], 11h

	;~add rcx  r9                      | ADD D64 REG R1 REG R9
	;~4C 01 C9                         | 4C 01 C9
	;~add rcx  [x8]                    | ADD D64 REG R1 MEM 0
	;~48 03 0D E1 11 00 00             | 48 03 0D FF FF FF FF
	;~add rcx  11223344h               | ADD D64 REG R1 IMM 0X11223344U
	;~48 81 C1 44 33 22 11             | 48 81 C2 44 33 22 11
	      ;~^^                                 ^^
	;~add [x8] rcx                     | ADD D64 MEM 0  REG R9
	;~48 01 0D D3 11 00 00             | 4C 01 0D FF FF FF FF
	;~^^                                 ^^
	;~add [x8] 11223344h               | ADD D64 MEM 0  IMM 0X11223344U
	;~48 81 05 C8 11 00 00 44 33 22 11 | 48 81 05 FF FF FF FF 44 33 22 11

	nop
	nop
	nop

	xor rcx, r9
	xor rcx, [x8]
	xor rcx, 11223344h
	xor [x8], rcx
	xor qword[x8], 11223344h
	xor ecx, r9d
	xor ecx, [x4]
	xor ecx, 11223344h
	xor [x4], ecx
	xor dword[x4], 11223344h
	xor cx, r9w
	xor cx, [x2]
	xor cx, 1122h
	xor [x2], cx
	xor word[x2], 1122h
	xor cl, r9b
	xor cl, [x1]
	xor cl, 11h
	xor [x1], cl
	xor byte[x1], 11h

	nop
	nop
	nop

	xor rcx, r9
	xor rcx, [x8]
	xor rcx, 11223344h
	xor [x8], rcx
	xor qword[x8], 11223344h
	xor ecx, r9d
	xor ecx, [x4]
	xor ecx, 11223344h
	xor [x4], ecx
	xor dword[x4], 11223344h
	xor cx, r9w
	xor cx, [x2]
	xor cx, 1122h
	xor [x2], cx
	xor word[x2], 1122h
	xor cl, r9b
	xor cl, [x1]
	xor cl, 11h
	xor [x1], cl
	xor byte[x1], 11h

	nop
	nop
	nop

	inc rcx
	inc qword[x8]
	inc ecx
	inc dword[x4]
	inc cx
	inc word[x2]
	inc cl
	inc byte[x1]

	nop
	nop
	nop

	div r9
	div qword[x8]
	div r9d
	div dword[x4]
	div r9w
	div word[x2]
	div r9b
	div byte[x1]

	nop
	nop
	nop

	ret
	leave
	lock
	hlt

	nop
	nop
	nop

	sysenter
	sysexit
	syscall
	sysret
	pause
	cpuid

	;~nop
	;~nop
	;~nop

	;~jo fff
	;~jno fff
	;~jb fff
	;~jae fff
	;~je fff
	;~jne fff
	;~jbe fff
	;~ja fff
	;~js fff
	;~jns fff
	;~jpe fff
	;~jpo fff
	;~jl fff
	;~jge fff
	;~jle fff
	;~jg fff

	;~nop
	;~nop
	;~nop

	;~cmovo rax, rcx
	;~cmovno rax, rcx
	;~cmovb rax, rcx
	;~cmovae rax, rcx
	;~cmove rax, rcx
	;~cmovne rax, rcx
	;~cmovbe rax, rcx
	;~cmova rax, rcx
	;~cmovs rax, rcx
	;~cmovns rax, rcx
	;~cmovpe rax, rcx
	;~cmovpo rax, rcx
	;~cmovl rax, rcx
	;~cmovge rax, rcx
	;~cmovle rax, rcx
	;~cmovg rax, rcx

	nop
	nop
	nop

	;mov
	;jmp

	nop
	nop
	nop

segment readable writable

x1: db 011h
x2: dw 01122h
x4: dd 011223344h
x8: dq 01122334455667788h
