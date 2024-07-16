#include "assembler.h"

#include <stdlib.h>
#include <string.h>
#include "debug.h"

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
#define TESTING_FOR_NOW (4)
static next empty_count                   = 0;
static next empty_holes                   = 0;
static next empty_array [TESTING_FOR_NOW] = { 0 };
static next empty_imbue [TESTING_FOR_NOW] = { 0 };
static next empty_store [TESTING_FOR_NOW] = { 0 };

// Main function.
static void place (form when,
                   byte data) {
	/* */
	text_sector_byte [text_sector_size] = data;

	text_sector_size += (next) when;
}

static void print (form       when,
                   size_index size,
                   next       data) {
	/* */
	place ((when != 0) && (size >= D8),  (byte) ((data >>  0) & 0xff));
	place ((when != 0) && (size >= D16), (byte) ((data >>  8) & 0xff));
	place ((when != 0) && (size >= D32), (byte) ((data >> 16) & 0xff));
	place ((when != 0) && (size >= D32), (byte) ((data >> 24) & 0xff));
	/* 64-BIT SUPPORT */
}

static void asmdirrel (form when,
                       next data) {
	/* */
	if (empty_holes >= TESTING_FOR_NOW) exit (69);

	empty_array [empty_holes] = text_sector_size;
	empty_imbue [empty_holes] = data;

	empty_holes += (next) when;
}

static void asmdirmem (form when,
                       next code) {
	/* */
	if (code >= TESTING_FOR_NOW) exit (69);

	empty_store [code] = text_sector_size;

	empty_count += (next) when;
}

static void delay (form       when,
                   size_index size,
                   next       data) {
	/* */
	asmdirrel (when, data);

	place ((when != 0) && (size >= D8),  (byte) 0xb0);
	place ((when != 0) && (size >= D16), (byte) 0x10);
	place ((when != 0) && (size >= D32), (byte) 0x40);
	place ((when != 0) && (size >= D32), (byte) 0x00);
	/* NO DEREFERENCING */
	/* 64-BIT SUPPORT */
}

static void asmdirimm (form       when,
                       size_index size,
                       next       data) {
	print (when, size, data);
}

static form front (form data) { return ((data >= 4) && (data <=  7)); }
static form lower (form data) { return ((data >= 0) && (data <=  7)); }
static form upper (form data) { return ((data >= 8) && (data <= 15)); }

// Important stuff that I need to handle later, it saves bytes!
static form far  (next label) { return (label && 1); }
static form near (next label) { return (label && 0); }

static void build_short_prefix (form when) {
	place (when, 0x66);
}

// 40-4D!0X02
static void build_long_prefix (form use_big_registers,
                               form use_new_destination,
                               form use_new_source) {
	/* */
	place (use_big_registers || use_new_destination || use_new_source,
	       (byte) (0x40
	             + 0x01 * use_new_destination
	             + 0x04 * use_new_source
	             + 0x08 * use_big_registers));
}

// C0-FF
static void build_register_direction (form when,
                                      next destination,
                                      next source) {
	/* */
	place (when, (byte) (0xc0
	     + 0x01 * (destination & 0x07)
	     + 0x08 * (source      & 0x07)));
}

// 05-3D
static void build_register_redirection (form when,
                                        next direction) {
	/* */
	place (when, (byte) (0x05
	     + 0x08 * (direction & 0x07)));
}

// 80/81
static void build_constant (form       when,
                            size_index size) {
	/* */
	place (when, (byte) (0x80
	     + 0x01 * (size != D8)));
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

	// 40>front
	place ((size == D8) && (to == REG) && ((from == REG) || (from == IMM))
	      && (((front ((form) destination) && lower ((form) source))
	      ||   (lower ((form) destination) && front ((form) source))) ||
	      ((to == REG) && (from == IMM) && front ((form) destination))),
	      (byte) 0x40);

	place ((from == IMM) && (to == REG) && (destination == 0),
	      (byte) (0x05
	     + 0x08 * (operation & 0x07))
	     - 0x01 * (size == D8));

	build_constant ((from == IMM) && ! ((to == REG) && (destination == 0)), size);

	place (! ((from == IMM) && (to == REG) && (destination == 0)),
	     (byte) (0x08 * (operation - REGULAR_BEGIN)
	     + (destination & 0x07) * ((to == REG) && (from == IMM))
	     + 0x01 * ((to   == MEM) && (from == IMM) && (size == D8)) //
	     - 0x01 * ((to   == REG) && (from == IMM) && (size != D8)) //
	     + 0x01 *  (size != D8)
	     + 0x02 * ((to   == REG) && (from == MEM))
	     + 0x04 * ((to   == MEM) && (from == IMM))
	     + 0xc0 * ((to   == REG) && (from == IMM))));

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

	// 40>front
	place ((size == D8) && (to == REG) && front ((form) destination), (byte) 0x40);

	place (1, (byte) (0xf7
	     + 0x08 * ((operation == INC) || (operation == DEC))
	     - 0x01 * (size       == D8)));

	place (to == REG, (byte) (0xc0
	     + 0x08 * (operation - IRREGULAR_BEGIN))
	     + 0x01 * (destination & 0x07));

	place (to == MEM, (byte) (0x05
	     + 0x08 * (operation - IRREGULAR_BEGIN)));
}

// SPECIAL_1_BEGIN-SPECIAL_1_END
static void build_special_1 (operation_index operation) {
	const byte data [1 * SPECIAL_1_COUNT] = {
		0x90, 0xc3, 0xcb, 0xc9, 0xf0, 0xf4
	};

	place (1, data [operation - SPECIAL_1_BEGIN]);
}

// SPECIAL_2_BEGIN-SPECIAL_2_END
static void build_special_2 (operation_index operation) {
	const byte data [2 * SPECIAL_2_COUNT] = {
		0x0f, 0x0f, 0x0f, 0x0f, 0xf3, 0x0f,
		0x34, 0x35, 0x05, 0x07, 0x90, 0xa2
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

	place (far  (location), 0x0f); /* EVERYTHING IS FAR JUMP! */
	place (far  (location), (byte) (0x80 + operation - JUMP_IF_BEGIN));
	place (near (location), (byte) (0x70 + operation - JUMP_IF_BEGIN));

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

	place (1, 0x0f);
	place (1, (byte) (0x40 + operation - MOVE_IF_BEGIN));

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

	place (to == REL, (byte) (0xe9 + 0x02 * (size == D8)));
	place (to == REG, (byte) 0xff);
	place (to == REG, (byte) (0xe0 + 0x01 * (destination & 0x07)));
	place (to == MEM, (byte) 0xff);
	place (to == MEM, (byte) 0x25);

	//~displacement (to == MEM, 4, 0x12345678); // Keep when in mind!
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

	place ((to == REG) && (from == REG), (byte) (0x88 + (size != D8)));
	place ((to == REG) && (from == MEM), (byte) (0x8a + (size != D8)));
	place ((to == MEM) && (from == REG), (byte) (0x88 + (size != D8)));

	build_register_redirection ((to == REG) && (from == MEM), destination);
	build_register_redirection ((to == MEM) && (from == REG), source);

	place ((to == REG) && ((from == IMM) || (from == REL)), (byte) (0xb8
	                                           //~+ 0x08 * (size != D8)
	                                           + 0x01 * (destination & 0x07)));

	place ((to == MEM) && (from == IMM), (byte) (0xc6 + (size != D8)));
	place ((to == MEM) && (from == IMM), (byte) (0x05));

	delay ((to == REG) && (from == MEM), D32,  source);
	print ((to == REG) && (from == IMM), size, source);
	delay ((to == MEM) && (from == REG), D32,  (next) ~0);
	delay ((to == MEM) && (from == IMM), D32,  (next) ~0);
	print ((to == MEM) && (from == IMM), size, source);
	delay ((to == REG) && (from == REL), D32,  source);
}

next   text_sector_size = 0;
byte * text_sector_byte = NULL;

int was_instruction_array_empty = 0;

void assemble (next   count,
               next * array) {
	/* */
	next index;

	if ((count == 0) || (array == NULL)) {
		was_instruction_array_empty = 1;
		return;
	}

	for (index = 0; index < count; ++index) {
		if (array [index] == ASMDIRREL) {
			asmdirrel (1, array [index + 1]);
			index += 1;
		} else if (array [index] == ASMDIRMEM) {
			asmdirmem (1, array [index + 1]);
			index += 1;
		} else if (array [index] == ASMDIRIMM) {
			next repeat;
			for (repeat = 0; repeat < array [index + 2]; ++repeat) {
				asmdirimm (1, array [index + 1],
				              array [index + 3 + repeat]);
			}
			index += 2 + array [index + 2];
		//~} else if (array [index] == ASMDIRREP) {
		} else if ((array [index] >= REGULAR_BEGIN)
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
			debug_printf ("MOV %i %i %i %i %i\n",
			              array [index + 1], array [index + 2],
			              array [index + 3], array [index + 4],
			              array [index + 5]);
			index += 5;
		} else {
			//~exit (array [index]); // For debugging only!
		}
	}

	for (index = 0; index < empty_holes; ++index) {
		next set = 0, get = empty_array [index];
		memcpy (& set, & text_sector_byte [get], sizeof (set));
		set += empty_store [empty_imbue [index]];
		memcpy (& text_sector_byte [get], & set, sizeof (set));
	}
}
