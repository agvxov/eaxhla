format ELF64 executable 3

segment readable executable

	mov eax, 1
	mov edi, 1
	mov esi, heyo
	mov edx, 12
	syscall

	mov eax, 60
	mov edi, 0
	syscall

segment readable writable

heyo: db "heyo world!", 10
