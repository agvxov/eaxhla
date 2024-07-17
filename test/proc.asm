fast procedure heyo
	s8 <> h = "Heyo world!\n"
begin
	nop mov eax 1
	nop mov edi 1
	nop mov esi h
	nop mov edx 12
	nop syscall
end procedure

fast procedure lnao
	s8 <> l = "Lnao world!!!!\n"
begin
	nop mov eax 1
	nop mov edi 1
	nop mov esi l
	nop mov edx 15
	nop syscall
end procedure

fast procedure cyaa
	s8 <> c = "Cyaa world!!!\n"
begin
	nop mov eax 1
	nop mov edi 1
	nop mov esi c
	nop mov edx 14
	nop syscall
end procedure

unix program main
	s8 <> m = "Meme world!!\n"
begin
	nop fastcall heyo
	nop mov eax 1
	nop mov edi 1
	nop mov esi m
	nop mov edx 13
	nop syscall
	nop fastcall cyaa
	nop fastcall lnao
	nop mov eax 60
	nop mov edi 60
	nop syscall
end program
