format ELF64 executable 3

segment executable readable

entry R_ENTRY_POINT

R_F2_READ_CHARACTER:
	mov rax, R_SYSTEM_CALL_READ
	mov rdi, r12
	mov rsi, r13
	mov rdx, 1
	syscall
	ret

R_F2_WRITE_CHARACTER:
	mov rax, R_SYSTEM_CALL_WRITE
	mov rdi, r12
	mov rsi, r13
	mov rdx, 1
	syscall
	ret

R_F3_WRITE_STRING:
	mov rax, R_SYSTEM_CALL_WRITE
	mov rdi, r12
	mov rsi, r13
	mov rdx, r14
	syscall
	ret

R_ENTRY_POINT:
; Main function start.
	pop r11    ; Pop argument count from the stack to r13 register.
	cmp r11, 2 ; Compare argument count to argument 0.
	je SKIP    ; Skip if equal, write message and exit if unequal.
	mov r12, R_STANDARD_OUTPUT
	mov r13, R_A1_FAILURE_ARGUMENT_COUNT
	mov r14, R_S8_FAILURE_ARGUMENT_COUNT
	call R_F3_WRITE_STRING
	mov rax, R_SYSTEM_CALL_EXIT
	mov rdi, R_FAILURE
	syscall
	SKIP:

	; Open default file in read only mode.
	pop r11 ; Ignore argument 0.
	pop r11 ; Pop argument 1 from the stack to r12 register.
	mov rax, R_SYSTEM_CALL_OPEN
	mov rdi, r11
	xor rsi, rsi
	xor rdx, rdx
	syscall
	mov [R_D8_FILE], rax

	MAIN_LOOP:
		; Read 1 byte from default file.
		mov r12, [R_D8_FILE]
		mov r13, R_D1_BYTE
		call R_F2_READ_CHARACTER

		; Write new line if byte equals "nop" instruction.
		mov r10, rax          ; Store stop signal to r10.
		mov r15b, [R_D1_BYTE] ; Store byte data to r15b.
		cmp r15b, 144         ; Compare byte to "nop" instruction.
		jne MAIN_LOOP_SKIP    ; Skip if not "nop" instruction.
		mov r12, R_STANDARD_OUTPUT
		mov r13, R_D1_NEW_LINE
		call R_F2_WRITE_CHARACTER
		MAIN_LOOP_SKIP:

		mov r12, R_STANDARD_OUTPUT
		mov r13, R_A1_DIGITS
		sar r15, 4
		add r13, r15
		call R_F2_WRITE_CHARACTER

		mov r12, R_STANDARD_OUTPUT
		mov r13, R_A1_DIGITS
		mov r15b, [R_D1_BYTE]
		and r15, 15
		add r13, r15
		call R_F2_WRITE_CHARACTER

		mov r12, R_STANDARD_OUTPUT
		mov r13, R_D1_SPACE
		call R_F2_WRITE_CHARACTER

	cmp r10, 0    ; Compare stop signal to 0.
	jne MAIN_LOOP ; Loop back if not equal.

	mov r12, R_STANDARD_OUTPUT
	mov r13, R_D1_NEW_LINE
	call R_F2_WRITE_CHARACTER

	; Close default file.
	mov rax, R_SYSTEM_CALL_CLOSE
	mov rdi, [R_D8_FILE]
	syscall

	mov rax, R_SYSTEM_CALL_EXIT
	mov rdi, R_SUCCESS
	syscall
; Main function end.

segment readable writable

; Program variables and constants.

R_D8_FILE     dq 0
R_D1_BYTE     db 0
R_A1_DIGITS   db '0123456789ABCDEF', 0
R_D1_SPACE    db 32
R_D1_NEW_LINE db 10

; Program strings and string sizes.

R_A1_FAILURE_ARGUMENT_COUNT db 'Failure: Argument count must be one.', 10, 0

R_S8_FAILURE_ARGUMENT_COUNT = $-R_A1_FAILURE_ARGUMENT_COUNT

; GNU/Linux OS system call enumeration.

R_SYSTEM_CALL_READ  = 0
R_SYSTEM_CALL_WRITE = 1
R_SYSTEM_CALL_OPEN  = 2
R_SYSTEM_CALL_CLOSE = 3
R_SYSTEM_CALL_EXIT  = 60

; GNU/Linux OS standard input and output descriptors and exit codes.

R_STANDARD_INPUT  = 0
R_STANDARD_OUTPUT = 1

R_SUCCESS = 0
R_FAILURE = 1
