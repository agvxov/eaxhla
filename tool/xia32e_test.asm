nop mov eax 1
nop mov edi 1
nop mov esi heyo
nop mov edx 5
nop syscall
call c
nop mov eax 60
nop mov edi 60
nop syscall
. c
nop mov eax 1
nop mov edi 1
nop mov esi cyaa
nop mov edx 5
nop syscall
nop retn
nop
. heyo db 5 72 69 89 79 10
. cyaa db 5 67 89 65 65 10
end
