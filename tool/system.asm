; General C to fasm reference, from xolatile, with hate...
; This is not intended to be assembled or ran in current state!

format ELF64 executable 3
; Executable and linkable file format, this can be done in HLA with:
; machine
; 0x7F "ELF" ... + some more "macro" magic ...
; end machine

segment readable executable

entry $; main: ...

	; uint32_t a, b, c; c = a + b;
	mov eax, [a]
	add eax, [b]
	mov [c], eax

	; uint32_t a, b, c; c = a - b;
	mov eax, [a]
	sub eax, [b]
	mov [c], eax

	; uint32_t a, b, c; c = a * b;
	mov eax, [a]
	mul [b]
	mov [c], eax

	; uint32_t a, b, c; c = a / b;
	mov eax, [a]
	div [b]
	mov [c], eax

	; uint32_t a, b, c; c = a % b;
	mov eax, [a]
	div [b]
	mov [c], edx

	; exit (c);
	mov eax, 60
	mov edi, [c]
	syscall
	; This could be implemented also with 'end program'.

segment readable writable

a: db 11h               ; uint8_t  a = 0x11;
b: dw 1122h             ; uint16_t b = 0x1122;
c: dd 11223344h         ; uint32_t c = 0x11223344;
d: dq 1122334455667788h ; uint64_t d = 0x1122334455667788;

; I find null-terminated strings based and redpilled.
string_a: db "Heyo world!", 0     ; char   string_a [] = "Heyo world!";
string_b: db "Heyo world!", 10, 0 ; char   string_b [] = "Heyo world!\n";
string_c: db "Heyo world!", 0     ; char   string_c [] = "Heyo world!";
string_c_length: dq $-string_c    ; size_t string_c_length = sizeof (string_c);
; If you want to store the length of a string, it must be immediately after it.
