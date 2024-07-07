#include "assembler.h"

#include <stdlib.h>

/*
gcc -c -ansi -Wall -Wextra -Wpedantic -Werror:
	-- warns about trailing comma in enumerations.
	-- kept because of chad coding standard.
	-- unused functions for now, obviously...
clang -c -ansi -Weverything -Werror:
	-- exactly same warnings as with gcc above.
splint -fcnuse +boolint +charint assembler.c:
	-- we obviously ignore unused functions for now.
	-- we assume bool is int, which is true.
	-- we assume char is int, which is true.
	-- assembler.c:167:13: Function fabs shadows outer declaration
	-- ^ function from <math.h>, which we don't even use.
splint assembler.h:
	-- no warnings at all.
*/

#define REGULAR_BEGIN   (ADD)
#define REGULAR_END     (CMP)
#define IRREGULAR_BEGIN (INC)
#define IRREGULAR_END   (IDIV)
#define SPECIAL_1_BEGIN (NOP)
#define SPECIAL_1_END   (HLT)
#define SPECIAL_2_BEGIN (SYSENTER)
#define SPECIAL_2_END   (CPUID)
#define JUMP_IF_BEGIN   (JO)
#define JUMP_IF_END     (JG)
#define MOVE_IF_BEGIN   (CMOVO)
#define MOVE_IF_END     (CMOVG)

#define REGULAR_COUNT   (REGULAR_END   - REGULAR_BEGIN   + 1)
#define IRREGULAR_COUNT (IRREGULAR_END - IRREGULAR_BEGIN + 1)
#define SPECIAL_1_COUNT (SPECIAL_1_END - SPECIAL_1_BEGIN + 1)
#define SPECIAL_2_COUNT (SPECIAL_2_END - SPECIAL_2_BEGIN + 1)
#define JUMP_IF_COUNT   (JUMP_IF_END   - JUMP_IF_BEGIN   + 1)
#define MOVE_IF_COUNT   (MOVE_IF_END   - MOVE_IF_BEGIN   + 1)

// Regulates displacement, immediate, label, variable, constant, string data.
static next * empty_from;
static next * empty_to;
static next * imbue_with;
static next * imbue_size;

// Main function.
static void place (form when,
                   byte data) {
	/* */
	token_array [token_count] = data;

	token_count += (next) when;
}

static void print (form       when,
                   size_index size,
                   next       data) {
	/* */
	place ((when != 0) && (size >= D8),  (byte) ((data >>  0) & 0XFF));
	place ((when != 0) && (size >= D16), (byte) ((data >>  8) & 0XFF));
	place ((when != 0) && (size >= D32), (byte) ((data >> 16) & 0XFF));
	place ((when != 0) && (size >= D32), (byte) ((data >> 24) & 0XFF));
}

static form valid (form data) { return ((data >= 0) && (data <= 15)); }
static form lower (form data) { return ((data >= 0) && (data <=  7)); }
static form upper (form data) { return ((data >= 8) && (data <= 15)); }

// Important stuff that I need to handle later, it saves bytes!
static form far  (next label) { return (1); /* DO NOT CHANGE YET! */ }
static form near (next label) { return (0); /* DO NOT CHANGE YET! */ }

static void build_short_prefix (form when) {
	place (when, 0X66);
}

// 40-4D!0X02
static void build_long_prefix (form use_big_registers,
                               form use_new_destination,
                               form use_new_source) {
	/* */
	place (use_big_registers || use_new_destination || use_new_source,
	       (byte) (0X40
	             + 0X01 * use_new_destination
	             + 0X04 * use_new_source
	             + 0X08 * use_big_registers));
}

// C0-FF
static void build_register_direction (form when,
                                      next destination,
                                      next source) {
	/* */
	place (when, (byte) (0XC0
	     + 0X01 * (destination & 0X07)
	     + 0X08 * (source      & 0X07)));
}

// 05-3D
static void build_register_redirection (form when,
                                        next direction) {
	/* */
	place (when, (byte) (0X05
	     + 0X08 * (direction & 0X07)));
}

// 80/81
static void build_constant (form       when,
                            size_index size) {
	/* */
	place (when, (byte) (0X80
	     + 0X01 * (size != D8)));
}

// REGULAR_BEGIN-REGULAR_END D8-D64 REG/MEM R0-R15/MEM -||-/IMM -||-/IMM
static void build_regular (operation_index operation,
                           size_index      size,
                           type_index      to,
                           next            destination,
                           type_index      from,
                           next            source) {
	/* */
	build_short_prefix (size == D16);

	build_long_prefix (size == D64,
	                  (to   == REG) && (upper ((form) destination)),
	                  (from == REG) && (upper ((form) source)));

	build_constant (from == IMM, size);

	place (1, (byte) (0X08 * (operation - REGULAR_BEGIN)
	     + (destination & 0X07) * ((to == REG) && (from == IMM))
	     + 0X01 *  (size != D8)
	     + 0X02 * ((from == MEM) && (to == REG))
	     + 0X04 * ((from == IMM) && (to == MEM))
	     + 0XC0 * ((from == IMM) && (to == REG))));

	build_register_direction ((to == REG) && (from == REG),
	                          destination, source);

	build_register_redirection ((to == REG) && (from == MEM), destination);
	build_register_redirection ((to == MEM) && (from == REG), source);

	print ((to == REG) && (from == MEM), D32,  (next) ~0);
	print ((to == REG) && (from == IMM), size, source);
	print ((to == MEM) && (from == REG), D32,  (next) ~0);
	print ((to == MEM) && (from == IMM), D32,  (next) ~0);
	print ((to == MEM) && (from == IMM), size, source);
}

// IRREGULAR_BEGIN-IRREGULAR_END D8-D64 REG/MEM R0-R15/MEM
static void build_irregular (operation_index operation,
                             size_index      size,
                             type_index      to,
                             next            destination) {
	/* */
	build_short_prefix (size == D16);

	build_long_prefix (size == D64,
	                  (to   == REG) && (upper ((form) destination)), 0);

	place (1, (byte) (0XF6
	     + 0X08 * ((operation == INC) || (operation == DEC))
	     + 0X01 * (size       != D8)));

	place (to == REG, (byte) (0XC0
	     + 0X08 * (operation - IRREGULAR_BEGIN))
	     + 0X01 * (destination & 0X07));

	place (to == MEM, (byte) (0X05
	     + 0X08 * (operation - IRREGULAR_BEGIN)));
}

// SPECIAL_1_BEGIN-SPECIAL_1_END
static void build_special_1 (operation_index operation) {
	const byte data [1 * SPECIAL_1_COUNT] = {
		0X90, 0XC3, 0XCB, 0XC9, 0XF0, 0XF4
	};

	place (1, data [operation - SPECIAL_1_BEGIN]);
}

// SPECIAL_2_BEGIN-SPECIAL_2_END
static void build_special_2 (operation_index operation) {
	const byte data [2 * SPECIAL_2_COUNT] = {
		0X0F, 0X0F, 0X0F, 0X0F, 0XF3, 0X0F,
		0X34, 0X35, 0X05, 0X07, 0X90, 0XA2
	};

	place (1, data [operation - SPECIAL_2_BEGIN]);
	place (1, data [operation - SPECIAL_2_BEGIN + SPECIAL_2_COUNT]);
}

// JUMP_IF_BEGIN-JUMP_IF_END D8/32 IMM8/32
static void build_jump_if (operation_index operation,
                           size_index      size,
                           next            location) {
	/* */
	(void) size; /* HIGHLY DEPENDS FOR FAR AND NEAR JUMPS! */

	place (far  (location), 0X0F); /* EVERYTHING IS FAR JUMP! */
	place (far  (location), (byte) (0X80 + operation - JUMP_IF_BEGIN));
	place (near (location), (byte) (0X70 + operation - JUMP_IF_BEGIN));

	//~displacement (4, 0X12345678);
}

// MOVE_IF_BEGIN-MOVE_IF_END D16-D64 REG R0-R15 -||-/MEM -||-/MEM
static void build_move_if (operation_index operation,
                           size_index      size,
                           type_index      to,
                           next            destination,
                           type_index      from,
                           next            source) {
	/* */
	build_short_prefix (size == D16);

	build_long_prefix (size == D64,
	                  (to   == REG) && (upper ((form) destination)),
	                  (from == REG) && (upper ((form) source)));

	place (1, 0X0F);
	place (1, (byte) (0X40 + operation - MOVE_IF_BEGIN));

	build_register_direction ((to == REG) && (from == REG),
	                          destination, source);

	build_register_redirection ((to == REG) && (from == MEM), destination);

	//~displacement (4, 0X12345678); // Not implemented at this point!
}

// SPECIAL!
static void build_jump (size_index size,
                        type_index to,
                        next       destination) {
	/* */
	place ((to == REG) && upper ((form) destination), (byte) 0X41);

	place (to == REL, (byte) (0XE9 + 0X02 * (size == D8)));
	place (to == REG, (byte) 0XFF);
	place (to == REG, (byte) (0XE0 + 0X01 * (destination & 0X07)));
	place (to == MEM, (byte) 0XFF);
	place (to == MEM, (byte) 0X25);

	//~displacement (to == MEM, 4, 0X12345678); // Keep when in mind!
}

// VERY FUCKING SPECIAL!
static void build_move (size_index size,
                        type_index to,
                        next       destination,
                        type_index from,
                        next       source) {
	/* */
	build_short_prefix (size == D16);

	build_long_prefix (size == D64,
	                  (to   == REG) && (upper ((form) destination)),
	                  (from == REG) && (upper ((form) source)));

	place ((to == REG) && (from == REG), (byte) (0X88 + (size != D8)));
	place ((to == REG) && (from == MEM), (byte) (0X8A + (size != D8)));
	place ((to == MEM) && (from == REG), (byte) (0X88 + (size != D8)));

	build_register_redirection ((to == REG) && (from == MEM), destination);
	build_register_redirection ((to == MEM) && (from == REG), source);

	place ((to == REG) && (from == IMM), (byte) (0XB0
	                                           + 0X08 * (size != D8)
	                                           + 0X01 * (destination & 0X07)));

	place ((to == MEM) && (from == IMM), (byte) (0XC6 + (size != D8)));
	place ((to == MEM) && (from == IMM), (byte) (0X05));

	print ((to == REG) && (from == MEM), D32,  (next) ~0);
	print ((to == REG) && (from == IMM), size, source);
	print ((to == MEM) && (from == REG), D32,  (next) ~0);
	print ((to == MEM) && (from == IMM), D32,  (next) ~0);
	print ((to == MEM) && (from == IMM), size, source);
}

next   token_count;
byte * token_array;

void assemble (next   count,
               next * array) {
	/* */
	next index;

	for (index = 0; index < count; ++index) {
		if ((array [index] >= REGULAR_BEGIN)
		&&  (array [index] <= REGULAR_END)) {
			build_regular (array [index + 0], array [index + 1],
			               array [index + 2], array [index + 3],
			               array [index + 4], array [index + 5]);
			index += 5;
		} else if ((array [index] >= IRREGULAR_BEGIN)
		       &&  (array [index] <= IRREGULAR_END)) {
			build_irregular (array [index + 0], array [index + 1],
			                 array [index + 2], array [index + 3]);
			index += 3;
		} else if ((array [index] >= SPECIAL_1_BEGIN)
		       &&  (array [index] <= SPECIAL_1_END)) {
			build_special_1 (array [index + 0]);
			index += 0;
		} else if ((array [index] >= SPECIAL_2_BEGIN)
		       &&  (array [index] <= SPECIAL_2_END)) {
			build_special_2 (array [index + 0]);
			index += 0;
		} else if ((array [index] >= JUMP_IF_BEGIN)
		       &&  (array [index] <= JUMP_IF_END)) {
			build_jump_if (array [index + 0], array [index + 1],
			               array [index + 2]);
			index += 2;
		} else if ((array [index] >= MOVE_IF_BEGIN)
		       &&  (array [index] <= MOVE_IF_END)) {
			build_move_if (array [index + 0], array [index + 1],
			               array [index + 2], array [index + 3],
			               array [index + 4], array [index + 5]);
			index += 5;
		} else if (array [index] == JMP) {
			build_jump (array [index + 1], array [index + 2],
			            array [index + 3]);
			index += 3;
		} else if (array [index] == MOV) {
			build_move (array [index + 1], array [index + 2],
			            array [index + 3], array [index + 4],
			            array [index + 5]);
			index += 5;
		} else {
			exit (EXIT_FAILURE); // For debugging only!
		}
	}
}
