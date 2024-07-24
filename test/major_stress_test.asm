format ELF64 executable 3

segment executable readable

entry main

read_character:
	nop
mov eax, 0
	nop
mov edi, r12d
	nop
mov esi, r13d
	nop
mov edx, 1
	nop
syscall
	ret

write_character:
	nop
mov eax, 1
	nop
mov edi, r12d
	nop
mov esi, r13d
	nop
mov edx, 1
	nop
syscall
	ret

write_string:
	nop
mov eax, 1
	nop
mov edi, r12d
	nop
mov esi, r13d
	nop
mov edx, r14d
	nop
syscall
	ret

write_space:
	nop
mov eax, 1
	nop
mov edi, 1
	nop
mov esi, space
	nop
mov edx, 1
	nop
syscall
	ret

write_line_feed:
	nop
mov eax, 1
	nop
mov edi, 1
	nop
mov esi, line_feed
	nop
mov edx, 1
	nop
syscall
	ret

main:
	nop
nop
nop

	nop
pop r11
	nop
cmp r11, 2
	nop
je skip
	nop
mov r12d, 1
	nop
mov r13d, failure
	nop
mov r14d, 12
	nop
call write_string
	nop
mov eax, 60
mov edi, 1
syscall
	nop
skip:

	nop
pop r11
	nop
pop r11
	nop
mov eax, 2
	nop
mov edi, r11d
	nop
xor esi, esi
	nop
xor edx, edx
	nop
syscall
	nop
mov dword[filaa], eax

	main_loop:
		nop
mov r12d, [filaa]
		nop
mov r13, bytaa
		nop
call read_character
nop
		nop
mov r10d, eax
		nop
mov r15b, [bytaa]
		nop
cmp r15b, 144
		nop
jne main_loop_skip
		nop
call write_line_feed
		nop
main_loop_skip:
nop
		nop
mov r12d, 1
		nop
mov r13d, digits
		nop
sar r15d, 4
		nop
add r13d, r15d
		nop
	call write_character
nop
		nop
mov r12d, 1
		nop
mov r13d, digits
		nop
mov r15b, [bytaa]
		nop
and r15d, 15
		nop
add r13d, r15d
		nop
	call write_character
nop
		nop
	call write_space
	nop
cmp r10d, 0
	nop
jne main_loop

	nop
	call write_line_feed

	nop
mov eax, 3
	nop
mov edi, [filaa]
	nop
syscall

	nop
mov eax, 60
mov edi, 0
syscall

	nop
nop
nop

segment readable writable

space     db ' '
line_feed db '\n'
failure   db "> nopfilaa", 10
digits    db "0123456789abcdef"
filaa      dd 0
bytaa      db 0
