format ELF64

public _start

section '.text' executable

_start:

	fff:
	nop
	mov eax, 1
	nop
	mov edi, 1
	nop
	mov esi, heyo
	nop
	mov edx, 12
	nop
	syscall
	nop
	nop
	nop

	mov rax, 1122334455667788h
	nop
	mov rcx, 1122334455667788h
	nop
	mov rdx, 1122334455667788h
	nop
	mov rbx, 1122334455667788h
	nop
	mov rsp, 1122334455667788h
	nop
	mov rbp, 1122334455667788h
	nop
	mov rsi, 1122334455667788h
	nop
	mov rdi, 1122334455667788h
	nop
	mov r8,  1122334455667788h
	nop
	mov r9,  1122334455667788h
	nop
	mov r10, 1122334455667788h
	nop
	mov r11, 1122334455667788h
	nop
	mov r12, 1122334455667788h
	nop
	mov r13, 1122334455667788h
	nop
	mov r14, 1122334455667788h
	nop
	mov r15, 1122334455667788h
	nop
	nop
	nop

	mov eax, 60
	nop
	mov edi, 0
	nop
	syscall
	nop
	jmp fff
	nop
	nop
	nop

section '.data' writable

heyo: db "heyo world!", 10
