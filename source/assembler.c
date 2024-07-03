#include "assembler.h"

typedef signed   int  form;
typedef unsigned int  next;
typedef unsigned char byte;

static next   token_count = 0;
static byte * token_array = NULL;

static void token_print (byte data) {
	token_array [token_count] = data;

	token_count += 1;
}

static inline form valid (form data) { return ((data >= 0) && (data <= 15)); }
static inline form lower (form data) { return ((data >= 0) && (data <=  7)); }
static inline form upper (form data) { return ((data >= 8) && (data <= 15)); }

static inline
void format_short_prefix (void) {
	token_print (0X66);
}

static inline
void format_long_prefix (size_index size,
                         type_index to,
                         form       destination,
                         type_index from,
                         form       source) {
	token_print (0X40                                            +
	             0X01 * ((to   == REG) && (upper (destination))) +
	             0X04 * ((from == REG) && (upper (source)))      +
	             0X08 * (size == D64));
}

static inline
void format_register_direction (form destination,
                                form source) {
	token_print (0XC0 + 0X01 * (destination % 8) + 0X08 * (source % 8));
}

static inline
void format_register_redirection (form direction) {
	token_print (0X05 + 0X08 * (direction % 8));
}

static inline
void format_constant (size_index size) {
	token_print (0X80 + 0X01 * (size != D8));
}

static inline
void format_regular_instruction (byte       format,
                                 size_index size,
                                 type_index to,
                                 form       destination,
                                 type_index from) {
	token_print (format +
	             destination % 8 * ((to == REG) && (from == IMM)) +
	             0X01 * (size != D8)                              +
	             0X02 * ((to == REG) && (from == MEM))            +
	             0X04 * ((to == MEM) && (from == IMM))            +
	             0XC0 * ((to == REG) && (from == IMM)));
}

static inline
void assemble_enter (form dynamic_storage,
                     form nesting_level) {
	token_print (0XC8);
	token_print ((byte) (dynamic_storage /   1) % 256); // FIX LATER
	token_print ((byte) (dynamic_storage / 256) % 256);
	token_print ((byte) (nesting_level   /   1) % 256);
}

static inline void assemble_leave          (void) { token_print (0XC9); }
static inline void assemble_system_call    (void) { token_print (0X0F); token_print (0X05); } // FIX LATER
static inline void assemble_system_return  (void) { token_print (0X0F); token_print (0X07); }
static inline void assemble_system_enter   (void) { token_print (0X0F); token_print (0X34); }
static inline void assemble_system_exit    (void) { token_print (0X0F); token_print (0X35); }
static inline void assemble_cpu_identifier (void) { token_print (0X0F); token_print (0XA2); }

static inline
void assemble (operation_index operation,
               size_index      size,
               type_index      to,
               form            destination,
               type_index      from,
               form            source) {
	byte code = 0X00;

	if (size == D16) {
		format_short_prefix ();
	}

	if ((size == D64)
	|| ((to   == REG) && (upper (destination)))
	|| ((from == REG) && (upper (source)))) {
		format_long_prefix (size, to, destination, from, source);
	}

	if (from == IMM) {
		format_constant (size);
	}

	switch (operation) {
		case ADD: code = 0X00; break;
		case OR:  code = 0X08; break;
		case ADC: code = 0X10; break;
		case SBB: code = 0X18; break;
		case AND: code = 0X20; break;
		case SUB: code = 0X28; break;
		case XOR: code = 0X30; break;
		case CMP: code = 0X38; break;
		default:               break;
	}

	format_regular_instruction (code, size, to, destination, from);

	if ((to == REG) && (from == REG)) {
		format_register_direction (destination, source);
	}

	if ((to == REG) && (from == MEM)) {
		format_register_redirection (destination);
	}

	if ((to == MEM) && (from == REG)) {
		format_register_redirection (source);
	}
}
