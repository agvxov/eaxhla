format ELF64 executable 3

segment readable executable

entry $

	jmp cyaa

	mov eax, 1
	mov edi, 1
	mov esi, heyo
	mov edx, 12
	syscall

	cyaa:

	mov eax, 60
	mov edi, 60
	syscall

segment readable writable

heyo: db 'Heyo world!', 10
