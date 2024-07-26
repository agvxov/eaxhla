format ELF64 executable 3

segment readable executable

entry main

char:	nop
	nop
	mov eax, 1
	nop
	mov edi, 1
	nop
	mov edx, 1
	nop
	syscall
	ret

main:	nop
	main_loop:
		nop
		mov dword[y], 0
		begin_loop_y:
		nop
		cmp dword[y], 8
		nop
		je end_loop_y
			nop
			mov dword[x], 0
			begin_loop_x:
			nop
			cmp dword[x], 8
			nop
			je end_loop_x
				nop
				mov esi, c
				nop
				call char
			nop
			inc dword[x]
			nop
			jmp begin_loop_x
			end_loop_x:
			nop
			mov esi, c
			nop
			add esi, 2
			nop
			call char
		nop
		inc dword[y]
		nop
		jmp begin_loop_y
		end_loop_y:
		nop
		mov esi, c
		nop
		add esi, 3
		nop
		call char
		nop
		inc esi
		nop
		call char
		nop
		inc esi
		nop
		call char
	nop
	jmp main_loop
	nop
	mov eax, 60
	mov edi, 0
	syscall

segment readable writable

y: dd 0
x: dd 0
p: dd 0
q: dd 0
f: db "..#.....#.#.......##............................................"
c: db ".#", 10, 27, "[H"
n: dd 0
b: db "..#.....#.#.......##............................................"
