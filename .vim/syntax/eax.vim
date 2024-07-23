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

syn iskeyword @,48-57,_

syn region eaxSingleLineComment  start=+//+ end=+\n+
syn region eaxSingleLineComment2 start=+#+ end=+\n+
syn region eaxMultiLineComment  start=+\/\*+ end=+\*\/+
syn keyword eaxSpecifier fast unix in
syn keyword eaxKeyword program machine procedure begin until repeat break if then else end
syn keyword eaxType u8 u16 u32 u64 s8 s16 s32 s64
syn keyword eaxInstruction inc xor mov
syn keyword eaxInstructionLike fastcall exit
syn keyword eaxRegister
    \ rax rcx rdx rbx rsp rbp rsi rdi rg8 rg9 rg10 rg11 rg12 rg13 rg14 rg15
    \ eax ecx edx ebx esp ebp esi edi rg8d rg9d rg10d rg11d rg12d rg13d rg14d rg15d
    \ ax cx dx bx sp bp si di r8w r9w r10w r11w r12w r13w r14w r15w
    \ al cl dl bl spl bpl sil dil r8b r9b r10b r11b r12b r13b r14b r15b
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
