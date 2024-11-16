#include "assembler.h"
#include "debug.h"

#include <stdlib.h>

#define DOUBLE_BEGIN   (ADD)
//~#define DOUBLE_END     (CMP)
#define SINGLE_BEGIN   (INC)
//~#define SINGLE_END     (IDIV)
#define STATIC_1_BEGIN (NOP)
//~#define STATIC_1_END   (PUSHF)
#define STATIC_2_BEGIN (SYSCALL)
//~#define STATIC_2_END   (FCOS)
#define JUMP_IF_BEGIN  (JO)
//~#define JUMP_IF_END    (JG)
#define MOVE_IF_BEGIN  (CMOVO)
//~#define MOVE_IF_END    (CMOVG)
#define SET_IF_BEGIN   (SETO)
//~#define SET_IF_END     (SETG)
#define FLOAT_BEGIN    (FADD)
//~#define FLOAT_END      (FDIVR)
#define SHIFT_BEGIN    (ROL)
//~#define SHIFT_END      (SAR)

#if DEBUG == 1

static const char * size_name [] = {
    "d8",           "d16",          "d32",          "d64",
    "d80",          "d128",         "d256",         "d512"
};

static const char * operand_name [] = {
    "rel",          "reg",          "mem",          "imm"
};

static const char * operation_name [] = {
    "asmdirmem",    "asmdirrel",    "asmdirimm",    "asmdirrep",
    "add",          "or",           "adc",          "sbb",
    "and",          "sub",          "xor",          "cmp",
    "inc",          "dec",          "not",          "neg",
    "mul",          "imul",         "div",          "idiv",
    "fadd",         "fmul",         "fcom",         "fcomp",
    "fsub",         "fsubr",        "fdiv",         "fdivr",
    "rol",          "ror",          "rcl",          "rcr",
    "sal",          "shr",          "shl",          "sar",
    "nop",          "retn",         "retf",         "leave",
    "popf",         "pushf",
    "syscall",      "cpuid",        "fnop",         "fchs",
    "fabs",         "ftst",         "fxam",         "fld1",
    "fldl2t",       "fldl2e",       "fldpi",        "fldlg2",
    "fldln2",       "fldz",         "f2xm1",        "fyl2x",
    "fptan",        "fpatan",       "fxtract",      "fprem1",
    "fdecstp",      "fincstp",      "fprem",        "fyl2xp1",
    "fsqrt",        "fsincos",      "frndint",      "fscale",
    "fsin",         "fcos",
    "enter",        "call",         "in",           "out",
    "jmp",          "mov",          "pop",          "push",
    "jo",           "jno",          "jb",           "jae",
    "je",           "jne",          "jbe",          "ja",
    "js",           "jns",          "jpe",          "jpo",
    "jl",           "jge",          "jle",          "jg",
    "cmovo",        "cmovno",       "cmovb",        "cmovae",
    "cmove",        "cmovne",       "cmovbe",       "cmova",
    "cmovs",        "cmovns",       "cmovpe",       "cmovpo",
    "cmovl",        "cmovge",       "cmovle",       "cmovg",
    "seto",         "setno",        "setb",         "setae",
    "sete",         "setne",        "setbe",        "seta",
    "sets",         "setns",        "setpe",        "setpo",
    "setl",         "setge",        "setle",        "setg",
    "bswap",        "bsf",          "bsr",          "loop",
    "loope",        "loopne"
};

#endif

static int   empty_count = 1;
static int   empty_holes = 1;
static int * empty_array = NULL;
static int * empty_imbue = NULL;
static int * empty_store = NULL;

static void replace(char * destination,
                    char * source,
                    int    size) {
    for (--size; size != -1; --size) {
        destination[size] = source[size];
    }
}

static void inset(int  when,
                  int data) {
    text_sector_byte[text_sector_size] = (char)data;

    text_sector_size += (int)when;
}

static void inset_immediate(int  when,
                            int size,
                            int data) {
    inset((when),                  (data >>  0) & 0xff);
    inset((when) && (size >= D16), (data >>  8) & 0xff);
    inset((when) && (size >= D32), (data >> 16) & 0xff);
    inset((when) && (size >= D32), (data >> 24) & 0xff);
}

static void inset_memory(int  when,
                         int size,
                         int data,
                         int base) {
    empty_array[empty_holes] = text_sector_size;
    empty_imbue[empty_holes] = data;

    empty_holes += (int)when;

    inset_immediate(when, size, base);
}

static int store_relative(int * array) {
    int relative = array[1];

    empty_array[empty_holes] = text_sector_size;
    empty_imbue[empty_holes] = relative;

    ++empty_holes;

    return 1;
}

static int store_memory(int * array) {
    int memory = array[1];

    empty_store[memory] = text_sector_size;

    ++empty_count;

    return 1;
}

static int store_immediate(int * array) {
    int size   = array[1],
             amount = array[2];

    for (int index = 0; index < amount; ++index) {
        inset_immediate(1, size, array[3 + index]);
    }

    return amount + 2;
}

static int front(int data) { return (data >= GR4) && (data <= GR7); }
static int upper(int data) { return (data >= GR8) && (data <= GR15); }
static int lower(int data) { return data <= GR7; }

// We don't use these yet, hardcoded.
static int far(int label)  { return label && 1; }
static int near(int label) { return label && 0; }

static int trr(int to, int from) {
    return (to == REG) && (from == REG);
}

static int trm(int to, int from) {
    return (to == REG) && (from == MEM);
}

static int tmr(int to, int from) {
    return (to == MEM) && (from == REG);
}

static int tmi(int to, int from) {
    return (to == MEM) && (from == IMM);
}

static int tri(int to, int from) {
    return (to == REG) && (from == IMM);
}

static int trl(int to, int from) {
    return (to == REG) && (from == REL);
}

static void short_prefix(int size) {
    inset(size == D16, 0x66);
}

static void long_prefix(int size,
                        int to,
                        int destination,
                        int from,
                        int source) {
    int to_upper   = (to   == REG) && (upper(destination));
    int from_upper = (from == REG) && (upper(source));

    // Refactor.
    inset ((size == D64) || (to_upper) || (from_upper), 0x40 +
        0x01 * to_upper + 0x04 * from_upper + 0x08 * (size == D64));
}

static void modify_registers(int to,
                             int destination,
                             int from,
                             int source) {
    // Refactor.
    inset (trr(to, from), 0xc0 + 0x01 * (destination & 0x07) + 0x08 * (source & 0x07));
}

static void modify_memory(int operation,
                          int to,
                          int from) {
    // Refactor.
    inset ((tmr(to, from)) || (trm(to, from)), 0x05 +
        0x08 * operation * (tmi(to, from)));
}

// REFACTORING IN PROGRESS
static int build_double(int * array) {
    int operation   = array[0],
             size        = array[1],
             to          = array[2],
             destination = array[3],
             from        = array[4],
             source      = array[5];

    debug_error(size > D64, "size : double = %i; -- XBA\n", size);
    debug_error(to   > MEM, "to   : double = %i; -- XBA\n", to);
    debug_error(from > IMM, "from : double = %i; -- XBA\n", from);

    debug_printf("@y%s@- @b%s@- @c%s@- %u @c%s@- %u",
                 operation_name [operation],
                 size_name [size],
                 operand_name [to],
                 destination,
                 operand_name [from],
                 source);

    short_prefix(size);

    long_prefix(size, to, destination, from, source);

    // What the fuck...?
    inset((size == D8) && (to == REG) && ((from == REG) || (from == IMM)) &&
        (((front(destination) && lower(source)) || (lower(destination) && front(source))) ||
        (tri(to, from) && front(destination))), 0x40);

    inset((from == IMM) && (to == REG), 0x81 - 0x01 * (size == D8));

    inset((from == IMM) && (to == REG) && (destination == 0), 0x05 + 0x08 * (operation & 0x07) - 0x01 * (size == D8));

    // Seriously, what the fuck...?
    inset(! ((from == IMM) && (to == REG) && (destination == 0)),
        (destination & 0x07) * (tri(to, from)) +
        0x08 * (operation - DOUBLE_BEGIN) +
        0x01 * (tmi(to, from) && (size == D8)) -
        0x01 * (tri(to, from) && (size != D8)) +
        0x01 * (size != D8) + 0x02 * (trm(to, from)) +
        0x04 * (tmi(to, from)) +
        0xc0 * (tri(to, from)));

    modify_registers(to, destination, from, source);

    modify_memory(destination, to, from);
    modify_memory(source,      to, from);

    inset_memory(trm(to, from), D32,  source, 0x1000 - text_sector_size - 4);

    inset_immediate(tri(to, from), size, source);

    inset_memory(tmr(to, from), D32,  destination, 0x1000 - text_sector_size - 4);
    inset_memory(tmi(to, from), D32,  destination, 0x1000 - text_sector_size - 4);

    inset_immediate(tmi(to, from), size, source);

    inset_memory(trl(to, from), D32,  source, 0x4010b0 - text_sector_size - 4);

    return 5;
}

static int build_single(int * array) {
    int operation   = array[0],
             size        = array[1],
             to          = array[2],
             destination = array[3];

    debug_error(size > D64, "size : single = %i; -- XBA\n", size);
    debug_error(to   > MEM, "to   : single = %i; -- XBA\n", to);

    debug_printf("@y%s@- @b%s@- @c%s@- %u",
                 operation_name [operation],
                 size_name [size],
                 operand_name [to],
                 destination);

    short_prefix(size);

    long_prefix(size, to, destination, 0, 0);

    inset((size == D8) && (to == REG) && front(destination), 0x40);

    inset(1, 0xf7 + 0x08 * ((operation == INC) || (operation == DEC)) - 0x01 * (size == D8));

    // THIS CAN BE REFACTORED TO C0F8 AND 053D
    // This old comment should be respected out...
    // And there's probably a way to shorten these.
    inset(to == REG, 0xc0 + 0x08 * (operation - SINGLE_BEGIN) + 0x01 * (destination & 0x07));
    inset(to == MEM, 0x05 + 0x08 * (operation - SINGLE_BEGIN));

    inset_memory(to == MEM, D32, destination, 0x1000 - text_sector_size - 4);

    return 3;
}

static int build_static_1(int * array) {
    int operation = array[0];

    const unsigned char data[] = {
        0x90, 0xc3, 0xcb, 0xc9, 0x9d, 0x9c
    };

    debug_printf("@y%s@-", operation_name [operation]);

    inset(1, data[operation - STATIC_1_BEGIN]);

    return 0;
}

static int build_static_2(int * array) {
    int operation = array[0];

    const unsigned short data[] = {
        0x050f, 0xa20f, 0xd0d9, 0xe0d9, 0xe1d9, 0xe4d9, 0xe5d9, 0xe8d9,
        0xe9d9, 0xead9, 0xebd9, 0xecd9, 0xedd9, 0xeed9, 0xf0d9, 0xf1d9,
        0xf2d9, 0xf3d9, 0xf4d9, 0xf5d9, 0xf6d9, 0xf7d9, 0xf8d9, 0xf9d9,
        0xfad9, 0xfbd9, 0xfcd9, 0xfdd9, 0xfed9, 0xffd9
    };

    debug_printf("@y%s@-", operation_name [operation]);

    inset_immediate(1, D16, data[operation - STATIC_2_BEGIN]);

    return 0;
}

static int build_jump_if(int * array) {
    int operation = array[0],
             size      = array[1],
             location  = array[3];

    inset(far(location) && (size == D32), 0x0f);

    inset(far(location),  0x80 + operation - JUMP_IF_BEGIN);
    inset(near(location), 0x70 + operation - JUMP_IF_BEGIN);

    inset_memory(1, D32, location, -text_sector_size - 4);

    return 3;
}

static int build_move_if(int * array) {
    int operation   = array[0],
             size        = array[1],
             to          = array[2],
             destination = array[3],
             from        = array[4],
             source      = array[5];

    short_prefix(size);

    long_prefix(size, to, destination, from, source);

    inset(1, 0x0f);
    inset(1, 0x40 + operation - MOVE_IF_BEGIN);

    modify_registers(to, destination, from, source);

    modify_memory(destination, to, from);

    return 5;
}

static int build_set_if(int * array) {
    int operation   = array[0],
             to          = array[2],
             destination = array[3];

    inset((to == REG) && (front(destination)), 0x40);
    inset((to == REG) && (upper(destination)), 0x41);

    inset(1, 0x0f);
    inset(1, 0x90 + operation - SET_IF_BEGIN);

    inset(to == REG, 0xc0 + 0x01 * (destination & 0x07));
    inset(to == MEM, 0x05);

    inset_memory(to == MEM, D32, destination, 0x1000 - text_sector_size - 4);

    return 3;
}

static int build_jump(int * array) {
    int size        = array[1],
             to          = array[2],
             destination = array[3];

    inset((to == REG) && upper(destination), 0X41);

    inset(to == REL, 0xe9 + 0x02 * (size == D8));
    inset(to == REG, 0xff);
    inset(to == REG, 0xe0 + 0x01 * (destination & 0x07));
    inset(to == MEM, 0xff);
    inset(to == MEM, 0x25);

    inset_memory(to == REL, D32, destination, -text_sector_size - 4);
    inset_memory(to == MEM, D32, destination, 0x4010b0);

    return 3;
}

// Please refactor this entire crap...
static int build_move(int * array) {
    int size        = array[1],
             to          = array[2],
             destination = array[3],
             from        = array[4],
             source      = array[5],
             extension   = array[6],
             offset      = 0x1000 - text_sector_size - 4;

    debug_error(size > D64, "size : move = %i; -- XBA\n", size);
    debug_error(to   > MEM, "to   : move = %i; -- XBA\n", to);
    debug_error(from > IMM, "from : move = %i; -- XBA\n", from);

    debug_printf("@ymov@- @b%s@- @c%s@- %u @c%s@- %u %u",
                 size_name [size],
                 operand_name [to],
                 destination,
                 operand_name [from],
                 source,
                 (size == D64) ? extension : 0);

    short_prefix(size);

    long_prefix(size, to, destination, from, source);

    inset(trr(to, from), 0x88 + 0x01 * (size != D8));
    inset(trm(to, from), 0x8a + 0x01 * (size != D8));
    inset(tmr(to, from), 0x88 + 0x01 * (size != D8));

    modify_memory(destination, to, from);
    modify_memory(source,      to, from);

    modify_registers(to, destination, from, source);

    inset((to == REG) && ((from == IMM) || (from == REL)), 0xb0 + 0x08 * (size != D8) + 0x01 * (destination & 0x07));

    inset(tmi(to, from), 0xc6 + 0x01 * (size != D8));
    inset(tmi(to, from), 0x05);

    inset_memory(trm(to, from), D32, source,      offset);
    inset_memory(tmr(to, from), D32, destination, offset);
    inset_memory(tmi(to, from), D32, destination, offset);
    inset_memory(trl(to, from), D32, source,      0x4010b0);

    inset_immediate(tri(to, from) && (size <= D32), size, source);
    inset_immediate(tmi(to, from) && (size <= D32), size, source);

    inset_immediate(tri(to, from) && (size == D64), D32, source);
    inset_immediate(tri(to, from) && (size == D64), D32, extension);
    inset_immediate(tri(to, from) && (size == D64), D32, 0);

    return 5 + (size == D64);
}

static int build_call(int * array) {
    int from   = array[1],
        source = array[2];

    debug_printf("@ycall@- @c%s@- %u",
                 operand_name [from],
                 source);

    inset((from == REG) && (upper(source)), 0x41);

    inset(from == REL, 0xe8);
    inset(from == REG, 0xff);

    inset_memory(from == REL, D32, source, -text_sector_size - 4);

    inset(from == REG, (0xd0 + 0x01 * (source & 0x07)));

    return 2;
}

static int build_enter(int * array) {
    int dynamic_storage = array[1],
             nesting_level   = array[2];

    debug_printf("@yenter@- %u %u", dynamic_storage, nesting_level);

    inset(1, 0xc8);

    inset_immediate(1, D16, dynamic_storage);
    inset_immediate(1, D8,  nesting_level & 0x1f);

    return 2;
}

// Check if this works at all...
static int build_float(int * array) {
    int operation = array[0],
        size      = array[1],
        from      = array[2],
        source    = array[3];

    inset(from == MEM, 0xd8 + 0x04 * (size == D64));

    modify_memory(operation - FLOAT_BEGIN, 0, from);

    inset_memory(from == MEM, size, source, 0);

    return 3;
}

static int build_shift(int * array) {
    int operation   = array[0],
        size        = array[1],
        to          = array[2],
        destination = array[3],
        offset      = array[5];

    short_prefix(size);

    long_prefix(size, to, destination, 0, 0);

    inset(1, 0xc0 + 0x01 * (size != D8));

    inset(to == REG, 0x05 + 0x08 * ((operation - SHIFT_BEGIN) & 7));
    inset(to == MEM, 0xc0 + 0x08 * ((operation - SHIFT_BEGIN) & 7));

    inset_memory(to == MEM, D32, destination, 0x1000 - text_sector_size - 4);

    inset_immediate(1, D8, offset);

    return 5;
}

static int build_in_out(int * array) {
    int move = array[0],
             size = array[1],
             type = array[2],
             port = array[3];

    short_prefix(size);

    // Shorten and extend, I think we're not covering all cases.
    inset(1, 0xe4 + 0x01 * (size != D8) + 0x02 * (move == IN) + 0x08 * (type == REG));

    inset_immediate(type == IMM, D8, port);

    return 3;
}

static int build_pop(int * array) {
    int size        = array[1],
             to          = array[2],
             destination = array[3];

    short_prefix(size);

    inset((to == REG) && (upper(destination)), 0x41);

    inset(to == REG, 0x58 + 0x01 * (destination & 0x07));
    inset(to == MEM, 0x8f);
    inset(to == MEM, 0x05);

    inset_memory(to == MEM, D32, destination, 0);

    return 3;
}

static int build_push(int * array) {
    int size   = array[1],
             from   = array[2],
             source = array[3];

    short_prefix(size);

    inset((from == REG) && (upper(source)), 0x41);

    inset(from == REG, 0x50 + 0x01 * (source & 0x07));
    inset(from == MEM, 0xff);
    inset(from == MEM, 0x35);
    inset(from == IMM, 0x68 + 0x02 * (size == D8));

    inset_memory(from == MEM, D32, source, 0);

    inset_immediate(from == IMM, size, source);

    return 3;
}

static int build_swap(int * array) {
    int size        = array[1],
             destination = array[3];

    long_prefix(size, REG, destination, 0, 0);

    inset(1, 0x0f);
    inset(1, 0xc8 + 0x01 * (destination & 0x07));

    return 3;
}

static int build_bit_scan(int * array) {
    int size        = array[1],
             destination = array[3],
             from        = array[4],
             source      = array[5];

    short_prefix(size);

    long_prefix(size, REG, destination, from, source);

    inset_immediate(1, D16, 0xbc0f);

    modify_registers(REG, destination, from, source);

    inset(from == MEM, 0x05 + 0x08 * destination);

    inset_memory(from == MEM, D32, source, 0x1000 - text_sector_size - 4);

    return 5;
}

static int build_loop(int * array) {
    int location = array[3];

    inset(array[0] == LOOPNE, 0xe0);
    inset(array[0] == LOOPE,  0xe1);
    inset(array[0] == LOOP,   0xe2);

    inset_memory(1, D8, location, -text_sector_size - 1);

    return 3;
}

static int (*build_instruction[])(int * array) = {
    store_memory,   // ASMDIRMEM : LABEL
    store_relative, // ASMDIRREL : "IMPLEMENTED"
    store_immediate,// ASMDIRIMM : LITERAL
    NULL,           // ASMDIRREP : UNIMPLEMENTED
    build_double,   build_double,   build_double,   build_double,
    build_double,   build_double,   build_double,   build_double,
    build_single,   build_single,   build_single,   build_single,
    build_single,   build_single,   build_single,   build_single,
    build_float,    build_float,    build_float,    build_float,
    build_float,    build_float,    build_float,    build_float,
    build_shift,    build_shift,    build_shift,    build_shift,
    build_shift,    build_shift,    build_shift,    build_shift,
    build_static_1, build_static_1, build_static_1, build_static_1,
    build_static_1, build_static_1,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2,
    build_enter,    build_call,     build_in_out,   build_in_out,
    build_jump,     build_move,     build_pop,      build_push,
    build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,
    build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,
    build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,
    build_jump_if,  build_jump_if,  build_jump_if,  build_jump_if,
    build_move_if,  build_move_if,  build_move_if,  build_move_if,
    build_move_if,  build_move_if,  build_move_if,  build_move_if,
    build_move_if,  build_move_if,  build_move_if,  build_move_if,
    build_move_if,  build_move_if,  build_move_if,  build_move_if,
    build_set_if,   build_set_if,   build_set_if,   build_set_if,
    build_set_if,   build_set_if,   build_set_if,   build_set_if,
    build_set_if,   build_set_if,   build_set_if,   build_set_if,
    build_set_if,   build_set_if,   build_set_if,   build_set_if,
    build_swap,     build_bit_scan, build_bit_scan, build_loop,
    build_loop,     build_loop
};

int    main_entry_point = 0;
int    text_sector_size = 0;
char * text_sector_byte = NULL;
int    data_sector_size = 0;    // This is unused, and it should be used...
char * data_sector_byte = NULL; // This is unused, and it should be used...

int was_instruction_array_empty = 0;

int assemble (int count, int * array) {
    if ((!count) || (!array)) {
        was_instruction_array_empty = 1;
        return EXIT_FAILURE;
    }

    empty_array = calloc(1024ul, sizeof(*empty_array));
    empty_imbue = calloc(1024ul, sizeof(*empty_imbue));
    empty_store = calloc(1024ul, sizeof(*empty_store));

    for (int index = 0; index < count; ++index) {
        inset(array[index] > ASMDIRREP, 0x90);

        int size = text_sector_size;

        index += build_instruction[array[index]](&array[index]);

        debug_printf(" -- ");
        for (int byte = size; byte < text_sector_size; ++byte) {
            debug_printf("@p%02X@- ", (unsigned char)text_sector_byte[byte]);
        }
        debug_printf("\n");
    }

    main_entry_point = empty_store[0];

    for (int index = 1; index < empty_holes; ++index) {
        int set = 0;
        int get = empty_array[index];

        replace((char*)&set, &text_sector_byte[get], (int)sizeof(set));

        set += empty_store[empty_imbue[index]];

        replace(&text_sector_byte[get], (char*)&set, (int)sizeof(get));
    }

    free(empty_array);
    free(empty_imbue);
    free(empty_store);

    return EXIT_SUCCESS;
}
