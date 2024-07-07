format ELF64 executable 3

segment readable executable
entry $

	nop
	nop
	nop

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

segment readable writable

x1: db 011h
x2: dw 01122h
x4: dd 011223344h
x8: dq 01122334455667788h
