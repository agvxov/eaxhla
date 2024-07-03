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

	nop
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
	adc rcx, rdx; 48 31 D1
	nop
	adc ecx, [x4]; 33 0D 14 10 00 00
	nop
	adc [x2], r10w; 66 44 31 15 09 10 00 00
	nop
	adc bl, 077h; 80 F3 77

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
