format ELF64 executable 3

segment readable executable

	mov al,  1
	mov dil, 1
	mov esi, heyo
	mov dl,  5
	syscall

	mov al,  60
	mov dil, 0
	syscall

segment readable writable

heyo: db 72, 69, 89, 79, 10

;~MOV D8 REG R0 IMM 1
;~MOV D8 REG R7 IMM 1                                                        ***
;~MOV D32 REG R6 IMM 4198597
;~MOV D8 REG R2 IMM 5
;~SYSCALL
;~MOV D8 REG R0 IMM 60
;~MOV D8 REG R7 IMM 0                                                        ***
;~SYSCALL

;~7F 45 4C 46 02 01 01 03
;~00 00 00 00 00 00 00 00
;~02 00 3E 00 01 00 00 00
;~B0 00 40 00 00 00 00 00
;~40 00 00 00 00 00 00 00
;~00 00 00 00 00 00 00 00
;~00 00 00 00 40 00 38 00
;~02 00 40 00 00 00 00 00

;~01 00 00 00 05 00 00 00
;~00 00 00 00 00 00 00 00
;~00 00 40 00 00 00 00 00
;~00 00 40 00 00 00 00 00
;~C5 00 00 00 00 00 00 00
;~C5 00 00 00 00 00 00 00
;~00 10 00 00 00 00 00 00

;~01 00 00 00 06 00 00 00
;~C5 00 00 00 00 00 00 00
;~C5 10 40 00 00 00 00 00
;~C5 10 40 00 00 00 00 00
;~05 00 00 00 00 00 00 00
;~05 00 00 00 00 00 00 00
;~00 10 00 00 00 00 00 00

;~B0 01
;~40 B7 01
;~BE C5 10 40 00
;~B2 05
;~0F 05

;~B0 3C
;~40 B7 00
;~0F 05

;~48 45 59 4F 0A
