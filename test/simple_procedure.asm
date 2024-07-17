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
	ret

cyaa:
	nop
	mov eax, 1
	nop
	mov edi, 1
	nop
	mov esi, c
	nop
	mov edx, 14
	nop
	syscall
	ret

main:
	nop
	call heyo
	nop
	mov eax, 1
	nop
	mov edi, 1
	nop
	mov esi, m
	nop
	mov edx, 13
	nop
	syscall
	nop
	call cyaa
	lod:
	nop
	mov eax, 60
	nop
	mov edi, 60
	nop
	syscall
	nop
	nop
	nop
	nop
	call rax
	nop
	call rcx
	nop
	call r8
	nop
	call r9
	nop
	call loc
	nop
	call lod
	nop
	nop
	nop

segment readable writable

h: db "Heyo world!", 10
m: db "Meme world!!", 10
c: db "Cyaa world!!!", 10

loc: dq 0

;~FASM                                                                            EAXHLA
;~7F 45 4C 46 02 01 01 03 00 00 00 00 00 00 00 00                                 7F 45 4C 46 02 01 01 03 00 00 00 00 00 00 00 00
;~02 00 3E 00 01 00 00 00 EA 00 40 00 00 00 00 00                                 02 00 3E 00 01 00 00 00 7C 00 00 00 00 00 00 00
;~40 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00                                 40 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
;~00 00 00 00 40 00 38 00 02 00 40 00 00 00 00 00                                 00 00 00 00 40 00 38 00 02 00 40 00 00 00 00 00
;~01 00 00 00 05 00 00 00 00 00 00 00 00 00 00 00                                 01 00 00 00 05 00 00 00 00 00 00 00 00 00 00 00
;~00 00 40 00 00 00 00 00 00 00 40 00 00 00 00 00                                 00 00 40 00 00 00 00 00 00 00 40 00 00 00 00 00
;~20 01 00 00 00 00 00 00 20 01 00 00 00 00 00 00                                 53 01 00 00 00 00 00 00 53 01 00 00 00 00 00 00
;~00 10 00 00 00 00 00 00 01 00 00 00 06 00 00 00                                 00 10 00 00 00 00 00 00 01 00 00 00 06 00 00 00
;~20 01 00 00 00 00 00 00 20 11 40 00 00 00 00 00                                 53 01 00 00 00 00 00 00 53 11 40 00 00 00 00 00
;~20 11 40 00 00 00 00 00 27 00 00 00 00 00 00 00                                 53 11 40 00 00 00 00 00 0C 00 00 00 00 00 00 00
;~27 00 00 00 00 00 00 00 00 10 00 00 00 00 00 00                                 0C 00 00 00 00 00 00 00 00 10 00 00 00 00 00 00
;~90 B8 01 00 00 00                                                               90 48 B8 01 00 00 00
;~90 BF 01 00 00 00                                                               90 48 BF 01 00 00 00
;~90 BE 20 11 40 00                                                               90 48 BE 2C 11 40 00
;~90 BA 0C 00 00 00                                                               90 48 BA 0C 00 00 00
;~90 0F 05 C3                                                                     90 0F 05 C3
;~90 B8 01 00 00 00                                                               90 48 B8 01 00 00 00
;~90 BF 01 00 00 00                                                               90 48 BF 01 00 00 00
;~90 BE 39 11 40 00                                                               90 48 BE 45 11 40 00
;~90 BA 0E 00 00 00                                                               90 48 BA 0E 00 00 00
;~90 0F 05 C3                                                                     90 0F 05 C3
;~90 E8 C0 FF FF FF                                                               90 E8 BA FF FF FF
;~90 B8 01 00 00 00                                                               90 48 B8 01 00 00 00
;~90 BF 01 00 00 00                                                               90 48 BF 01 00 00 00
;~90 BE 2C 11 40 00                                                               90 48 BE 38 11 40 00
;~90 BA 0D 00 00 00                                                               90 48 BA 0D 00 00 00
;~90 0F 05                                                                        90 0F 05
;~90 E8 BC FF FF FF                                                               90 E8 B5 FF FF FF
;~90 B8 3C 00 00 00                                                               90 48 B8 3C 00 00 00
;~90 BF 3C 00 00 00                                                               90 48 BF 3C 00 00 00
;~90 0F 05                                                                        90 0F 05
;~48 65 79 6F 20 77 6F 72 6C 64 21 0A                                             48 65 79 6F 20 77 6F 72 6C 64 21 0A
;~4D 65 6D 65 20 77 6F 72 6C 64 21 21 0A                                          4D 65 6D 65 20 77 6F 72 6C 64 21 21 0A
;~43 79 61 61 20 77 6F 72 6C 64 21 21 21 0A                                       43 79 61 61 20 77 6F 72 6C 64 21 21 21 0A
