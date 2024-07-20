format ELF64 executable 3
segment readable executable
entry main
heyo:	nop
	mov eax, 1
	nop
	mov edi, 1
	nop
	mov esi, heyo_data
	nop
	mov edx, 12
	nop
	syscall
	ret
cyaa:	nop
	mov eax, 1
	nop
	mov edi, 1
	nop
	mov esi, cyaa_data
	nop
	mov edx, 12
	nop
	syscall
	ret
main:	nop
	mov r10d, 11223344h
	nop
	call heyo
	nop
	call cyaa
	nop
	call heyo
	nop
	call cyaa
	nop
	mov eax, 60
	mov edi, 60
	syscall
	nop
segment readable writable
heyo_data: db "Heyo world!", 10
cyaa_data: db "Cyaa world!", 10
