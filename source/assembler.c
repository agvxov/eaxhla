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

#define REGULAR_START   (ADD)
#define REGULAR_COUNT   (UMUL - REGULAR_START)
#define IRREGULAR_START (UMUL)
#define IRREGULAR_COUNT (NOP - IRREGULAR_START)
#define SPECIAL_1_START (NOP)
#define SPECIAL_1_COUNT (SYSENTER - SPECIAL_1_START)
#define SPECIAL_2_START (SYSENTER)
#define SPECIAL_2_COUNT (ENTER - SPECIAL_2_START)

typedef signed   int  form;
typedef unsigned int  next;
typedef unsigned char byte;

static next   input_count; /* SHOULD I EXPOSE THESE? */
static form * input_array;
static next   output_count;
static byte * output_array;

static void place (form when, byte data) {
	output_array [output_count] = data;

	output_count += (next) when;
}

static form valid (form data) { return ((data >= 0) && (data <= 15)); }
static form lower (form data) { return ((data >= 0) && (data <=  7)); }
static form upper (form data) { return ((data >= 8) && (data <= 15)); }

static void build_short_prefix (form when) {
	place (when, 0X66);
}

static void build_long_prefix (form       when,
                               size_index size,
                               type_index to,
                               form       destination,
                               type_index from,
                               form       source) {
	place (when, (byte) (0X40
	      + 0X01 * ((to   == REG) && (upper (destination)))
	      + 0X04 * ((from == REG) && (upper (source)))
	      + 0X08 *  (size == D64)));
}

static void build_register_direction (form when,
                                      form destination,
                                      form source) { /* LENGTH */
	place (when, (byte) (0XC0 + 0X01 * (destination % 8) + 0X08 * (source % 8)));
}

static void build_register_redirection (form when,
                                        form direction) {
	place (when, (byte) (0X05 + 0X08 * (direction % 8)));
}

static void build_constant (form       when,
                            size_index size) {
	place (when, (byte) (0X80 + 0X01 * (size != D8)));
}

static void build_regular_instruction (form       when,
                                       byte       code,
                                       size_index size,
                                       type_index to,
                                       form       destination,
                                       type_index from) {
	place (when, (byte) (code
	      + destination % 8 * ((to == REG) && (from == IMM))
	      + 0X01 *  (size != D8)
	      + 0X02 * ((from == MEM) && (to == REG))
	      + 0X04 * ((from == IMM) && (to == MEM))
	      + 0XC0 * ((from == IMM) && (to == REG))));
}

static void build_regular (operation_index operation,
                           size_index      size,
                           type_index      to,
                           form            destination,
                           type_index      from,
                           form            source) {
	build_short_prefix (size == D16);

	build_long_prefix ((size == D64)
	               || ((to   == REG) && (upper (destination)))
	               || ((from == REG) && (upper (source))),
	                   size, to, destination, from, source);

	build_constant (from == IMM, size);

	build_regular_instruction (1, (byte) (0X08 * operation),
	                           size, to, destination, from);

	build_register_direction ((to == REG) && (from == REG),
	                          destination, source);

	build_register_redirection ((to == REG) && (from == MEM), destination);
	build_register_redirection ((to == MEM) && (from == REG), source);
}

static void build_special_1 (operation_index operation) {
	const byte data [1 * SPECIAL_1_COUNT] = {
		0X90, 0XC3, 0XCB, 0XC9, 0XF0, 0XF4
	};

	place (1, data [operation - SPECIAL_1_START]);
}

static void build_special_2 (operation_index operation) {
	const byte data [2 * SPECIAL_2_COUNT] = {
		0X0F, 0X0F, 0X0F, 0X0F, 0XF3, 0X0F,
		0X34, 0X35, 0X05, 0X07, 0X90, 0XA2
	};

	place (1, data [operation - SPECIAL_2_START]);
	place (1, data [operation - SPECIAL_2_START + SPECIAL_2_COUNT]);
}
