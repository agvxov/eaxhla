#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdlib.h>

typedef enum {
	size_256b,              size_128b,              size_64b,
	size_32b,               size_16b,               size_8b
} size_index;

typedef enum {
	type_register_register, type_register_variable, type_register_constant,
	type_variable_register, type_variable_constant
} type_index;

typedef enum {
	operation_move,         operation_add,          operation_subtract,
	operation_multiply,     operation_divide,       operation_modulus,
	operation_compare,      operation_jump,         operation_xor,
	operation_and,          operation_or,           operation_not,
	operation_is,           operation_above,        operation_below,
	operation_if,           operation_increment,    operation_decrement,
	operation_system,       operation_call,         operation_return,
	operation_enter,        operation_leave,        operation_exit
} operation_index;

typedef enum {
	operand_rax,            operand_rcx,            operand_rdx,
	operand_rbx,            operand_rsi,            operand_rdi,
	operand_rsp,            operand_rbp,            operand_r8,
	operand_r9,             operand_r10,            operand_r11,
	operand_r12,            operand_r13,            operand_r14,
	operand_r15
} operand_64b_index;

typedef enum {
	operand_eax,            operand_ecx,            operand_edx,
	operand_ebx,            operand_esi,            operand_edi,
	operand_esp,            operand_ebp,            operand_r8d,
	operand_r9d,            operand_r10d,           operand_r11d,
	operand_r12d,           operand_r13d,           operand_r14d,
	operand_r15d
} operand_32b_index;

typedef enum {
	operand_ax,             operand_cx,             operand_dx,
	operand_bx,             operand_si,             operand_di,
	operand_sp,             operand_bp,             operand_r8w,
	operand_r9w,            operand_r10w,           operand_r11w,
	operand_r12w,           operand_r13w,           operand_r14w,
	operand_r15w
} operand_16b_index;

typedef enum {
	operand_al,             operand_cl,             operand_dl,
	operand_bl,             operand_sil,            operand_dil,
	operand_spl,            operand_bpl,            operand_r8b,
	operand_r9b,            operand_r10b,           operand_r11b,
	operand_r12b,           operand_r13b,           operand_r14b,
	operand_r15b
} operand_8b_index;

typedef enum {
	command_operation,      command_size,           command_operand_d,
	command_operand_s
	command_length
} command_index;

typedef unsigned int  data;
typedef unsigned char byte;

static data   byte_count = 0;
static byte * byte_array = NULL;

static void assemble_xor (size_index size,
                          type_index type,
                          size_t     destination,
                          size_t     source);

#endif
