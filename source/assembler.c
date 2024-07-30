#include "assembler.h"
#include "debug.h"

#include <stdlib.h>

#define DOUBLE_BEGIN   (ADD)
#define DOUBLE_END     (CMP)
#define SINGLE_BEGIN   (INC)
#define SINGLE_END     (IDIV)
#define STATIC_1_BEGIN (NOP)
#define STATIC_1_END   (PUSHF)
#define STATIC_2_BEGIN (SYSCALL)
#define STATIC_2_END   (FCOS)
#define JUMP_IF_BEGIN  (JO)
#define JUMP_IF_END    (JG)
#define MOVE_IF_BEGIN  (CMOVO)
#define MOVE_IF_END    (CMOVG)
#define FLOAT_BEGIN    (FADD)
#define FLOAT_END      (FDIVR)
#define SHIFT_BEGIN    (ROL)
#define SHIFT_END      (SAR)

static const char * size_name [] = {
    "\033[1;36md8 \033[0m",         "\033[1;36md16\033[0m",         "\033[1;36md32\033[0m",         "\033[1;36md64\033[0m"
};

static const char * type_name [] = {
    "\033[1;34mrel\033[0m",         "\033[1;34mreg\033[0m",         "\033[1;34mmem\033[0m",         "\033[1;34mimm\033[0m"
};

static const char * data_name [] = {
    "\033[1;33masmdirmem\033[0m",   "\033[1;33masmdirrel\033[0m",   "\033[1;33masmdirimm\033[0m",   "\033[1;33masmdirrep\033[0m",
    "\033[1;33madd\033[0m",         "\033[1;33mor\033[0m",          "\033[1;33madc\033[0m",         "\033[1;33msbb\033[0m",
    "\033[1;33mand\033[0m",         "\033[1;33msub\033[0m",         "\033[1;33mxor\033[0m",         "\033[1;33mcmp\033[0m",
    "\033[1;33minc\033[0m",         "\033[1;33mdec\033[0m",         "\033[1;33mnot\033[0m",         "\033[1;33mneg\033[0m",
    "\033[1;33mmul\033[0m",         "\033[1;33mimul\033[0m",        "\033[1;33mdiv\033[0m",         "\033[1;33midiv\033[0m",
    "\033[1;33mfadd\033[0m",        "\033[1;33mfmul\033[0m",        "\033[1;33mfcom\033[0m",        "\033[1;33mfcomp\033[0m",
    "\033[1;33mfsub\033[0m",        "\033[1;33mfsubr\033[0m",       "\033[1;33mfdiv\033[0m",        "\033[1;33mfdivr\033[0m",
    "\033[1;33mrol\033[0m",         "\033[1;33mror\033[0m",         "\033[1;33mrcl\033[0m",         "\033[1;33mrcr\033[0m",
    "\033[1;33msal\033[0m",         "\033[1;33mshr\033[0m",         "\033[1;33mshl\033[0m",         "\033[1;33msar\033[0m",
    "\033[1;33mnop\033[0m",         "\033[1;33mretn\033[0m",        "\033[1;33mretf\033[0m",        "\033[1;33mleave\033[0m",
    "\033[1;33mpopf\033[0m",        "\033[1;33mpushf\033[0m",
    "\033[1;33msyscall\033[0m",     "\033[1;33mcpuid\033[0m",       "\033[1;33mfnop\033[0m",        "\033[1;33mfchs\033[0m",
    "\033[1;33mfabs\033[0m",        "\033[1;33mftst\033[0m",        "\033[1;33mfxam\033[0m",        "\033[1;33mfld1\033[0m",
    "\033[1;33mfldl2t\033[0m",      "\033[1;33mfldl2e\033[0m",      "\033[1;33mfldpi\033[0m",       "\033[1;33mfldlg2\033[0m",
    "\033[1;33mfldln2\033[0m",      "\033[1;33mfldz\033[0m",        "\033[1;33mf2xm1\033[0m",       "\033[1;33mfyl2x\033[0m",
    "\033[1;33mfptan\033[0m",       "\033[1;33mfpatan\033[0m",      "\033[1;33mfxtract\033[0m",     "\033[1;33mfprem1\033[0m",
    "\033[1;33mfdecstp\033[0m",     "\033[1;33mfincstp\033[0m",     "\033[1;33mfprem\033[0m",       "\033[1;33mfyl2xp1\033[0m",
    "\033[1;33mfsqrt\033[0m",       "\033[1;33mfsincos\033[0m",     "\033[1;33mfrndint\033[0m",     "\033[1;33mfscale\033[0m",
    "\033[1;33mfsin\033[0m",        "\033[1;33mfcos\033[0m",
    "\033[1;33menter\033[0m",       "\033[1;33mcall\033[0m",        "\033[1;33min\033[0m",          "\033[1;33mout\033[0m",
    "\033[1;33mjmp\033[0m",         "\033[1;33mmov\033[0m",         "\033[1;33mpop\033[0m",         "\033[1;33mpush\033[0m",
    "\033[1;33mjo\033[0m",          "\033[1;33mjno\033[0m",         "\033[1;33mjb\033[0m",          "\033[1;33mjae\033[0m",
    "\033[1;33mje\033[0m",          "\033[1;33mjne\033[0m",         "\033[1;33mjbe\033[0m",         "\033[1;33mja\033[0m",
    "\033[1;33mjs\033[0m",          "\033[1;33mjns\033[0m",         "\033[1;33mjpe\033[0m",         "\033[1;33mjpo\033[0m",
    "\033[1;33mjl\033[0m",          "\033[1;33mjge\033[0m",         "\033[1;33mjle\033[0m",         "\033[1;33mjg\033[0m",
    "\033[1;33mcmovo\033[0m",       "\033[1;33mcmovno\033[0m",      "\033[1;33mcmovb\033[0m",       "\033[1;33mcmovae\033[0m",
    "\033[1;33mcmove\033[0m",       "\033[1;33mcmovne\033[0m",      "\033[1;33mcmovbe\033[0m",      "\033[1;33mcmova\033[0m",
    "\033[1;33mcmovs\033[0m",       "\033[1;33mcmovns\033[0m",      "\033[1;33mcmovpe\033[0m",      "\033[1;33mcmovpo\033[0m",
    "\033[1;33mcmovl\033[0m",       "\033[1;33mcmovge\033[0m",      "\033[1;33mcmovle\033[0m",      "\033[1;33mcmovg\033[0m",
    "\033[1;33mbswap\033[0m",       "\033[1;33mtest\033[0m",        "\033[1;33mxchg\033[0m",        "\033[1;33mlea\033[0m",
    "\033[1;33mbsf\033[0m",         "\033[1;33mbsr\033[0m",
    "\033[1;33mrep\033[0m",         "\033[1;33mrepe\033[0m",        "\033[1;33mrepne\033[0m",       "\033[1;33mrepz\033[0m",
    "\033[1;33mrepnz\033[0m",       "\033[1;33mloop\033[0m",        "\033[1;33mloope\033[0m",       "\033[1;33mloopne\033[0m"
};

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

static void inset (int when, unsigned int data) {
	text_sector_byte [text_sector_size] = (unsigned char) data;

	text_sector_size += (unsigned int) when;
}

static void inset_immediate (int when, unsigned int size, unsigned int data) {
	inset ((when),                  (data >>  0) & 0xff);
	inset ((when) && (size >= D16), (data >>  8) & 0xff);
	inset ((when) && (size >= D32), (data >> 16) & 0xff);
	inset ((when) && (size >= D32), (data >> 24) & 0xff);
}

static void inset_memory (int when, unsigned int size, unsigned int data, unsigned int base) {
	empty_array [empty_holes] = text_sector_size;
	empty_imbue [empty_holes] = data;

	empty_holes += (unsigned int) when;

	inset_immediate (when, size, base);
}

static unsigned int store_relative (unsigned int * array) {
	unsigned int relative = array [1];

	debug_printf ("> %s %u", data_name [array [0]], array [1]);

	empty_array [empty_holes] = text_sector_size;
	empty_imbue [empty_holes] = relative;

	++empty_holes;

	return (1);
}

static unsigned int store_memory (unsigned int * array) {
	unsigned int memory = array [1];

	debug_printf ("> %s %u", data_name [array [0]], array [1]);

	empty_store [memory] = text_sector_size;

	++empty_count;

	return (1);
}

static unsigned int store_immediate (unsigned int * array) {
	unsigned int index  = 0,
	             size   = array [1],
	             amount = array [2];

	debug_printf ("> %s %s %u", data_name [array [0]], size_name [array [1]], array [2]);

	for (index = 0; index < amount; ++index) {
		inset_immediate (1, size, array [3 + index]);
	}

	return (amount + 2);
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
	inset (size == D16, 0x66);
}

static void long_prefix (unsigned int size, unsigned int to, unsigned int destination, unsigned int from, unsigned int source) {
	unsigned int to_upper   = (to   == REG) && (upper (destination));
	unsigned int from_upper = (from == REG) && (upper (source));

	inset ((size == D64) || (to_upper) || (from_upper), 0x40 + 0x01 * to_upper + 0x04 * from_upper + 0x08 * (size == D64));
}

static void modify_registers (unsigned int to, unsigned int destination, unsigned int from, unsigned int source) {
	inset ((to == REG) && (from == REG), 0xc0 + 0x01 * (destination & 0x07) + 0x08 * (source & 0x07));
}

static void modify_memory (unsigned int operation, unsigned int to, unsigned int from) {
	inset (((to == MEM) && (from == REG)) || ((to == REG) && (from == MEM)), 0x05 + 0x08 * operation * ((to == MEM) && (from == IMM)));
}

// REFACTORING IN PROGRESS
static unsigned int build_double (unsigned int * array) {
	unsigned int operation   = array [0],
	             size        = array [1],
	             to          = array [2],
	             destination = array [3],
	             from        = array [4],
	             source      = array [5];

	debug_printf ("> %s %s %s %u %s %u", data_name [array [0]], size_name [array [1]], type_name [array [2]], array [3], type_name [array [4]], array [5]);

	short_prefix (size);

	long_prefix (size, to, destination, from, source);

	inset ((size == D8) && (to == REG) && ((from == REG) || (from == IMM)) && (( (front (destination) && lower (source)) || (lower (destination) && front (source))) || ((to == REG) && (from == IMM) && front (destination))), 0x40);

	inset ((from == IMM) && (to == REG), 0x81 - 0x01 * (size == D8));

	inset ((from == IMM) && (to == REG) && (destination == 0), 0x05 + 0x08 * (operation & 0x07) - 0x01 * (size == D8));

	inset (! ((from == IMM) && (to == REG) && (destination == 0)), (destination & 0x07) * ((to == REG) && (from == IMM)) + 0x08 * (operation - DOUBLE_BEGIN) + 0x01 * ((to == MEM) && (from == IMM) && (size == D8)) - 0x01 * ((to == REG) && (from == IMM) && (size != D8)) + 0x01 * (size != D8) + 0x02 * ((to == REG) && (from == MEM)) + 0x04 * ((to == MEM) && (from == IMM)) + 0xc0 * ((to == REG) && (from == IMM)));

	modify_registers (to, destination, from, source);

	modify_memory (destination, to, from);
	modify_memory (source,      to, from);

	inset_memory    ((to == REG) && (from == MEM), D32,  source, 0x1000 - (text_sector_size + 4));
	inset_immediate ((to == REG) && (from == IMM), size, source);
	inset_memory    ((to == MEM) && (from == REG), D32,  destination, 0x1000 - (text_sector_size + 4));
	inset_memory    ((to == MEM) && (from == IMM), D32,  destination, 0x1000 - (text_sector_size + 4));
	inset_immediate ((to == MEM) && (from == IMM), size, source);
	inset_memory    ((to == REG) && (from == REL), D32,  source, 0x4010b0 - (text_sector_size + 4));

	return (5);
}

static unsigned int build_single (unsigned int * array) {
	unsigned int operation   = array [0],
	             size        = array [1],
	             to          = array [2],
	             destination = array [3];

	debug_printf ("> %s %s %s %u", data_name [array [0]], size_name [array [1]], type_name [array [2]], array [3]);

	short_prefix (size);

	long_prefix (size, to, destination, 0, 0);

	inset ((size == D8) && (to == REG) && front (destination), 0x40);

	inset (1, 0xf7 + 0x08 * ((operation == INC) || (operation == DEC)) - 0x01 * (size == D8));

	// THIS CAN BE REFACTORED TO C0F8 AND 053D
	inset (to == REG, 0xc0 + 0x08 * (operation - SINGLE_BEGIN) + 0x01 * (destination & 0x07));
	inset (to == MEM, 0x05 + 0x08 * (operation - SINGLE_BEGIN));

	inset_memory (to == MEM, D32, destination, 0x1000 - (text_sector_size + 4));

	return (3);
}

static unsigned int build_static_1 (unsigned int * array) {
	unsigned int operation = array [0];

	const unsigned char data  [] = {
		0x90, 0xc3, 0xcb, 0xc9, 0x9d, 0x9c
	};

	debug_printf ("> %s", data_name [array [0]]);

	inset (1, data [operation - STATIC_1_BEGIN]);

	return (0);
}

static unsigned int build_static_2 (unsigned int * array) {
	unsigned int operation = array [0];

	const unsigned short data  [] = {
		0x050f, 0xa20f, 0xd0d9, 0xe0d9, 0xe1d9, 0xe4d9, 0xe5d9, 0xe8d9,
		0xe9d9, 0xead9, 0xebd9, 0xecd9, 0xedd9, 0xeed9, 0xf0d9, 0xf1d9,
		0xf2d9, 0xf3d9, 0xf4d9, 0xf5d9, 0xf6d9, 0xf7d9, 0xf8d9, 0xf9d9,
		0xfad9, 0xfbd9, 0xfcd9, 0xfdd9, 0xfed9, 0xffd9
	};

	debug_printf ("> %s", data_name [array [0]]);

	inset_immediate (1, D16, data [operation - STATIC_2_BEGIN]);

	return (0);
}

static unsigned int build_jump_if (unsigned int * array) {
	unsigned int operation = array [0],
	             size      = array [1],
	             location  = array [3];

	debug_printf ("> %s %s \033[0;31mrel\033[0m %u", data_name [array [0]], size_name [array [1]], array [3]);

	inset (far (location) && (size == D32), 0x0f);

	inset (far  (location), 0x80 + operation - JUMP_IF_BEGIN);
	inset (near (location), 0x70 + operation - JUMP_IF_BEGIN);

	inset_memory (1, D32, location, -(text_sector_size + 4));

	return (3);
}

static unsigned int build_move_if (unsigned int * array) {
	unsigned int operation   = array [0],
	             size        = array [1],
	             to          = array [2],
	             destination = array [3],
	             from        = array [4],
	             source      = array [5];

	debug_printf ("> %s %s %s %u %s %u", data_name [array [0]], size_name [array [1]], type_name [array [2]], array [3], type_name [array [4]], array [5]);

	short_prefix (size);

	long_prefix (size, to, destination, from, source);

	inset (1, 0x0f);
	inset (1, 0x40 + operation - MOVE_IF_BEGIN);

	modify_registers (to, destination, from, source);
	modify_memory (destination, to, from);

	return (5);
}

static unsigned int build_jump (unsigned int * array) {
	unsigned int size        = array [1],
	             to          = array [2],
	             destination = array [3];

	debug_printf ("> %s %s %s %u", data_name [array [0]], size_name [array [1]], type_name [array [2]], array [3]);

	inset ((to == REG) && upper (destination), 0X41);

	inset (to == REL, 0xe9 + 0x02 * (size == D8));
	inset (to == REG, 0xff);
	inset (to == REG, 0xe0 + 0x01 * (destination & 0x07));
	inset (to == MEM, 0xff);
	inset (to == MEM, 0x25);

	inset_memory (to == REL, D32, destination, -(text_sector_size + 4));
	inset_memory (to == MEM, D32, destination, 0x4010b0);

	return (3);
}

static unsigned int build_move (unsigned int * array) {
	unsigned int size        = array [1],
	             to          = array [2],
	             destination = array [3],
	             from        = array [4],
	             source      = array [5],
	             extension   = array [6];

	debug_printf ("> %s %s %s %u %s %u %u", data_name [array [0]], size_name [array [1]], type_name [array [2]], array [3], type_name [array [4]], array [5], (size == D64) ? array [6] : 0);

	short_prefix (size);

	long_prefix (size, to, destination, from, source);

	inset ((to == REG) && (from == REG), 0x88 + 0x01 * (size != D8));
	inset ((to == REG) && (from == MEM), 0x8a + 0x01 * (size != D8));
	inset ((to == MEM) && (from == REG), 0x88 + 0x01 * (size != D8));

	modify_memory (destination, to, from);
	modify_memory (source,      to, from);

	modify_registers (to, destination, from, source);

	inset ((to == REG) && ((from == IMM) || (from == REL)), 0xb0 + 0x08 * (size != D8) + 0x01 * (destination & 0x07));

	inset ((to == MEM) && (from == IMM), 0xc6 + 0x01 * (size != D8));
	inset ((to == MEM) && (from == IMM), 0x05);

	inset_memory ((to == REG) && (from == MEM), D32, source,      0x1000 - (text_sector_size + 4));
	inset_memory ((to == MEM) && (from == REG), D32, destination, 0x1000 - (text_sector_size + 4));
	inset_memory ((to == MEM) && (from == IMM), D32, destination, 0x1000 - (text_sector_size + 4));
	inset_memory ((to == REG) && (from == REL), D32, source,      0x4010b0);

	inset_immediate ((to == REG) && (from == IMM) && (size != D64), size, source);
	inset_immediate ((to == MEM) && (from == IMM) && (size != D64), size, source);
	inset_immediate ((to == REG) && (from == IMM) && (size == D64), D32,  source);
	inset_immediate ((to == REG) && (from == IMM) && (size == D64), D32,  extension);
	inset_immediate ((to == REG) && (from == IMM) && (size == D64), D32,  0);

	return (5 + (size == D64));
}

static unsigned int build_call (unsigned int * array) {
	unsigned int from   = array [1],
	             source = array [2];

	debug_printf ("> %s %s %u", data_name [array [0]], type_name [array [1]], array [2]);

	inset ((from == REG) && (upper (source)), 0x41);

	inset (from == REL, 0xe8);
	inset (from == REG, 0xff);

	inset_memory (from == REL, D32, source, -(text_sector_size + 4));

	inset (from == REG, (0xd0 + 0x01 * (source & 0x07)));

	return (2);
}

static unsigned int build_enter (unsigned int * array) {
	unsigned int dynamic_storage = array [1],
	             nesting_level   = array [2];

	debug_printf ("> %s %u %u", data_name [array [0]], array [1], array [2]);

	inset (1, 0xc8);

	inset_immediate (1, D16, dynamic_storage);
	inset_immediate (1, D8,  nesting_level & 0x1f);

	return (2);
}

static unsigned int build_float (unsigned int * array) {
	unsigned int operation = array [0],
	             size      = array [1],
	             from      = array [2],
	             source    = array [3];

	debug_printf ("> %s %s %s %u", data_name [array [0]], size_name [array [1]], type_name [array [2]], array [3]);

	inset (from == MEM, 0xd8 + 0x04 * (size == D64));

	modify_memory (operation, 0, from);

	inset_memory (from == MEM, size, source, 0);

	return (3);
}

static unsigned int build_shift (unsigned int * array) {
	unsigned int operation   = array [0],
	             size        = array [1],
	             to          = array [2],
	             destination = array [3],
	             offset      = array [5];

	debug_printf ("> %s %s %s %u \033[0;31mimm\033[0m %u", data_name [array [0]], size_name [array [1]], type_name [array [2]], array [3], array [5]);

	short_prefix (size);

	long_prefix (size, to, destination, 0, 0);

	inset (1, 0xc0 + 0x01 * (size != D8));

	inset (to == REG, 0x05 + 0x08 * (operation & 7));
	inset (to == MEM, 0xc0 + 0x08 * (operation & 7));

	inset_memory    (to == MEM, D32, destination, 0x1000 - (text_sector_size + 4));
	inset_immediate (1,         D8,  offset);

	return (5);
}

static unsigned int build_in_out (unsigned int * array) {
	unsigned int move = array [0],
	             size = array [1],
	             type = array [2],
	             port = array [3];

	debug_printf ("> %s %s %s %u", data_name [array [0]], size_name [array [1]], type_name [array [2]], array [3]);

	short_prefix (size);

	inset (1, 0xe4 + 0x01 * (size != D8) + 0x02 * (move != OUT) + 0x08 * (type == REG));

	inset_immediate (type == IMM, D8, port);

	return (3);
}

static unsigned int build_pop (unsigned int * array) {
	unsigned int size        = array [1],
	             to          = array [2],
	             destination = array [3];

	debug_printf ("> %s %s %s %u", data_name [array [0]], size_name [array [1]], type_name [array [2]], array [3]);

	short_prefix (size);

	inset ((to == REG) && (upper (destination)), 0x41);

	inset (to == REG, 0x58 + 0x01 * (destination & 0x07));
	inset (to == MEM, 0x8f);
	inset (to == MEM, 0x05);

	inset_memory (to == MEM, D32, destination, 0);

	return (3);
}

static unsigned int build_push (unsigned int * array) {
	unsigned int size   = array [1],
	             from   = array [2],
	             source = array [3];

	debug_printf ("> %s %s %s %u", data_name [array [0]], size_name [array [1]], type_name [array [2]], array [3]);

	short_prefix (size);

	inset ((from == REG) && (upper (source)), 0x41);

	inset (from == REG, 0x50 + 0x01 * (source & 0x07));
	inset (from == MEM, 0xff);
	inset (from == MEM, 0x35);
	inset (from == IMM, 0x68 + 0x02 * (size == D8));

	inset_memory (from == MEM, D32,  source, 0);
	inset_immediate (from == IMM, size, source);

	return (3);
}

static unsigned int fault (unsigned int * array) {
	debug_printf ("> \033[0;31m%u\033[0m", array [0]);

	return (0);
}

static const unsigned int (* build_instruction []) (unsigned int * array) = {
	store_memory,   store_relative, store_immediate,
	fault, // ASMDIRREP IS UNIMPLEMENTED CURRENTLY
	build_double,   build_double,   build_double,   build_double,   build_double,   build_double,   build_double,   build_double,
	build_single,   build_single,   build_single,   build_single,   build_single,   build_single,   build_single,   build_single,
	build_float,    build_float,    build_float,    build_float,    build_float,    build_float,    build_float,    build_float,
	build_shift,    build_shift,    build_shift,    build_shift,    build_shift,    build_shift,    build_shift,    build_shift,
	build_static_1, build_static_1, build_static_1, build_static_1, build_static_1, build_static_1,
	build_static_2, build_static_2, build_static_2, build_static_2, build_static_2, build_static_2, build_static_2, build_static_2,
	build_static_2, build_static_2, build_static_2, build_static_2, build_static_2, build_static_2, build_static_2, build_static_2,
	build_static_2, build_static_2, build_static_2, build_static_2, build_static_2, build_static_2, build_static_2, build_static_2,
	build_static_2, build_static_2, build_static_2, build_static_2, build_static_2, build_static_2,
	build_enter,    build_call,     build_in_out,   build_in_out,   build_jump,     build_move,     build_pop,      build_push,
	build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,
	build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,
	build_move_if,  build_move_if,  build_move_if,  build_move_if,  build_move_if,  build_move_if,  build_move_if,  build_move_if,
	build_move_if,  build_move_if,  build_move_if,  build_move_if,  build_move_if,  build_move_if,  build_move_if,  build_move_if,
	fault,          fault,          fault,          fault,          fault,          fault,
	fault,          fault,          fault,          fault,          fault,          fault,          fault,          fault
};

unsigned int    nopification     = 1;
unsigned int    text_entry_point = 0;
unsigned int    text_sector_size = 0;
unsigned char * text_sector_byte = NULL;

int was_instruction_array_empty = 0;

int assemble (unsigned int count, unsigned int * array) {
	unsigned int index;

	if ((count == 0) || (array == NULL)) {
		was_instruction_array_empty = 1;
		return (EXIT_FAILURE);
	}

	text_sector_byte = calloc (4096UL, sizeof (* text_sector_byte));
	empty_array      = calloc (1024UL, sizeof (* empty_array));
	empty_imbue      = calloc (1024UL, sizeof (* empty_imbue));
	empty_store      = calloc (1024UL, sizeof (* empty_store));

	for (index = 0; index < count; ++index) {
		unsigned int check_at;
		unsigned int byte;

		inset ((nopification) && (array [index] > ASMDIRREP), 0x90);

		check_at = text_sector_size;

		index += build_instruction [array [index]] (& array [index]);

		debug_printf (" -- \033[0;35m");

		for (byte = check_at; byte < text_sector_size; ++byte) {
			debug_printf ("%02X ", text_sector_byte [byte]);
		}

		debug_printf ("\033[0m\n");
	}

	text_entry_point = empty_store [0];

	for (index = 1; index < empty_holes; ++index) {
		unsigned int set = 0;
		unsigned int get = empty_array [index];

		replace ((unsigned char *) & set, & text_sector_byte [get], sizeof (set));

		set += empty_store [empty_imbue [index]];

		replace (& text_sector_byte [get], (unsigned char *) & set, sizeof (set));
	}

	free (text_sector_byte);
	free (empty_array);
	free (empty_imbue);
	free (empty_store);

	return (0);
}
