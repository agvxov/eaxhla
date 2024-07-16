#include "assembler.h"

#include <stdlib.h>
#include <string.h>

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

#define REGULAR_COUNT   (REGULAR_END   - REGULAR_BEGIN   + 1) // 16
#define IRREGULAR_COUNT (IRREGULAR_END - IRREGULAR_BEGIN + 1) // 16
#define SPECIAL_1_COUNT (SPECIAL_1_END - SPECIAL_1_BEGIN + 1) // 6
#define SPECIAL_2_COUNT (SPECIAL_2_END - SPECIAL_2_BEGIN + 1) // 6
#define JUMP_IF_COUNT   (JUMP_IF_END   - JUMP_IF_BEGIN   + 1) // 16
#define MOVE_IF_COUNT   (MOVE_IF_END   - MOVE_IF_BEGIN   + 1) // 16

static int assemble_clean_up_queued = 0;

static next   empty_count = 0;
static next   empty_holes = 0;
static next * empty_array = NULL;
static next * empty_imbue = NULL;
static next * empty_store = NULL;

static void input (form when,
                   byte data) {
	// Input one byte to text sector.
	text_sector_byte [text_sector_size] = data;

	text_sector_size += (next) when;
}

static void input_by (form       when,
                      size_index size,
                      next       data) {
	// Input size-dependent bytes to text sector.
	input ((when != 0) && (size >= D8),  (byte) ((data >>  0) & 0xff));
	input ((when != 0) && (size >= D16), (byte) ((data >>  8) & 0xff));
	input ((when != 0) && (size >= D32), (byte) ((data >> 16) & 0xff));
	input ((when != 0) && (size >= D32), (byte) ((data >> 24) & 0xff));
	// 64-BIT SUPPORT
}

static void asmdirrel (form when,
                       next data) {
	// Assembler directive, queue relative memory address.
	empty_array [empty_holes] = text_sector_size;
	empty_imbue [empty_holes] = data;

	empty_holes += (next) when;
}

static void asmdirmem (form when,
                       next code) {
	// Assembler directive, queue memory address.
	empty_store [code] = text_sector_size;

	empty_count += (next) when;
}

static void asmdirimm (form       when,
                       size_index size,
                       next       data) {
	// Assembler directive, queue immediate value at current size.
	input_by (when, size, data);
}

static void input_at (form       when,
                      size_index size,
                      next       data) {
	// Input relative memory address to text sector.
	asmdirrel (when, data);

	input ((when != 0) && (size >= D8),  (byte) 0xb0);
	input ((when != 0) && (size >= D16), (byte) 0x10);
	input ((when != 0) && (size >= D32), (byte) 0x40);
	input ((when != 0) && (size >= D32), (byte) 0x00);
	// NO DEREFERENCING
	// 64-BIT SUPPORT
}

static form front (form data) { return ((data >= 4) && (data <=  7)); }
static form lower (form data) { return ((data >= 0) && (data <=  7)); }
static form upper (form data) { return ((data >= 8) && (data <= 15)); }

static form far  (next label) { return (label && 1); } // Unused for now.
static form near (next label) { return (label && 0); } // Unused for now.

static void build_short_prefix (form when) {
	// 66
	input (when, 0x66);
}

static void build_long_prefix (form big_registers,
                               form new_destination,
                               form new_source) {
	// 40-4D!0X02
	input (big_registers || new_destination || new_source,
	       (byte) (0x40
	             + 0x01 * new_destination
	             + 0x04 * new_source
	             + 0x08 * big_registers));
}

static void build_register_direction (form when,
                                      next destination,
                                      next source) {
	// C0-FF
	input (when,
	       (byte) (0xc0
	             + 0x01 * (destination & 0x07)
	             + 0x08 * (source      & 0x07)));
}

static void build_register_redirection (form when,
                                        next direction) {
	// 05-3D
	input (when,
	       (byte) (0x05
	             + 0x08 * (direction & 0x07)));
}

static void build_constant (form       when,
                            size_index size) {
	// 80/81
	input (when,
	       (byte) (0x80
	             + 0x01 * (size != D8)));
}

static void build_regular (operation_index operation,
                           size_index      size,
                           type_index      to,
                           next            destination,
                           type_index      from,
                           next            source) {
	// 00-3F : add, or, adc, sbb, and, sub, xor, cmp
	build_short_prefix (size == D16);

	build_long_prefix (size == D64,
	                   (to   == REG) && (upper ((form) destination)),
	                   (from == REG) && (upper ((form) source)));

	input ((size == D8)
	       && (to == REG)
	       && ((from == REG) || (from == IMM))
	       && (((front ((form) destination) && lower ((form) source))     //
	       ||   (lower ((form) destination) && front ((form) source)))    //
	       || ((to == REG) && (from == IMM) && front ((form) destination))),
	       (byte) 0x40);

	input ((from == IMM) && (to == REG) && (destination == 0),
	       (byte) (0x05
	             + 0x08 * (operation & 0x07))
	             - 0x01 * (size == D8));

	build_constant ((from == IMM) && ! ((to == REG) && (destination == 0)),
	                size);

	input (! ((from == IMM) && (to == REG) && (destination == 0)),
	       (byte) (0x08 * (operation - REGULAR_BEGIN)
	             + 0x01 * ((to   == MEM) && (from == IMM) && (size == D8))
	             - 0x01 * ((to   == REG) && (from == IMM) && (size != D8))
	             + 0x01 *  (size != D8)
	             + 0x02 * ((to   == REG) && (from == MEM))
	             + 0x04 * ((to   == MEM) && (from == IMM))
	             + 0xc0 * ((to   == REG) && (from == IMM)))
	             + (destination & 0x07) * ((to == REG) && (from == IMM)));

	build_register_direction ((to == REG) && (from == REG),
	                          destination, source);

	build_register_redirection ((to == REG) && (from == MEM), destination);
	build_register_redirection ((to == MEM) && (from == REG), source);

	input_by ((to == REG) && (from == MEM), D32,  (next) ~0);
	input_by ((to == REG) && (from == IMM), size, source);
	input_by ((to == MEM) && (from == REG), D32,  (next) ~0);
	input_by ((to == MEM) && (from == IMM), D32,  (next) ~0);
	input_by ((to == MEM) && (from == IMM), size, source);
}

static void build_irregular (operation_index operation,
                             size_index      size,
                             type_index      to,
                             next            destination) {
	// F0-FF : inc, dec, not, neg, mul, imul, div, idiv
	build_short_prefix (size == D16);

	build_long_prefix (size == D64,
	                   (to == REG) && (upper ((form) destination)), 0);

	input ((size == D8) && (to == REG) && front ((form) destination),
	       (byte) 0x40);

	input (1,
	       (byte) (0xf7
	             + 0x08 * ((operation == INC) || (operation == DEC))
	             - 0x01 * (size       == D8)));

	input (to == REG,
	       (byte) (0xc0
	             + 0x08 * (operation - IRREGULAR_BEGIN))
	             + 0x01 * (destination & 0x07));

	input (to == MEM,
	       (byte) (0x05
	             + 0x08 * (operation - IRREGULAR_BEGIN)));
}

static void build_special_1 (operation_index operation) {
	// nop, retn, retf, leave, lock, hlt
	const byte data [1 * SPECIAL_1_COUNT] = {
		0x90, 0xc3, 0xcb, 0xc9, 0xf0, 0xf4
	};

	input (1, data [operation - SPECIAL_1_BEGIN]);
}

static void build_special_2 (operation_index operation) {
	// sysenter, sysleave, syscall, sysret, pause, cpuid
	const byte data [2 * SPECIAL_2_COUNT] = {
		0x0f, 0x0f, 0x0f, 0x0f, 0xf3, 0x0f,
		0x34, 0x35, 0x05, 0x07, 0x90, 0xa2
	};

	input (1, data [operation - SPECIAL_2_BEGIN]);
	input (1, data [operation - SPECIAL_2_BEGIN + SPECIAL_2_COUNT]);
}

static void build_jump_if (operation_index operation,
                           size_index      size,
                           next            location) {
	// j**
	(void) size; // For now, everything is a far jump.

	input (far  (location), 0x0f);
	input (far  (location), (byte) (0x80 + operation - JUMP_IF_BEGIN));
	input (near (location), (byte) (0x70 + operation - JUMP_IF_BEGIN));

	// displacement (4, 0X12345678);
}

static void build_move_if (operation_index operation,
                           size_index      size,
                           type_index      to,
                           next            destination,
                           type_index      from,
                           next            source) {
	// cmov**
	build_short_prefix (size == D16);

	build_long_prefix (size == D64,
	                  (to   == REG) && (upper ((form) destination)),
	                  (from == REG) && (upper ((form) source)));

	input (1, 0x0f);
	input (1, (byte) (0x40 + operation - MOVE_IF_BEGIN));

	build_register_direction ((to == REG) && (from == REG),
	                          destination, source);

	build_register_redirection ((to == REG) && (from == MEM), destination);

	// displacement (4, 0X12345678); // Not implemented at this point!
}

static void build_jump (size_index size,
                        type_index to,
                        next       destination) {
	// jmp
	input ((to == REG) && upper ((form) destination), (byte) 0X41);

	input (to == REL, (byte) (0xe9 + 0x02 * (size == D8)));
	input (to == REG, (byte) 0xff);
	input (to == REG, (byte) (0xe0 + 0x01 * (destination & 0x07)));
	input (to == MEM, (byte) 0xff);
	input (to == MEM, (byte) 0x25);

	// displacement (to == MEM, 4, 0x12345678); // Keep when in mind!
}

static void build_move (size_index size,
                        type_index to,
                        next       destination,
                        type_index from,
                        next       source) {
	// mov
	build_short_prefix (size == D16);

	build_long_prefix (size == D64,
	                  (to   == REG) && (upper ((form) destination)),
	                  (from == REG) && (upper ((form) source)));

	input ((to == REG) && (from == REG), (byte) (0x88 + (size != D8)));
	input ((to == REG) && (from == MEM), (byte) (0x8a + (size != D8)));
	input ((to == MEM) && (from == REG), (byte) (0x88 + (size != D8)));

	build_register_redirection ((to == REG) && (from == MEM), destination);
	build_register_redirection ((to == MEM) && (from == REG), source);

	input ((to == REG) && ((from == IMM) || (from == REL)),
	       (byte) (0xb8
	          // + 0x08 * (size != D8)
	             + 0x01 * (destination & 0x07)));

	input ((to == MEM) && (from == IMM), (byte) (0xc6 + (size != D8)));
	input ((to == MEM) && (from == IMM), (byte) (0x05));

	input_at ((to == REG) && (from == MEM), D32,  source);
	input_by ((to == REG) && (from == IMM), size, source);
	input_at ((to == MEM) && (from == REG), D32,  (next) ~0);
	input_at ((to == MEM) && (from == IMM), D32,  (next) ~0);
	input_by ((to == MEM) && (from == IMM), size, source);
	input_at ((to == REG) && (from == REL), D32,  source);
}

static void build_call (type_index from,
                        next       source) {
	// call
	(void) from;
	(void) source;
}

static void assemble_clean_up (void) {
	if (assemble_clean_up_queued == 0) {
		return;
	}

	free (text_sector_byte);
	free (empty_array);
	free (empty_imbue);
	free (empty_store);
}

next   text_sector_size = 0;
byte * text_sector_byte = NULL;

int was_instruction_array_empty = 0;

void assemble (next   count,
               next * array) {
	// Main batched assembling procedure.
	next index;

	if ((count == 0) || (array == NULL)) {
		was_instruction_array_empty = 1;
		return;
	}

	// Right now, batching is not supported, it will be soon.
	// In that case, realloc function will be used with memset.
	text_sector_byte = calloc (1440UL, sizeof (* text_sector_byte));
	empty_array      = calloc (144UL,  sizeof (* empty_array));
	empty_imbue      = calloc (144UL,  sizeof (* empty_imbue));
	empty_store      = calloc (144UL,  sizeof (* empty_store));

	if (assemble_clean_up_queued == 0) {
		atexit (assemble_clean_up);
		assemble_clean_up_queued = ! assemble_clean_up_queued;
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
		// } else if (array [index] == ASMDIRREP) {
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
			index += 5;
		} else if (array [index] == CALL) {
			build_call (array [index + 1], array [index + 2]);
			index += 2;
		} else {
			return;
		}
	}

	for (index = 0; index < empty_holes; ++index) {
		next set = 0, get = empty_array [index];
		memcpy (& set, & text_sector_byte [get], sizeof (set));
		set += empty_store [empty_imbue [index]];
		memcpy (& text_sector_byte [get], & set, sizeof (set));
	}
}
