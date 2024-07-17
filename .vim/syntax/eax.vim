" Vim syntax file
" Language:           EAX HLA
" Last Change:        2024
" Former Maintainer:  Anon8697
"
if exists("b:current_syntax")
  finish
endif

"runtime! syntax/c.vim
"unlet b:current_syntax

syn region eaxSingleLineComment  start=+//+ end=+\n+
syn region eaxSingleLineComment2 start=+#+ end=+\n+
syn region eaxMultiLineComment  start=+\/\*+ end=+\*\/+
syn keyword eaxSpecifier fast unix in
syn keyword eaxKeyword program machine procedure begin until repeat break if then else end
syn keyword eaxType u8 u16 u32 u64 s8 s16 s32 s64
syn keyword eaxInstruction inc xor mov
syn keyword eaxInstructionLike fastcall exit
syn keyword eaxRegister rax rbx rcx rdx rbp rsp rip rdi r7 r8 r9 r10 r11 r12 r13 r14 r15
syn match eaxInt     "\-\?\d\+"
syn match eaxHex     "0x[0-9a-fA-F]\+"
syn match eaxBin     "0b\[01\]\+"
syn region artimetricBlock start=+\[+ end=+\]+

hi link eaxSingleLineComment  Comment
hi link eaxSingleLineComment2 Comment
hi link eaxMultiLineComment   Comment
hi link eaxSpecifier          Statement
hi link eaxKeyword            Keyword
hi link eaxType               Type
hi link eaxInstruction        Function
hi link eaxInstructionLike    Function
hi link eaxRegister           Special
hi link eaxInt                Number
hi link eaxHex                Number
hi link eaxBin                Number
hi link artimetricBlock       Statement
