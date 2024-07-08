format ELF64

public _start

section '.text' executable

_start:

	mov eax, 1
	mov edi, 1
	mov esi, heyo
	mov edx, 12
	syscall

	mov eax, 60
	mov edi, 0
	syscall

section '.data' writable

heyo: db "heyo world!", 10
