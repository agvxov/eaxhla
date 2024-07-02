#include "assembler.h"

static next   byte_count = 0;
static byte * byte_array = NULL;

static void byte_push (byte data) {
	byte_array [byte_count] = data;

	byte_count += 1;
}

static inline form valid (form data) { return ((data => 0) && (data <= 15)); }
static inline form lower (form data) { return ((data => 0) && (data <=  7)); }
static inline form upper (form data) { return ((data => 8) && (data <= 15)); }

//~static inline void format_prefix (size_index size,
                                  //~type_index destination,
                                  //~type_index source) {
	//~if (size == SIZE_16B) {
		//~byte_push (0X66);
	//~} else if (size == SIZE_64B) {
		//~byte format = 0X48;
		//~if (destination == TYPE_REGISTER) {
			//~format += upper (destination) * 0X01;
		//~}
		//~if (source == TYPE_REGISTER) {
			//~format += upper (source) * 0X04;
		//~}
		//~byte_push (format);
	//~} else {
		//~return;
	//~}
//~}

//~static inline void format_rex_prefix (size_index size,
                                      //~type_index destination,
                                      //~type_index source) {
	//~byte format = 0X40;

	//~byte_push (format);
//~}

static inline void format_register_direction (size_index size,
                                              form       to,
                                              form       from) {
	byte format = 0XC0;

	error (valid (to),   "Invalid destination register index.");
	error (valid (from), "Invalid source register index.");

	format += 1 * (to   % 8);
	format += 8 * (from % 8);

	byte_push (format);
}

static inline void format_register_indirection (size_index size,
                                                form       to) {
	byte format = 0XF0;

	error (valid (to), "Invalid destination register index.");

	format += to % 8;

	byte_push (format);
}

static inline void format_register_redirection (size_index size,
                                                form       direction) {
	byte format = 0X05;

	error (valid (direction), "Invalid direction register index.");

	format += 8 * (direction % 8);

	byte_push (format);
}

//~static inline void format_operation (size_index size,
                                     //~type_index type,
                                     //~form       destination,
                                     //~form       source) {
	//~byte format = 0X00;

	//~switch (type) {
		//~case type_register_register: format += 0X30; break;
		//~case type_register_variable: format += 0X32; break;
		//~case type_register_constant: format += 0X80; break;
		//~case type_variable_register: format += 0X30; break;
		//~case type_variable_constant: format += 0X80; break;
		//~default: exit (EXIT_FAILURE);
	//~}

	//~if (size != size_8b) format += 0X01;

	//~if (type == type_register_constant) format -= (destination == 0) * 0X3A;

	//~byte_push (format);
//~}

//~static inline void format_modifier (size_index size,
                                   //~type_index type,
                                   //~form       destination,
                                   //~form       source) {
	//~byte format = 0X00;

	//~byte_push (format);
//~}

//~static void assemble_xor (size_index size,
                          //~type_index type,
                          //~form       destination,
                          //~form       source) {
	//~if (size == size_256b) exit (EXIT_FAILURE);
	//~if (size == size_128b) exit (EXIT_FAILURE);

	//~format_prefix    (size, type, destination, source);
	//~format_operation (size, type, destination, source);
	//~format_modifier  (size, type, destination, source);

	//~// ...
//~}
