format ELF64 executable 3

; fasm multiply.asm multiply
; chmod +x multiply
; ./multiply
; echo $?
; > 24

segment readable executable

entry $

	mov eax, [a]
	mov ecx, [b]
	mul ecx
	mov [c], eax

	; exit (c);
	mov eax, 60
	mov edi, [c]
	syscall

segment readable writable

a: dd 4
b: dd 6
c: dd 0; Expecting 24.
