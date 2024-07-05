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

#define REGULAR_COUNT   (REGULAR_END   - REGULAR_BEGIN   + 1)
#define IRREGULAR_COUNT (IRREGULAR_END - IRREGULAR_BEGIN + 1)
#define SPECIAL_1_COUNT (SPECIAL_1_END - SPECIAL_1_BEGIN + 1)
#define SPECIAL_2_COUNT (SPECIAL_2_END - SPECIAL_2_BEGIN + 1)

static void place (form when,
                   byte data) {
	token_array [token_count] = data;

	token_count += (next) when;
}

static form valid (form data) { return ((data >= 0) && (data <= 15)); }
static form lower (form data) { return ((data >= 0) && (data <=  7)); }
static form upper (form data) { return ((data >= 8) && (data <= 15)); }

static void build_short_prefix (form when) {
	place (when, 0X66);
}

static void build_long_prefix (form use_big_registers,
                               form use_new_destination,
                               form use_new_source) {
	place (use_big_registers || use_new_destination || use_new_source,
	       (byte) (0X40
	             + 0X01 * use_new_destination
	             + 0X04 * use_new_source
	             + 0X08 * use_big_registers));
}

static void build_register_direction (form when,
                                      next destination,
                                      next source) {
	place (when, (byte) (0XC0
	     + 0X01 * (destination % 8)
	     + 0X08 * (source      % 8)));
}

static void build_register_redirection (form when,
                                        next direction) {
	place (when, (byte) (0X05
	     + 0X08 * (direction % 8)));
}

static void build_constant (form       when,
                            size_index size) {
	place (when, (byte) (0X80
	     + 0X01 * (size != D8)));
}

static void build_regular (operation_index operation,
                           size_index      size,
                           type_index      to,
                           next            destination,
                           type_index      from,
                           next            source) {
	build_short_prefix (size == D16);

	build_long_prefix (size == D64,
	                  (to   == REG) && (upper (destination)),
	                  (from == REG) && (upper (source)));

	build_constant (from == IMM, size);

	place (1, (byte) (0X08 * (operation - REGULAR_BEGIN)
	     + destination % 8 * ((to == REG) && (from == IMM))
	     + 0X01 *  (size != D8)
	     + 0X02 * ((from == MEM) && (to == REG))
	     + 0X04 * ((from == IMM) && (to == MEM))
	     + 0XC0 * ((from == IMM) && (to == REG))));

	build_register_direction ((to == REG) && (from == REG),
	                          destination, source);

	build_register_redirection ((to == REG) && (from == MEM), destination);
	build_register_redirection ((to == MEM) && (from == REG), source);
}

static void build_irregular (operation_index operation,
                             size_index      size,
                             type_index      to,
                             next            destination) {
	build_short_prefix (size == D16);

	build_long_prefix (size == D64,
	                  (to == REG) && (upper (destination)), 0);

	place (1, (byte) (0XF6
	     + 0X08 * ((operation == INC) || (operation == DEC))
	     + 0X01 * (size != D8)));

	place (to == REG, (byte) (0XC0
	     + 0X08 * (operation - IRREGULAR_BEGIN))
	     + 0X01 * (destination % 8));

	place (to == MEM, (byte) (0X05
	     + 0X08 * (operation - IRREGULAR_BEGIN)));
}

static void build_special_1 (operation_index operation) {
	const byte data [1 * SPECIAL_1_COUNT] = {
		0X90, 0XC3, 0XCB, 0XC9, 0XF0, 0XF4
	};

	place (1, data [operation - SPECIAL_1_BEGIN]);
}

static void build_special_2 (operation_index operation) {
	const byte data [2 * SPECIAL_2_COUNT] = {
		0X0F, 0X0F, 0X0F, 0X0F, 0XF3, 0X0F,
		0X34, 0X35, 0X05, 0X07, 0X90, 0XA2
	};

	place (1, data [operation - SPECIAL_2_BEGIN]);
	place (1, data [operation - SPECIAL_2_BEGIN + SPECIAL_2_COUNT]);
}

next   token_count;
byte * token_array;

void assemble (next count, next * array) {
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
		}
	}
}
