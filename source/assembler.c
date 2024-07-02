#include "assembler.h"

static void byte_push (byte data) {
	byte_array [byte_count] = data;

	byte_count += 1;
}

static void assemble_xor (size_index size,
                          type_index type,
                          form       destination,
                          form       source) {
	byte set = 0;
	byte mod = 0;

	if (size == size_256b) exit (EXIT_FAILURE);
	if (size == size_128b) exit (EXIT_FAILURE);

	if (size == size_64b) byte_push (0X48);

	switch (type) {
		case type_register_register: set += 0X30; break;
		case type_register_variable: set += 0X32; break;
		case type_register_constant: set += 0X80; break;
		case type_variable_register: set += 0X30; break;
		case type_variable_constant: set += 0X80; break;
		default: exit (EXIT_FAILURE);
	}

	if (size != size_8b) set += 1;

	if ((type == type_register_constant) && (destination == 0)) set += 4;

	byte_push (set);

	// FORMAT MOD/SIB BYTE IF NEEDED...

	byte_push (mod);

	// FORMAT MEMORY OR IMMEDIATE IF NEEDED...

	// ...
}
