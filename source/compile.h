#ifndef COMPILE_H
#define COMPILE_H

extern unsigned int * token_array;
extern unsigned int   token_count;

extern char * output_file_name;

extern int compile(void);

extern void append_instruction_t1 (int t1);
extern void append_instruction_t4 (int t4, int w, int d, int r);
extern void append_instruction_t6 (int t6, int w, int d, int r, int s, int i);

extern void append_label (int rel);

extern void append_fastcall_begin     (int rel);
extern void append_fastcall_end       (void);
extern void append_fastcall_arguments (int rel, int wid, int imm);

#endif
