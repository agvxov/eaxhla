; fasm proc.asm proc && chmod +x proc && ./proc

format ELF64 executable 3

segment readable executable

entry main

heyo:	mov eax, 1
	mov edi, 1
	mov esi, h
	mov edx, 12
	syscall
	call meme
	ret

cyaa:	mov eax, 1
	mov edi, 1
	mov esi, c
	mov edx, 12
	syscall
	ret

main:	call heyo
	call cyaa
	mov eax, 60
	mov edi, 60
	syscall

meme:	mov eax, 1
	mov edi, 1
	mov esi, m
	mov edx, 12
	syscall
	ret

segment readable writable

h: db "Heyo world!", 10
c: db "Cyaa world!", 10
m: db "Meme world!", 10
