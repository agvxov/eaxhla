format ELF64 executable 3

segment readable executable
entry $

	nop
	nop
	nop

; 66 44    03 0D 1A 11 00 00       | add r9w [x]
; 66 44    01 0D 11 11 00 00       | add [x] r9w
; 66 41 81 C1                FF 7F | add r9w 07fffh
; 66    81 05    00 11 00 00 FF 7F | add [x] 07fffh

fff:	nop
	add r9w, [x2]
	nop
	add [x2], r9w
	nop
	add r9w, 07fffh
	nop
	add word[x2], 07fffh

; 66 44    23 0D 1A 11 00 00       | and r9w [x]
; 66 44    21 0D 11 11 00 00       | and [x] r9w
; 66 41 81 E1                FF 7F | and r9w 07fffh
; 66    81 25    00 11 00 00 FF 7F | and [x] 07fffh

	nop
	and r9w, [x2]
	nop
	and [x2], r9w
	nop
	and r9w, 07fffh
	nop
	and word[x2], 07fffh

; 66 44    33 0D 1A 11 00 00       | xor r9w [x]
; 66 44    31 0D 11 11 00 00       | xor [x] r9w
; 66 41 81 F0                FF 7F | xor r9w 07fffh
; 66    81 35    00 11 00 00 FF 7F | xor [x] 07fffh

	nop
	xor r9w, [x2]
	nop
	xor [x2], r9w
	nop
	xor r9w, 07fffh
	nop
	xor word[x2], 07fffh

	nop
	nop
	nop

	nop
	add rcx, 07fffffffh
	nop
	or rcx, 07fffffffh
	nop
	adc rcx, 07fffffffh
	nop
	sbb rcx, 07fffffffh
	nop
	and rcx, 07fffffffh
	nop
	sub rcx, 07fffffffh
	nop
	xor rcx, 07fffffffh
	nop
	cmp rcx, 07fffffffh

	nop
	nop
	nop

	nop
	add r8b, 07fh
	nop
	xor r8b, 07fh
	nop
	add byte[x1], 07fh
	nop
	xor byte[x1], 07fh

	nop
	nop
	nop

; 4C    01 C9                            | add rcx r9
; 48    03    0D 97 10 00 00             | add rcx [x]
; 48 81    C1                FF FF FF 7F | add rcx 07fffffffh
; 48    01    0D 87 10 00 00             | add [x] rcx
; 48 81 05       7B 10 00 00 FF FF FF 7F | add [x] 07fffffffh

	nop
	add rcx, r9
	nop
	add rcx, [x8]
	nop
	add rcx, 07fffffffh
	nop
	add [x8], rcx
	nop
	add qword[x8], 07fffffffh

	nop
	nop
	nop

	nop
	add ecx, r9d
	nop
	add ecx, [x4]
	nop
	add ecx, 07fffffffh
	nop
	add [x4], ecx
	nop
	add dword[x4], 07fffffffh

	nop
	nop
	nop

	nop
	add cx, r9w
	nop
	add cx, [x2]
	nop
	add cx, 07fffh
	nop
	add [x2], cx
	nop
	add word[x2], 07fffh

	nop
	nop
	nop

	nop
	add cl, r9b
	nop
	add cl, [x1]
	nop
	add cl, 07fh
	nop
	add [x1], cl
	nop
	add byte[x1], 07fh

	nop
	nop
	nop

	nop
	enter 2, 2
	nop
	leave
	nop
	syscall
	nop
	sysret
	nop
	sysenter
	nop
	sysexit
	nop
	cpuid
	nop
	movbe rax, [x8]
	nop
	movbe rcx, [x8]
	nop
	movsq
	nop
	neg rax
	nop
	neg dword[x4]

	nop
	nop
	nop

	nop
	inc byte[x1]
	nop
	inc word[x2]
	nop
	inc dword[x4]
	nop
	inc qword[x8]
	nop
	inc cl
	nop
	inc cx
	nop
	inc ecx
	nop
	inc rcx
	nop
	inc r9b
	nop
	inc r9w
	nop
	inc r9d
	nop
	inc r9

	nop
	nop
	nop

	nop
	adc rcx, rdx;
	nop
	adc ecx, [x4];
	nop
	adc [x2], r10w;
	nop
	adc bl, 077h;
	nop
	inc ax
	nop
	lock

;~48 11 D1
;~13 0D 1B 10 00 00
;~66 44 11 15 10 10 00 00
;~80 D3 77
;~66 FF C0

;~48
;~11
;~D1
;~13
;~0D
;~66
;~44
;~11
;~15
;~80
;~D3
;~66
;~FF
;~C0
;~F0

; 48 11 D1
; 13 0D 1D 10 00 00
; 66 44 11 15 12 10 00 00
; 80 D3 77
; 66 FF C0

	;~nop
	;~nop
	;~nop

	;~nop
	;~jmp qword[x4]
	;~nop
	;~jmp qword[x8]
	;~nop
	;~jmp rax
	;~nop
	;~jmp rcx
	;~nop
	;~jmp rdx
	;~nop
	;~jmp rbx
	;~nop
	;~jmp r8
	;~nop
	;~jmp r9

	;~nop
	;~nop
	;~nop

	;~nop
	;~inc rax
	;~nop
	;~inc eax
	;~nop
	;~inc ax
	;~nop
	;~inc al
	;~nop
	;~inc qword[x8]
	;~nop
	;~inc dword[x4]
	;~nop
	;~inc word[x2]
	;~nop
	;~inc byte[x1]

	;~nop
	;~nop
	;~nop

	;~nop
	;~idiv rax
	;~nop
	;~idiv eax
	;~nop
	;~idiv ax
	;~nop
	;~idiv al
	;~nop
	;~idiv qword[x8]
	;~nop
	;~idiv dword[x4]
	;~nop
	;~idiv word[x2]
	;~nop
	;~idiv byte[x1]

	;~nop
	;~nop
	;~nop

	;~nop
	;~inc word[x2]
	;~nop
	;~dec word[x2]
	;~nop
	;~not word[x2]
	;~nop
	;~neg word[x2]
	;~nop
	;~mul word[x2]
	;~nop
	;~imul word[x2]
	;~nop
	;~div word[x2]
	;~nop
	;~idiv word[x2]

	;~nop
	;~inc ax
	;~nop
	;~dec cx
	;~nop
	;~not dx
	;~nop
	;~neg bx
	;~nop
	;~mul cx
	;~nop
	;~imul cx
	;~nop
	;~div cx
	;~nop
	;~idiv cx

	nop
	nop
	nop

nnn:	nop
	jmp nnn
	nop
	jmp fff
	nop
	jmp rax
	nop
	jmp rcx
	nop
	jmp r8
	nop
	jmp r9
	nop
	jmp word[x2]
	nop
	jmp qword[x8]

	nop
	nop
	nop

	nop
	jo nnn
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
	nop
	nop

	nop
	cmovo cx, r9w
	nop
	cmovg cx, r9w
	nop
	cmovo ecx, r9d
	nop
	cmovg ecx, r9d
	nop
	cmovo rcx, r9
	nop
	cmovg rcx, r9 ; 49 0F 4F C9
	nop
	xor rcx, rcx
	nop
	xor r9, rcx;+1
	nop
	xor rcx, r9;+4
	nop
	xor r9, r9;+1+4

	nop
	nop
	nop

	nop
	cmovg rcx, rcx; 48 0F 4F C9
	nop
	cmovg rcx, r9; 49 0F 4F C9
	nop
	cmovg r9, rcx; 4C 0F 4F C9
	nop
	cmovg r9, r9; 4D 0F 4F C9

	nop

	nop
	nop
	nop

segment readable writable

x1: db 011h
x2: dw 01122h
x4: dd 011223344h
x8: dq 01122334455667788h

;    4C    01 C9                            | add rcx r9
;    48    03    0D 97 10 00 00             | add rcx [x]
;    48 81 C1                   FF FF FF 7F | add rcx 07fffffffh
;    48    01    0D 87 10 00 00             | add [x] rcx
;    48 81 05       7B 10 00 00 FF FF FF 7F | add qword[x] 07fffffffh

;    44    01 C9                            | add ecx r9d
;          03    0D 69 10 00 00             | add ecx [x]
;       81 C1                   FF FF FF 7F | add ecx 07fffffffh
;          01    0D 5B 10 00 00             | add [x] ecx
;       81 05       50 10 00 00 FF FF FF 7F | add dword[x] 07fffffffh

; 66 44    01 C9                            | add cx r9w
; 66       03    0D 3E 10 00 00             | add cx [x]
; 66    81 C1                   FF 7F       | add cx 07fffh
; 66       01    0D 30 10 00 00             | add [x] cx
; 66    81 05       26 10 00 00 FF 7F       | add word[x] 07fffh

;    44    00 C9                            | add cl r9b
;          02    0D 17 10 00 00             | add cl [x]
;       80 C1                   7F          | add cl 07fh
;          00    0D 0C 10 00 00             | add [x] cl
;       80 05       04 10 00 00 7F          | add byte[x] 07fh
