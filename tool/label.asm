; @BAKE fasm $@ $*.out
format ELF64 executable 3

segment readable executable

entry $

  redo:
	mov eax, 1
	mov edi, 1
	mov esi, heyo
	mov edx, 4
	syscall
    jmp redo

segment readable writable

heyo: db "heyo", 4
