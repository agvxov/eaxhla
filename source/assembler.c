#include "assembler.h"

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
	token_print (0X40                                                      +
	             0X01 * ((to   == TYPE_REGISTER) && (upper (destination))) +
	             0X04 * ((from == TYPE_REGISTER) && (upper (source)))      +
	             0X08 * (size == SIZE_64B));
}

static inline
void format_register_direction (size_index size,
                                form       destination,
                                form       source) {
	token_print (0XC0                     +
	             0X01 * (destination % 8) +
	             0X08 * (source      % 8));
}
/*
static inline
void format_register_indirection (size_index size,
                                  form       destination) {
	byte format = 0XF0;

	format += destination % 8;

	token_print (format);
}
*/
static inline
void format_register_redirection (size_index size,
                                  form       direction) {
	token_print (0X05 +
	             0X08 * (direction % 8));
}

static inline
void format_constant (size_index size) {
	token_print (0X80 +
	             0X01 * (size != SIZE_8B));
}

static inline
void format_regular_instruction (byte       format,
                                 size_index size,
                                 type_index to,
                                 form       destination,
                                 type_index from,
                                 form       source) {
	//~format += 0X01 * (size != SIZE_8B);
	//~format += 0X02 * ((to == TYPE_REGISTER) && (from == TYPE_VARIABLE));
	//~format += 0XC0 * ((to == TYPE_REGISTER) && (from == TYPE_CONSTANT));
	//~format += 0X04 * ((to == TYPE_VARIABLE) && (from == TYPE_CONSTANT));

	//~if ((to == TYPE_REGISTER) && (from == TYPE_CONSTANT)) {
		//~format += destination % 8;
	//~}

	//~token_print (format);

	token_print (format +
	             destination % 8 * ((to == TYPE_REGISTER) && (from == TYPE_CONSTANT)) +
	             0X01 * (size != SIZE_8B)                                  +
	             0X02 * ((to == TYPE_REGISTER) && (from == TYPE_VARIABLE)) +
	             0XC0 * ((to == TYPE_REGISTER) && (from == TYPE_CONSTANT)) +
	             0X04 * ((to == TYPE_VARIABLE) && (from == TYPE_CONSTANT)));
}

static inline
void assemble_enter (form dynamic_storage,
                     form nesting_level) {
	token_print (0XC8);
	token_print ((dynamic_storage /   1) % 256); // FIX LATER
	token_print ((dynamic_storage / 256) % 256);
	token_print ((nesting_level   /   1) % 256);
}

static inline void assemble_leave          (void) { token_print (0XC9); }
static inline void assemble_system_call    (void) { token_print (0X0F); token_print (0X05); }
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

	if (size == SIZE_16B) {
		format_short_prefix ();
	}

	if ((size == SIZE_64B)
	|| ((to   == TYPE_REGISTER) && (upper (destination)))
	|| ((from == TYPE_REGISTER) && (upper (source)))) {
		format_long_prefix (size, to, destination, from, source);
	}

	if (from == TYPE_CONSTANT) {
		format_constant (size);
	}

	switch (operation) {
		case OPERATION_ADD:        code = 0X00; break;
		case OPERATION_OR:         code = 0X08; break;
		case OPERATION_ADD_F:      code = 0X10; break;
		case OPERATION_SUBTRACT_F: code = 0X18; break;
		case OPERATION_AND:        code = 0X20; break;
		case OPERATION_SUBTRACT:   code = 0X28; break;
		case OPERATION_XOR:        code = 0X30; break;
		case OPERATION_COMPARE:    code = 0X38; break;
		default:                                break;
	}

	format_regular_instruction (code, size, to, destination, from, source);

	if ((to == TYPE_REGISTER) && (from == TYPE_REGISTER)) {
		format_register_direction (size, destination, source);
	}

	if ((to == TYPE_REGISTER) && (from == TYPE_VARIABLE)) {
		format_register_redirection (size, destination);
	}

	if ((to == TYPE_VARIABLE) && (from == TYPE_REGISTER)) {
		format_register_redirection (size, source);
	}
}
