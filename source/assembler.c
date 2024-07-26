#include "assembler.h"

#include <stdlib.h>

#define REGULAR_BEGIN   (ADD)
#define REGULAR_END     (CMP)
#define IRREGULAR_BEGIN (INC)
#define IRREGULAR_END   (IDIV)
#define SPECIAL_1_BEGIN (NOP)
#define SPECIAL_1_END   (PUSHF)
#define SPECIAL_2_BEGIN (SYSCALL)
#define SPECIAL_2_END   (FCOS)
#define JUMP_IF_BEGIN   (JO)
#define JUMP_IF_END     (JG)
#define MOVE_IF_BEGIN   (CMOVO)
#define MOVE_IF_END     (CMOVG)
#define FLOAT_BEGIN     (FADD)
#define FLOAT_END       (FDIVR)

static int assemble_clean_up_queued = 0;

static unsigned int   empty_count = 1;
static unsigned int   empty_holes = 1;
static unsigned int * empty_array = NULL;
static unsigned int * empty_imbue = NULL;
static unsigned int * empty_store = NULL;

static void replace (unsigned char * destination, unsigned char * source, unsigned long size) {
	for (--size; size != (unsigned long) -1; --size) {
		destination  [size] = source  [size];
	}
}

static void input (int when, unsigned int data) {
	text_sector_byte [text_sector_size] = (unsigned char) data;

	text_sector_size += (unsigned int) when;
}

static void input_by (int when, unsigned int size, unsigned int data) {
	input ((when),                  (data >>  0) & 0xff);
	input ((when) && (size >= D16), (data >>  8) & 0xff);
	input ((when) && (size >= D32), (data >> 16) & 0xff);
	input ((when) && (size >= D32), (data >> 24) & 0xff);
}

static void asmdirrel (int when, unsigned int data) {
	empty_array [empty_holes] = text_sector_size;
	empty_imbue [empty_holes] = data;

	empty_holes += (unsigned int) when;
}

static void asmdirmem (int when, unsigned int code) {
	empty_store [code] = text_sector_size;

	empty_count += (unsigned int) when;
}

static void asmdirimm (int when, unsigned int size, unsigned int data) {
	input_by (when, size, data);
}

static void input_at (int when, unsigned int size, unsigned int data, unsigned int base) {
	asmdirrel (when, data);

	input_by (when, size, base);
}

static int front (unsigned int data) {
	return ((data >= GR4) && (data <= GR7));
}

static int lower (unsigned int data) {
	return (data <= GR7);
}

static int upper (unsigned int data) {
	return ((data >= GR8) && (data <= GR15));
}

static int far (unsigned int label) {
	return (label && 1);
}

static int near (unsigned int label) {
	return (label && 0);
}

static void short_prefix (unsigned int size) {
	input (size == D16, 0x66);
}

static void long_prefix (unsigned int size, unsigned int to, unsigned int destination, unsigned int from, unsigned int source) {
	int to_upper   = (to   == REG) && (upper (destination));
	int from_upper = (from == REG) && (upper (source));

	input ((size == D64) || (to_upper) || (from_upper), 0x40 + 0x01 * to_upper + 0x04 * from_upper + 0x08 * (size == D64));
}

static void modify_registers (unsigned int to, unsigned int destination, unsigned int from, unsigned int source) {
	input ((to == REG) && (from == REG), 0xc0 + 0x01 * (destination & 0x07) + 0x08 * (source & 0x07));
}

static void modify_memory (unsigned int operation, unsigned int to, unsigned int from) {
	input (((to == MEM) && (from == REG)) || ((to == REG) && (from == MEM)), 0x05 + 0x08 * operation * ((to == MEM) && (from == IMM)));
}

static void build_regular (unsigned int operation, unsigned int size, unsigned int to, unsigned int destination, unsigned int from, unsigned int source) {
	short_prefix (size);

	long_prefix (size, to, destination, from, source);

	input ((size == D8) && (to == REG)
	      && ((from == REG) || (from == IMM))
	      && (( (front (destination) && lower (source))
	      || (lower (destination) && front (source)))
	      || ((to == REG) && (from == IMM) && front (destination))),
	      0x40);

	input ((from == IMM) && (to == REG), 0x81 - 0x01 * (size == D8));

	input ((from == IMM) && (to == REG) && (destination == 0), 0x05 + 0x08 * (operation & 0x07) - 0x01 * (size == D8));

	input (! ((from == IMM) && (to == REG) && (destination == 0)),
	      (destination & 0x07) * ((to == REG) && (from == IMM))
	      + 0x08 * (operation - REGULAR_BEGIN)
	      + 0x01 * ((to   == MEM) && (from == IMM) && (size == D8))
	      - 0x01 * ((to   == REG) && (from == IMM) && (size != D8))
	      + 0x01 * (size != D8)
	      + 0x02 * ((to   == REG) && (from == MEM))
	      + 0x04 * ((to   == MEM) && (from == IMM))
	      + 0xc0 * ((to   == REG) && (from == IMM)));

	modify_registers (to, destination, from, source);

	modify_memory (destination, to, from);
	modify_memory (source,      to, from);

	input_at ((to == REG) && (from == MEM), D32,  source, 0x1000 - (text_sector_size + 4));
	input_by ((to == REG) && (from == IMM), size, source);
	input_at ((to == MEM) && (from == REG), D32,  destination, 0x1000 - (text_sector_size + 4));
	input_at ((to == MEM) && (from == IMM), D32,  destination, 0x1000 - (text_sector_size + 4));
	input_by ((to == MEM) && (from == IMM), size, source);
	input_at ((to == REG) && (from == REL), D32,  source, 0x4010b0 - (text_sector_size + 4));
}

static void build_irregular (unsigned int operation, unsigned int size, unsigned int to, unsigned int destination) {
	short_prefix (size);

	long_prefix (size, to, destination, 0, 0);

	input ((size == D8) && (to == REG) && front (destination), 0x40);

	input (1, 0xf7 + 0x08 * ((operation == INC) || (operation == DEC)) - 0x01 * (size == D8));

	input (to == REG, 0xc0 + 0x08 * (operation - IRREGULAR_BEGIN) + 0x01 * (destination & 0x07));
	input (to == MEM, 0x05 + 0x08 * (operation - IRREGULAR_BEGIN));

	input_at (to == MEM, D32, destination, 0x1000 - (text_sector_size + 4));
}

static void build_special_1 (unsigned int operation) {
	const unsigned char data  [] = {
		0x90, 0xc3, 0xcb, 0xc9, 0x9d, 0x9c
	};

	input (1, data [operation - SPECIAL_1_BEGIN]);
}

static void build_special_2 (unsigned int operation) {
	const unsigned short data  [] = {
		0x050f, 0xa20f, 0xd0d9, 0xe0d9, 0xe1d9, 0xe4d9, 0xe5d9, 0xe8d9,
		0xe9d9, 0xead9, 0xebd9, 0xecd9, 0xedd9, 0xeed9, 0xf0d9, 0xf1d9,
		0xf2d9, 0xf3d9, 0xf4d9, 0xf5d9, 0xf6d9, 0xf7d9, 0xf8d9, 0xf9d9,
		0xfad9, 0xfbd9, 0xfcd9, 0xfdd9, 0xfed9, 0xffd9
	};

	input_by (1, D16, data [operation - SPECIAL_2_BEGIN]);
}

static void build_jump_if (unsigned int operation, unsigned int size, unsigned int ignore, unsigned int location) {
	(void) ignore;

	input (far (location) && (size == D32), 0x0f);

	input (far  (location), 0x80 + operation - JUMP_IF_BEGIN);
	input (near (location), 0x70 + operation - JUMP_IF_BEGIN);

	input_at (1, D32, location, -(text_sector_size + 4));
}

static void build_move_if (unsigned int operation, unsigned int size, unsigned int to, unsigned int destination, unsigned int from, unsigned int source) {
	short_prefix (size);

	long_prefix (size, to, destination, from, source);

	input (1, 0x0f);
	input (1, 0x40 + operation - MOVE_IF_BEGIN);

	modify_registers (to, destination, from, source);
	modify_memory (destination, to, from);
}

static void build_jump (unsigned int size, unsigned int to, unsigned int destination) {
	input ((to == REG) && upper (destination), 0X41);

	input (to == REL, 0xe9 + 0x02 * (size == D8));
	input (to == REG, 0xff);
	input (to == REG, 0xe0 + 0x01 * (destination & 0x07));
	input (to == MEM, 0xff);
	input (to == MEM, 0x25);

	input_at (to == REL, D32, destination, -(text_sector_size + 4));
	input_at (to == MEM, D32, destination, 0x4010b0);
}

static void build_move (unsigned int size, unsigned int to, unsigned int destination, unsigned int from, unsigned int source) {
	short_prefix (size);

	long_prefix (size, to, destination, from, source);

	input ((to == REG) && (from == REG), 0x88 + 0x01 * (size != D8));
	input ((to == REG) && (from == MEM), 0x8a + 0x01 * (size != D8));
	input ((to == MEM) && (from == REG), 0x88 + 0x01 * (size != D8));

	modify_memory (destination, to, from);
	modify_memory (source,      to, from);

	modify_registers (to, destination, from, source);

	input ((to == REG) && ((from == IMM) || (from == REL)), 0xb8 + 0x01 * (destination & 0x07));

	input ((to == MEM) && (from == IMM), 0xc6 + 0x01 * (size != D8));
	input ((to == MEM) && (from == IMM), 0x05);

	input_at ((to == REG) && (from == MEM), D32,  source, 0x1000 - (text_sector_size + 4));
	input_by ((to == REG) && (from == IMM), size, source);
	input_at ((to == MEM) && (from == REG), D32,  destination, 0x1000 - (text_sector_size + 4));
	input_at ((to == MEM) && (from == IMM), D32,  destination, 0x1000 - (text_sector_size + 4));
	input_by ((to == MEM) && (from == IMM), size, source);
	input_at ((to == REG) && (from == REL), D32,  source, 0x4010b0);

	input_by ((to == REG) && (from == IMM) && (size == D64), D32, 0);
}

static void build_call (unsigned int from, unsigned int source) {
	input ((from == REG) && (upper (source)), 0x41);

	input (from == REL, 0xe8);
	input (from == REG, 0xff);

	input_at (from == REL, D32, source, -(text_sector_size + 4));

	input (from == REG, (0xd0 + 0x01 * (source & 0x07)));
}

static void build_enter (unsigned int dynamic_storage, unsigned int nesting_level) {
	input (1, 0xc8);

	input_by (1, D16, dynamic_storage);
	input_by (1, D8,  nesting_level & 0x1f);
}

static void build_in_out (unsigned int move, unsigned int size, unsigned int type, unsigned int port) {
	short_prefix (size);

	input (1, 0xe4 + 0x01 * (size != D8) + 0x02 * (move != OUT) + 0x08 * (type == REG));

	input_by (type == IMM, D8, port);
}

static void build_pop (unsigned int size, unsigned int to, unsigned int destination) {
	short_prefix (size);

	input ((to == REG) && (upper (destination)), 0x41);

	input (to == REG, 0x58 + 0x01 * (destination & 0x07));
	input (to == MEM, 0x8f);
	input (to == MEM, 0x05);

	input_at (to == MEM, D32, destination, 0);
}

static void build_push (unsigned int size, unsigned int from, unsigned int source) {
	short_prefix (size);

	input ((from == REG) && (upper (source)), 0x41);

	input (from == REG, 0x50 + 0x01 * (source & 0x07));
	input (from == MEM, 0xff);
	input (from == MEM, 0x35);
	input (from == IMM, 0x68 + 0x02 * (size == D8));

	input_at (from == MEM, D32,  source, 0);
	input_by (from == IMM, size, source);
}

static void build_float (unsigned int operation, unsigned int size, unsigned int from, unsigned int source) {
	input (from == MEM, 0xd8 + 0x04 * (size == D64));

	modify_memory (operation, 0, from);

	input_at (from == MEM, size, source, 0);
}

static void assemble_clean_up (void) {
	if (!assemble_clean_up_queued) {
		return;
	}

	free (text_sector_byte);
	free (empty_array);
	free (empty_imbue);
	free (empty_store);
}

unsigned int    text_sector_size = 0;
unsigned char * text_sector_byte = NULL;

         int was_instruction_array_empty = 0;
unsigned int text_entry_point            = 0;

void assemble (unsigned int count, unsigned int * array) {
	unsigned int index;

	if ((count == 0) || (array == NULL)) {
		was_instruction_array_empty = 1;
		return;
	}

	text_sector_byte = calloc (4096UL, sizeof (* text_sector_byte));
	empty_array      = calloc (1024UL, sizeof (* empty_array));
	empty_imbue      = calloc (1024UL, sizeof (* empty_imbue));
	empty_store      = calloc (1024UL, sizeof (* empty_store));

	if (!assemble_clean_up_queued) {
		atexit (assemble_clean_up);
		assemble_clean_up_queued = !assemble_clean_up_queued;
	}

	for (index = 0; index < count; ++index) {
		if (array [index] == ASMDIRREL) {
			asmdirrel (1, array [index + 1]);
			index += 1;
		} else if (array [index] == ASMDIRMEM) {
			asmdirmem (1, array [index + 1]);
			index += 1;
		} else if (array [index] == ASMDIRIMM) {
			unsigned int repeat;
			for (repeat = 0; repeat < array [index + 2]; ++repeat) {
				asmdirimm (1, array [index + 1], array [index + 3 + repeat]);
			}
			index += array [index + 2] + 2;
		} else if ((array [index] >= REGULAR_BEGIN) && (array [index] <= REGULAR_END)) {
			build_regular (array [index + 0], array [index + 1], array [index + 2], array [index + 3], array [index + 4], array [index + 5]);
			index += 5;
		} else if ((array [index] >= IRREGULAR_BEGIN) && (array [index] <= IRREGULAR_END)) {
			build_irregular (array [index + 0], array [index + 1], array [index + 2], array [index + 3]);
			index += 3;
		} else if ((array [index] >= SPECIAL_1_BEGIN) && (array [index] <= SPECIAL_1_END)) {
			build_special_1 (array [index + 0]);
			index += 0;
		} else if ((array [index] >= SPECIAL_2_BEGIN) && (array [index] <= SPECIAL_2_END)) {
			build_special_2 (array [index + 0]);
			index += 0;
		} else if ((array [index] >= JUMP_IF_BEGIN) && (array [index] <= JUMP_IF_END)) {
			build_jump_if (array [index + 0], array [index + 1], array [index + 2], array [index + 3]);
			index += 3;
		} else if ((array [index] >= MOVE_IF_BEGIN) && (array [index] <= MOVE_IF_END)) {
			build_move_if (array [index + 0], array [index + 1], array [index + 2], array [index + 3], array [index + 4], array [index + 5]);
			index += 5;
		} else if ((array [index] >= FLOAT_BEGIN) && (array [index] <= FLOAT_END)) {
			build_float (array [index + 0], array [index + 1], array [index + 2], array [index + 3]);
			index += 3;
		} else if (array [index] == JMP) {
			build_jump (array [index + 1], array [index + 2], array [index + 3]);
			index += 3;
		} else if (array [index] == MOV) {
			build_move (array [index + 1], array [index + 2], array [index + 3], array [index + 4], array [index + 5]);
			index += 5;
		} else if (array [index] == CALL) {
			build_call (array [index + 1], array [index + 2]);
			index += 2;
		} else if (array [index] == ENTER) {
			build_enter (array [index + 1], array [index + 2]);
			index += 2;
		} else if ((array [index] == IN) || (array [index] == OUT)) {
			build_in_out (array [index + 0], array [index + 1], array [index + 2], array [index + 3]);
			index += 3;
		} else if (array [index] == POP) {
			build_pop (array [index + 1], array [index + 2], array [index + 3]);
			index += 3;
		} else if (array [index] == PUSH) {
			build_push (array [index + 1], array [index + 2], array [index + 3]);
			index += 3;
		} else {
			exit (array [index]);
		}
	}

	text_entry_point = empty_store [0];

	for (index = 1; index < empty_holes; ++index) {
		unsigned int set = 0;
		unsigned int get = empty_array [index];

		replace ((unsigned char *) & set, & text_sector_byte [get], sizeof (set));

		set += empty_store [empty_imbue [index]];

		replace (& text_sector_byte [get], (unsigned char *) & set, sizeof (set));
	}
}
