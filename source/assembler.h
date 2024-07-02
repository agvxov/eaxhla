#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdlib.h>

typedef enum {
	SIZE_256B,              SIZE_128B,              SIZE_64B,
	SIZE_32B,               SIZE_16B,               SIZE_8B,
} size_index;

typedef enum {
	TYPE_REGISTER,          TYPE_VARIABLE,          TYPE_CONSTANT,
} type_index;

typedef enum {
	OPERATION_MOVE,         OPERATION_ADD,          OPERATION_SUBTRACT,
	OPERATION_MULTIPLY,     OPERATION_DIVIDE,       OPERATION_MODULUS,
	OPERATION_COMPARE,      OPERATION_JUMP,         OPERATION_XOR,
	OPERATION_AND,          OPERATION_OR,           OPERATION_NOT,
	OPERATION_IS,           OPERATION_ABOVE,        OPERATION_BELOW,
	OPERATION_IF,           OPERATION_INCREMENT,    OPERATION_DECREMENT,
	OPERATION_SYSTEM,       OPERATION_CALL,         OPERATION_RETURN,
	OPERATION_ENTER,        OPERATION_LEAVE,        OPERATION_EXIT,
} operation_index;

typedef enum {
	OPERAND_REGISTER_0,     OPERAND_REGISTER_1,     OPERAND_REGISTER_2,
	OPERAND_REGISTER_3,     OPERAND_REGISTER_4,     OPERAND_REGISTER_5,
	OPERAND_REGISTER_6,     OPERAND_REGISTER_7,     OPERAND_REGISTER_8,
	OPERAND_REGISTER_9,     OPERAND_REGISTER_A,     OPERAND_REGISTER_B,
	OPERAND_REGISTER_C,     OPERAND_REGISTER_D,     OPERAND_REGISTER_E,
	OPERAND_REGISTER_F,     OPERAND_REFERENCE,      OPERAND_DEREFERENCE,
} operand_index;

typedef signed   int  form;
typedef unsigned int  next;
typedef unsigned char byte;

// size, operation, type, operand/form, type, operand/form

#endif
