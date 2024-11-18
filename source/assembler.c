#include "assembler.h"
#include "debug.h"
#include "arena.h"

#include <stdlib.h>

#define DOUBLE_BEGIN   (ADD)
#define SINGLE_BEGIN   (INC)
#define STATIC_1_BEGIN (NOP)
#define STATIC_2_BEGIN (SYSCALL)
#define JUMP_IF_BEGIN  (JO)
#define MOVE_IF_BEGIN  (CMOVO)
#define SET_IF_BEGIN   (SETO)
#define FLOAT_BEGIN    (FADD)
#define SHIFT_BEGIN    (ROL)

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

static int front(int data) { return (data >= GR4) && (data <= GR7);  }
static int upper(int data) { return (data >= GR8) && (data <= GR15); }
static int lower(int data) { return (data >= GR0) && (data <= GR7);  }

// We don't use these yet, hardcoded.
static int far (int label) { return label && 1; }
static int near(int label) { return label && 0; }

static int transfer(int to, int from) { return (to == REG) && (from == REG); }
static int import  (int to, int from) { return (to == REG) && (from == MEM); }
static int export  (int to, int from) { return (to == MEM) && (from == REG); }
static int assign  (int to, int from) { return (to == MEM) && (from == IMM); }
static int attach  (int to, int from) { return (to == REG) && (from == IMM); }
static int relate  (int to, int from) { return (to == REG) && (from == REL); }

static void replace(char * destination,
                    char * source,
                    int    size) {
    for (--size; size != -1; --size) {
        destination[size] = source[size];
    }
}

static void inset(int when,
                  int data) {
    text_sector_byte[text_sector_size] = (char)data;

    text_sector_size += when;
}

static void inset_immediate(int when,
                            int size,
                            int data) {
    inset((when),                  (data >>  0) & 0xff);
    inset((when) && (size >= D16), (data >>  8) & 0xff);
    inset((when) && (size >= D32), (data >> 16) & 0xff);
    inset((when) && (size >= D32), (data >> 24) & 0xff);
}

static void inset_memory(int when,
                         int size,
                         int data,
                         int base) {
    empty_array[empty_holes] = text_sector_size;
    empty_imbue[empty_holes] = data;

    empty_holes += when;

    inset_immediate(when, size, base);
}

static void short_prefix(int size) {
    inset(size == D16, 0x66);
}

static void long_prefix(int size,
                        int to,
                        int destination,
                        int from,
                        int source) {
    const int long_destination = (to   == REG) && (upper(destination));
    const int long_source      = (from == REG) && (upper(source));
    const int long_size        = (size == D64);

    inset(long_destination || long_source || long_size,
          0x40 +
          0x01 * long_destination +
          0x04 * long_source +
          0x08 * long_size);
}

static void modify_registers(int to,
                             int destination,
                             int from,
                             int source) {
    // Refactor.
    inset(transfer(to, from),
          0xc0 +
          0x01 * (destination & 7) +
          0x08 * (source      & 7));
}

static void modify_memory(int operation,
                          int to,
                          int from) {
    inset((export(to, from)) || (import(to, from)),
          0x05 +
          0x08 * operation * (assign(to, from)));
}

static int store_relative(const int * restrict array) {
    const int relative = array[1];

    debug_print("@yasmrel@- %i", relative);

    empty_array[empty_holes] = text_sector_size;
    empty_imbue[empty_holes] = relative;

    ++empty_holes;

    return 1;
}

static int store_memory(const int * restrict array) {
    const int memory = array[1];

    debug_print("@yasmmem@- %i", memory);

    empty_store[memory] = text_sector_size;

    ++empty_count;

    return 1;
}

static int store_immediate(const int * restrict array) {
    const int size   = array[1];
    const int amount = array[2];

    debug_print("@yasmimm@- @b%s@- %i", size_name [size], amount);

    for (int index = 0; index < amount; ++index) {
        inset_immediate(1, size, array[3 + index]);
    }

    return amount + 2;
}

// REFACTORING IN PROGRESS
static int build_double(const int * restrict array) {
    const int operation   = array[0];
    const int size        = array[1];
    const int to          = array[2];
    const int destination = array[3];
    const int from        = array[4];
    const int source      = array[5];
    const int offset      = text_sector_size + 4;

    debug_error(size > D64, "size : double = %i; -- XBA\n", size);
    debug_error(to   > MEM, "to   : double = %i; -- XBA\n", to);
    debug_error(from > IMM, "from : double = %i; -- XBA\n", from);

    debug_print("@y%s@- @b%s@- @c%s@- %i @c%s@- %i",
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
          (((front(destination) && lower(source)) ||
            (lower(destination) && front(source))) ||
              (attach(to, from) && front(destination))), 0x40);

    inset((from == IMM) && (to == REG),
          0x81 -
          0x01 * (size == D8));

    inset((from == IMM) && (to == REG) && (destination == 0),
          0x05 +
          0x08 * (operation & 7) -
          0x01 * (size == D8));

    // Seriously, what the fuck...?
    inset(!((from == IMM) && (to == REG) && (destination == 0)),
          (destination & 7) * (attach(to, from)) +
          0x08 * (operation - DOUBLE_BEGIN) +
          0x01 * (assign(to, from) && (size == D8)) -
          0x01 * (attach(to, from) && (size != D8)) +
          0x01 * (size != D8) +
          0x02 * (import(to, from)) +
          0x04 * (assign(to, from)) +
          0xc0 * (attach(to, from)));

    modify_registers(to, destination, from, source);

    modify_memory(destination, to, from);
    modify_memory(source,      to, from);

    inset_memory(import(to, from), D32,  source, 0x1000 - offset);

    inset_immediate(attach(to, from), size, source);

    inset_memory(export(to, from), D32, destination, 0x1000 - offset);
    inset_memory(assign(to, from), D32, destination, 0x1000 - offset);

    inset_immediate(assign(to, from), size, source);

    inset_memory(relate(to, from), D32, source, 0x4010b0 - offset);

    return 5;
}

static int build_single(const int * restrict array) {
    const int operation   = array[0];
    const int size        = array[1];
    const int to          = array[2];
    const int destination = array[3];
    const int offset      = text_sector_size + 4;

    debug_error(size > D64, "size : single = %i; -- XBA\n", size);
    debug_error(to   > MEM, "to   : single = %i; -- XBA\n", to);

    debug_print("@y%s@- @b%s@- @c%s@- %i",
                operation_name [operation],
                size_name [size],
                operand_name [to],
                destination);

    short_prefix(size);

    long_prefix(size, to, destination, 0, 0);

    inset((size == D8) && (to == REG) && front(destination), 0x40);

    inset(1,
          0xf7 +
          0x08 * ((operation == INC) || (operation == DEC)) -
          0x01 * (size == D8));

    // THIS CAN BE REFACTORED TO C0F8 AND 053D
    // This old comment should be respected out...
    // And there's probably a way to shorten these.
    inset(to == REG,
          0xc0 +
          0x08 * (operation - SINGLE_BEGIN) +
          0x01 * (destination & 7));

    inset(to == MEM,
          0x05 +
          0x08 * (operation - SINGLE_BEGIN));

    inset_memory(to == MEM, D32, destination, 0x1000 - offset);

    return 3;
}

static int build_static_1(const int * restrict array) {
    const int operation = array[0];

    const unsigned char data[] = {
        0x90, 0xc3, 0xcb, 0xc9, 0x9d, 0x9c
    };

    debug_print("@y%s@-", operation_name [operation]);

    inset(1, data[operation - STATIC_1_BEGIN]);

    return 0;
}

static int build_static_2(const int * restrict array) {
    const int operation = array[0];

    const unsigned short data[] = {
        0x050f, 0xa20f, 0xd0d9, 0xe0d9, 0xe1d9, 0xe4d9, 0xe5d9, 0xe8d9,
        0xe9d9, 0xead9, 0xebd9, 0xecd9, 0xedd9, 0xeed9, 0xf0d9, 0xf1d9,
        0xf2d9, 0xf3d9, 0xf4d9, 0xf5d9, 0xf6d9, 0xf7d9, 0xf8d9, 0xf9d9,
        0xfad9, 0xfbd9, 0xfcd9, 0xfdd9, 0xfed9, 0xffd9
    };

    debug_print("@y%s@-", operation_name [operation]);

    inset_immediate(1, D16, data[operation - STATIC_2_BEGIN]);

    return 0;
}

static int build_jump_if(const int * restrict array) {
    const int operation = array[0];
    const int size      = array[1];
    const int location  = array[3];

    debug_print("@y%s@- @b%s@- @crel@- %i",
                operation_name [operation],
                size_name [size],
                location);

    inset(far(location) && (size == D32), 0x0f);

    inset(far(location),  0x80 + operation - JUMP_IF_BEGIN);
    inset(near(location), 0x70 + operation - JUMP_IF_BEGIN);

    inset_memory(1, D32, location, -text_sector_size - 4);

    return 3;
}

static int build_move_if(const int * restrict array) {
    const int operation   = array[0];
    const int size        = array[1];
    const int to          = array[2];
    const int destination = array[3];
    const int from        = array[4];
    const int source      = array[5];

    debug_print("@y%s@- @b%s@- @c%s@- %i @c%s@- %i",
                operation_name [operation],
                size_name [size],
                operand_name [to],
                destination,
                operand_name [from],
                source);

    short_prefix(size);

    long_prefix(size, to, destination, from, source);

    inset(1, 0x0f);
    inset(1, 0x40 + operation - MOVE_IF_BEGIN);

    modify_registers(to, destination, from, source);

    modify_memory(destination, to, from);

    return 5;
}

static int build_set_if(const int * restrict array) {
    const int operation   = array[0];
    const int to          = array[2];
    const int destination = array[3];

    debug_print("@y%s@- @bd8@- @c%s@- %i",
                operation_name [operation],
                operand_name [to],
                destination);

    inset((to == REG) && (front(destination)), 0x40);
    inset((to == REG) && (upper(destination)), 0x41);

    inset(1, 0x0f);
    inset(1, 0x90 + operation - SET_IF_BEGIN);

    inset(to == REG, 0xc0 + 0x01 * (destination & 7));
    inset(to == MEM, 0x05);

    inset_memory(to == MEM, D32, destination, 0x1000 - text_sector_size - 4);

    return 3;
}

static int build_jump(const int * restrict array) {
    const int size        = array[1];
    const int to          = array[2];
    const int destination = array[3];

    debug_print("@yjmp@- @b%s@- @c%s@- %i",
                size_name [size],
                operand_name [to],
                destination);

    inset((to == REG) && upper(destination), 0X41);

    inset(to == REL, 0xe9 + 0x02 * (size == D8));
    inset(to == REG, 0xff);
    inset(to == REG, 0xe0 + 0x01 * (destination & 7));
    inset(to == MEM, 0xff);
    inset(to == MEM, 0x25);

    inset_memory(to == REL, D32, destination, -text_sector_size - 4);
    inset_memory(to == MEM, D32, destination, 0x4010b0);

    return 3;
}

// Please refactor this entire crap...
static int build_move(const int * restrict array) {
    const int size        = array[1];
    const int to          = array[2];
    const int destination = array[3];
    const int from        = array[4];
    const int source      = array[5];
    const int extension   = array[6];
    const int offset      = 0x1000 - text_sector_size - 4;

    debug_error(size > D64, "size : move = %i; -- XBA\n", size);
    debug_error(to   > MEM, "to   : move = %i; -- XBA\n", to);
    debug_error(from > IMM, "from : move = %i; -- XBA\n", from);

    debug_print("@ymov@- @b%s@- @c%s@- %i @c%s@- %i %i",
                size_name [size],
                operand_name [to],
                destination,
                operand_name [from],
                source,
                (size == D64) ? extension : 0);

    short_prefix(size);

    long_prefix(size, to, destination, from, source);

    inset(transfer(to, from), 0x88 + 0x01 * (size != D8));
    inset(import(to, from), 0x8a + 0x01 * (size != D8));
    inset(export(to, from), 0x88 + 0x01 * (size != D8));

    modify_memory(destination, to, from);
    modify_memory(source,      to, from);

    modify_registers(to, destination, from, source);

    inset((to == REG) && ((from == IMM) || (from == REL)), 0xb0 +
          0x08 * (size != D8) + 0x01 * (destination & 7));

    inset(assign(to, from), 0xc6 + 0x01 * (size != D8));
    inset(assign(to, from), 0x05);

    inset_memory(import(to, from), D32, source,      offset);
    inset_memory(export(to, from), D32, destination, offset);
    inset_memory(assign(to, from), D32, destination, offset);
    inset_memory(relate(to, from), D32, source,      0x4010b0);

    inset_immediate(attach(to, from) && (size <= D32), size, source);
    inset_immediate(assign(to, from) && (size <= D32), size, source);

    inset_immediate(attach(to, from) && (size == D64), D32, source);
    inset_immediate(attach(to, from) && (size == D64), D32, extension);
    inset_immediate(attach(to, from) && (size == D64), D32, 0);

    return 5 + (size == D64);
}

static int build_call(const int * restrict array) {
    const int from   = array[1];
    const int source = array[2];

    debug_print("@ycall@- @c%s@- %i", operand_name [from], source);

    inset((from == REG) && (upper(source)), 0x41);

    inset(from == REL, 0xe8);
    inset(from == REG, 0xff);

    inset_memory(from == REL, D32, source, -text_sector_size - 4);

    inset(from == REG, (0xd0 + 0x01 * (source & 7)));

    return 2;
}

static int build_enter(const int * restrict array) {
    const int dynamic_storage = array[1];
    const int nesting_level   = array[2];

    debug_print("@yenter@- %i %i", dynamic_storage, nesting_level);

    inset(1, 0xc8);

    inset_immediate(1, D16, dynamic_storage);
    inset_immediate(1, D8,  nesting_level & 0x1f);

    return 2;
}

// Check if this works at all...
static int build_float(const int * restrict array) {
    const int operation = array[0];
    const int size      = array[1];
    const int from      = array[2];
    const int source    = array[3];

    debug_print("@y%s@- @b%s@- @c%s@- %i",
                operation_name [operation],
                size_name [size],
                operand_name [from],
                source);

    inset(from == MEM, 0xd8 + 0x04 * (size == D64));

    modify_memory(operation - FLOAT_BEGIN, 0, from);

    inset_memory(from == MEM, size, source, 0);

    return 3;
}

static int build_shift(const int * restrict array) {
    const int operation   = array[0];
    const int size        = array[1];
    const int to          = array[2];
    const int destination = array[3];
    const int offset      = array[5];

    debug_print("@y%s@- @b%s@- @c%s@- %i @cimm@- %i",
                operation_name [operation],
                size_name [size],
                operand_name [to],
                destination,
                offset);

    short_prefix(size);

    long_prefix(size, to, destination, 0, 0);

    inset(1, 0xc0 + 0x01 * (size != D8));

    inset(to == REG, 0x05 + 0x08 * ((operation - SHIFT_BEGIN) & 7));
    inset(to == MEM, 0xc0 + 0x08 * ((operation - SHIFT_BEGIN) & 7));

    inset_memory(to == MEM, D32, destination, 0x1000 - text_sector_size - 4);

    inset_immediate(1, D8, offset);

    return 5;
}

static int build_in_out(const int * restrict array) {
    const int move = array[0];
    const int size = array[1];
    const int type = array[2];
    const int port = array[3];

    debug_print("@y%s@- @b%s@- @c%s@- %i",
                operation_name [move],
                size_name [size],
                operand_name [type],
                port);

    short_prefix(size);

    // Shorten and extend, I think we're not covering all cases.
    inset(1, 0xe4 +
             0x01 * (size != D8) +
             0x02 * (move == IN) +
             0x08 * (type == REG));

    inset_immediate(type == IMM, D8, port);

    return 3;
}

static int build_pop(const int * restrict array) {
    const int size        = array[1];
    const int to          = array[2];
    const int destination = array[3];

    debug_print("@ypop@- @b%s@- @c%s@- %i",
                size_name [size],
                operand_name [to],
                destination);

    short_prefix(size);

    inset((to == REG) && (upper(destination)), 0x41);

    inset(to == REG, 0x58 + 0x01 * (destination & 7));
    inset(to == MEM, 0x8f);
    inset(to == MEM, 0x05);

    inset_memory(to == MEM, D32, destination, 0);

    return 3;
}

static int build_push(const int * restrict array) {
    const int size   = array[1];
    const int from   = array[2];
    const int source = array[3];

    debug_print("@ypush@- @b%s@- @c%s@- %i",
                size_name [size],
                operand_name [from],
                source);

    short_prefix(size);

    inset((from == REG) && (upper(source)), 0x41);

    inset(from == REG, 0x50 + 0x01 * (source & 7));
    inset(from == MEM, 0xff);
    inset(from == MEM, 0x35);
    inset(from == IMM, 0x68 + 0x02 * (size == D8));

    inset_memory(from == MEM, D32, source, 0);

    inset_immediate(from == IMM, size, source);

    return 3;
}

static int build_swap(const int * restrict array) {
    const int size        = array[1];
    const int destination = array[3];

    debug_print("@yswap@- @b%s@- @creg@- %i",
                size_name [size],
                destination);

    long_prefix(size, REG, destination, 0, 0);

    inset(1, 0x0f);
    inset(1, 0xc8 + 0x01 * (destination & 7));

    return 3;
}

// Forward or reverse...?!
static int build_bit_scan(const int * restrict array) {
    const int operation   = array[0];
    const int size        = array[1];
    const int destination = array[3];
    const int from        = array[4];
    const int source      = array[5];

    debug_print("@y%s@- @b%s@- @creg@- %i @c%s@- %i",
                operation_name [operation],
                size_name [size],
                destination,
                operand_name [from],
                source);

    short_prefix(size);

    long_prefix(size, REG, destination, from, source);

    inset_immediate(1, D16, 0xbc0f + 0x100 * (operation == BSR));

    modify_registers(REG, destination, from, source);

    inset(from == MEM, 0x05 + 0x08 * destination);

    inset_memory(from == MEM, D32, source, 0x1000 - text_sector_size - 4);

    return 5;
}

static int build_loop(const int * restrict array) {
    const int operation = array[0];
    const int location  = array[3];

    debug_print("@y%s@- @bd8@- @crel@- %i",
                operation_name [operation],
                location);

    inset(operation == LOOPNE, 0xe0);
    inset(operation == LOOPE,  0xe1);
    inset(operation == LOOP,   0xe2);

    inset_memory(1, D8, location, -text_sector_size - 1);

    return 3;
}

static int (*build_instruction[])(const int * restrict array) = {
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

int assemble (      int            count,
              const int * restrict array) {
    if ((!count) || (!array)) {
        was_instruction_array_empty = 1;
        return EXIT_FAILURE;
    }

    empty_array = aalloc(1024ul * sizeof(*empty_array));
    empty_imbue = aalloc(1024ul * sizeof(*empty_imbue));
    empty_store = aalloc(1024ul * sizeof(*empty_store));

    for (int index = 0; index < count; ++index) {
        const int size = text_sector_size;

#if DEBUG == 1
        inset(array[index] > ASMDIRREP, 0x90);
#endif
        index += build_instruction[array[index]](&array[index]);

        debug_print(" @a--@- ");
        for (int byte = size; byte < text_sector_size; ++byte) {
            debug_print("@p%02X@- ", (unsigned char)text_sector_byte[byte]);
        }
        debug_print("\n");
    }

    main_entry_point = empty_store[0];

    for (int index = 1; index < empty_holes; ++index) {
        int set = 0;
        int get = empty_array[index];

        replace((char*)&set, &text_sector_byte[get], (int)sizeof(set));

        set += empty_store[empty_imbue[index]];

        replace(&text_sector_byte[get], (char*)&set, (int)sizeof(get));
    }

    return EXIT_SUCCESS;
}
