format ELF64 executable 3

segment readable executable

entry $

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
	mov eax, 60
	nop
	mov edi, 60
	nop
	syscall

segment readable writable

heyo: db "Heyo world!", 10
