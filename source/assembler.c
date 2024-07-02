#include "assembler.h"

static next   byte_count = 0;
static byte * byte_array = NULL;

static void byte_push (byte data) {
	byte_array [byte_count] = data;

	byte_count += 1;
}

static form lower (form data) { return ((data => 0) && (data <=  7)); }
static form upper (form data) { return ((data => 8) && (data <= 15)); }

static void format_prefix (size_index size,
                           type_index type,
                           form       destination,
                           form       source) {
	byte format = 0X00;

	if (size == size_16b) byte_push (0X66);

	if (size == size_64b) format += 0X48;
	if (size == size_32b) format += 0X48;
	if (size == size_64b) format += 0X48;
	if (size == size_64b) format += 0X48;

	if (upper (destination)) format += 0X01;
	if (upper (source))      format += 0X04;

	byte_push (format);
}

static void format_operation (size_index size,
                              type_index type,
                              form       destination,
                              form       source) {
	byte format = 0X00;

	switch (type) {
		case type_register_register: format += 0X30; break;
		case type_register_variable: format += 0X32; break;
		case type_register_constant: format += 0X80; break;
		case type_variable_register: format += 0X30; break;
		case type_variable_constant: format += 0X80; break;
		default: exit (EXIT_FAILURE);
	}

	if (size != size_8b) format += 0X01;

	if (type == type_register_constant) format -= (destination == 0) * 0X3A;

	byte_push (format);
}

static void format_modifier (size_index size,
                             type_index type,
                             form       destination,
                             form       source) {
	byte format = 0X00;

	byte_push (format);
}

static void assemble_xor (size_index size,
                          type_index type,
                          form       destination,
                          form       source) {
	if (size == size_256b) exit (EXIT_FAILURE);
	if (size == size_128b) exit (EXIT_FAILURE);

	format_prefix    (size, type, destination, source);
	format_operation (size, type, destination, source);
	format_modifier  (size, type, destination, source);

	// ...
}
