format ELF64 executable 3

segment readable executable

entry $

	mov eax, 1
	mov edi, 1
	mov esi, heyo
	mov edx, 12
	syscall
	mov eax, 60
	mov edi, 60
	syscall

segment readable writable

heyo: db "Heyo world!", 10
