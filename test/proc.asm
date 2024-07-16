; fasm proc.asm proc && chmod +x proc && ./proc

format ELF64 executable 3

segment readable executable

entry main

heyo:
	nop
	mov eax, 1
	nop
	mov edi, 1
	nop
	mov esi, h
	nop
	mov edx, 12
	nop
	syscall
	nop
	call meme
	nop
	ret

cyaa:
	nop
	mov eax, 1
	nop
	mov edi, 1
	nop
	mov esi, c
	nop
	mov edx, 12
	nop
	syscall
	nop
	ret

main:
	nop
	call heyo
	nop
	call cyaa
	nop
	mov eax, 60
	nop
	mov edi, 60
	nop
	syscall

meme:
	nop
	mov eax, 1
	nop
	mov edi, 1
	nop
	mov esi, m
	nop
	mov edx, 12
	nop
	syscall
	nop
	ret

	nop

segment readable writable

h: db "Heyo world!", 10
c: db "Cyaa world!", 10
m: db "Meme world!", 10
