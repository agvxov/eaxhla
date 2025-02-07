#include "assembler.h"
#include "debug.h"
#include "arena.h"

#define DOUBLE_BEGIN   (ADD) /// X: INTERFACE SHOULDN'T CHANGE!
#define DOUBLE_END     (CMP)
#define SINGLE_BEGIN   (INC)
#define SINGLE_END     (IDIV)
#define SHIFT_BEGIN    (ROL)
#define SHIFT_END      (SAR)
#define FLOAT_BEGIN    (FADD)
#define FLOAT_END      (FDIVR)
#define FLOATL_BEGIN   (FLADD)
#define FLOATL_END     (FLDIVR)
#define FMOVE_IF_BEGIN (FCMOVB)
#define FMOVE_IF_END   (FCMOVNU)
#define STATIC_1_BEGIN (NOP)
#define STATIC_1_END   (RETF)
#define STATIC_2_BEGIN (SYSCALL)
#define STATIC_2_END   (FNCLEX)
#define STATIC_3_BEGIN (MONITOR)
#define STATIC_3_END   (FCLEX)
#define JUMP_IF_BEGIN  (JO)
#define JUMP_IF_END    (JG)
#define MOVE_IF_BEGIN  (CMOVO)
#define MOVE_IF_END    (CMOVG)
#define SET_IF_BEGIN   (SETO)
#define SET_IF_END     (SETG)

#if DEBUG == 1

static const char * size_name[SIZE_END] = {
    "d8",           "d16",          "d32",          "d64",
    "d80",          "d128",         "d256",         "d512",
};

static const char * operand_name[OPERAND_END] = {
    "rel",          "reg",          "mem",          "imm",
    "der",          "int",          "bcf",          "bcd",
};

static const char * operation_name[OPERATION_END] = {
    "asmdirmem",    "asmdirrel",    "asmdirimm",    "asmdirrep",
    "add",          "or",           "adc",          "sbb",
    "and",          "sub",          "xor",          "cmp",
    "inc",          "dec",          "not",          "neg",
    "mul",          "imul",         "div",          "idiv",
    "rol",          "ror",          "rcl",          "rcr",
    "sal",          "shr",          "shl",          "sar",
    "fadd",         "fmul",         "fcom",         "fcomp",
    "fsub",         "fsubr",        "fdiv",         "fdivr",
    "fladd",        "flmul",        "flcom",        "flcomp",
    "flsub",        "flsubr",       "fldiv",        "fldivr",
    "fcmovb",       "fcmove",       "fcmovbe",      "fcmovu",
    "fcmovae",      "fcmovne",      "fcmova",       "fcmovnu",
    "nop",          "cwde",         "popf",         "pushf",
    "halt",         "lock",         "wait",         "leave",
    "cmc",          "clc",          "cld",          "cli",
    "stc",          "std",          "sti",          "retn",
    "retf",
    "syscall",      "sysenter",     "sysretn",      "sysexitn",
    "cpuid",        "cdqe",         "rsm",          "ud2",
    "emms",         "pause",        "invd",         "wbinvd",
    "wrmsr",        "rdmsr",        "rdpmc",        "rdtsc",
    "fnop",         "fchs",         "fsin",         "fcos",
    "fabs",         "ftst",         "fxam",         "fld1",
    "fldl2t",       "fldl2e",       "fldpi",        "fldlg2",
    "fldln2",       "fldz",         "f2xm1",        "fyl2x",
    "fptan",        "fpatan",       "fxtract",      "fprem1",
    "fdecstp",      "fincstp",      "fprem",        "fyl2xp1",
    "fsqrt",        "fsincos",      "frndint",      "fscale",
    "fcompp",       "fucompp",      "fninit",       "fnclex",
    "monitor",      "mwait",        "sysretf",      "sysexitf",
    "lfence",       "mfence",       "sfence",       "rdtscp",
    "finit",        "fclex",
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
    //~"lea",          "movbe",
    //~"test",         "ud2",          "xadd",         "xchg",
    "bt",           "bts",          "btr",          "btc",
    "bsf",          "bsr",          "bswap",
    "loop",         "loope",        "loopne",
    //~"rep",          "repe",         "repne",
    //~"ins",          "outs",         "lods",         "stos",
    //~"movs",         "cmps",         "scas"
};

#endif

static uint32_t   empty_count = 1; /// X: REMOVE GLOBAL VARIABLES.
static uint32_t   empty_holes = 1; /// X: MULTITHREAD? MODERNIZE?
static uint32_t * empty_array = NULL;
static uint32_t * empty_imbue = NULL;
static uint32_t * empty_store = NULL;

static bool front(uint32_t data) { return (data >= GR4) && (data <= GR7);  }
static bool upper(uint32_t data) { return (data >= GR8) && (data <= GR15); }
static bool lower(uint32_t data) { return (data <= GR7);                   }

// We don't use these yet, hardcoded.
static bool far (uint32_t label) { return label && 1; }
static bool near(uint32_t label) { return label && 0; }

static bool transfer(uint32_t to, uint32_t from) { return (to == REG) && (from == REG); }
static bool import  (uint32_t to, uint32_t from) { return (to == REG) && (from == MEM); }
static bool attach  (uint32_t to, uint32_t from) { return (to == REG) && (from == IMM); }
static bool export  (uint32_t to, uint32_t from) { return (to == MEM) && (from == REG); }
static bool assign  (uint32_t to, uint32_t from) { return (to == MEM) && (from == IMM); }
static bool relate  (uint32_t to, uint32_t from) { return (to == REG) && (from == REL); }

static uint32_t absolute(void) { return (0x004010b0 - text_sector_size - 4); }
static uint32_t relative(void) { return (0x00001000 - text_sector_size - 4); }
static uint32_t unsorted(void) { return (0x00000000 - text_sector_size - 4); }

static void replace(uint8_t * destination, /// X: THIS IS SILLY.
                    uint8_t * source,
                    size_t    size) {
    for (; size; --size) {
        destination[size - 1] = source[size - 1];
    }
}

static void inset(uint32_t when,
                  uint32_t data) {
    text_sector_byte[text_sector_size] = (uint8_t)data;

    text_sector_size += when;
}

static void inset_immediate(uint32_t when,
                            uint32_t size,
                            uint32_t data) {
    inset((when),                  (data >>  0) & 0xff);
    inset((when) && (size >= D16), (data >>  8) & 0xff);
    inset((when) && (size >= D32), (data >> 16) & 0xff);
    inset((when) && (size >= D32), (data >> 24) & 0xff);
}

static void inset_memory(uint32_t when,
                         uint32_t size,
                         uint32_t data,
                         uint32_t base) {
    empty_array[empty_holes] = text_sector_size;
    empty_imbue[empty_holes] = data;

    empty_holes += when;

    inset_immediate(when, size, base);
}

static void short_prefix(uint32_t size) {
    inset(size == D16, 0x66);
}

static void long_prefix(uint32_t size,
                        uint32_t to,
                        uint32_t destination,
                        uint32_t from,
                        uint32_t source) {
    const uint32_t long_destination = (to   == REG) && (upper(destination));
    const uint32_t long_source      = (from == REG) && (upper(source));
    const uint32_t long_size        = (size == D64);

    inset(long_destination || long_source || long_size,
          0x40 +
          0x01 * long_destination +
          0x04 * long_source +
          0x08 * long_size);
}

static void modify_memory(uint32_t code,
                          uint32_t to,
                          uint32_t from) {
    inset((export(to, from)) || (import(to, from)),
          0x05 +
          0x08 * code * (assign(to, from)));
}

static uint32_t mc0(uint32_t code, /// X: THIS IS PLAIN EVIL.
                    uint32_t base) {
    return (0xc0 +
            0x01 * (code % 8) +
            0x08 * (base % 8));
}

static uint32_t m05(uint32_t code) {
    return (0x05 + 0x08 * code);
}

static uint32_t store_relative(const uint32_t * restrict array) {
    const uint32_t relative = array[1];

    debug_print("@yasmrel@- %i", relative);

    empty_array[empty_holes] = text_sector_size;
    empty_imbue[empty_holes] = relative;

    ++empty_holes;

    return 1;
}

static uint32_t store_memory(const uint32_t * restrict array) {
    const uint32_t memory = array[1];

    debug_print("@yasmmem@- %i", memory);

    empty_store[memory] = text_sector_size;

    ++empty_count;

    return 1;
}

static uint32_t store_immediate(const uint32_t * restrict array) {
    const uint32_t size   = array[1];
    const uint32_t amount = array[2];

    debug_print("@yasmimm@- @b%s@- %i", size_name[size], amount);

    for (uint32_t index = 0; index < amount; ++index) {
        inset_immediate(true, size, array[3 + index]);
    }

    return amount + 2;
}

static uint32_t build_double(const uint32_t * restrict array) { /// X: ERROR PRONE.
    const uint32_t operation   = array[0];
    const uint32_t size        = array[1];
    const uint32_t to          = array[2];
    const uint32_t destination = array[3];
    const uint32_t from        = array[4];
    const uint32_t source      = array[5];

    debug_error(size > D64, "@rsize : double = %i;@-\n", size);
    debug_error(to   > MEM, "@rto   : double = %i;@-\n", to);
    debug_error(from > IMM, "@rfrom : double = %i;@-\n", from);

    debug_print("@y%s@- @b%s@- @c%s@- %i @c%s@- %i",
                operation_name[operation],
                size_name[size],
                operand_name[to],
                destination,
                operand_name[from],
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

    inset(transfer(to, from), mc0(destination, source));

    modify_memory(destination, to, from);
    modify_memory(source,      to, from);

    inset_memory(import(to, from), D32,  source, relative());

    inset_immediate(attach(to, from), size, source);

    inset_memory(export(to, from), D32, destination, relative());
    inset_memory(assign(to, from), D32, destination, relative());

    inset_immediate(assign(to, from), size, source);

    inset_memory(relate(to, from), D32, source, absolute());

    return 5;
}

static uint32_t build_single(const uint32_t * restrict array) { /// X: ERROR PRONE.
    const uint32_t operation   = array[0];
    const uint32_t size        = array[1];
    const uint32_t to          = array[2];
    const uint32_t destination = array[3];

    debug_error(size > D64, "@rsize : single = %i;@-\n", size);
    debug_error(to   > MEM, "@rto   : single = %i;@-\n", to);

    debug_print("@y%s@- @b%s@- @c%s@- %i",
                operation_name[operation],
                size_name[size],
                operand_name[to],
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

    inset_memory(to == MEM, D32, destination, relative());

    return 3;
}

static uint32_t build_static_1(const uint32_t * restrict array) {
    const uint32_t operation = array[0];

    const uint8_t data[STATIC_1_END - STATIC_1_BEGIN + 1] = {
        0x90, 0x98, 0x9d, 0x9c, 0xf4, 0xf0, 0x9b, 0xc9,
        0xf5, 0xf8, 0xfc, 0xfa, 0xf9, 0xfd, 0xfb, 0xc3,
        0xcb,
    };

    debug_print("@y%s@-", operation_name[operation]);

    inset(true, data[operation - STATIC_1_BEGIN]);

    return 0;
}

static uint32_t build_static_2(const uint32_t * restrict array) {
    const uint32_t operation = array[0];

    const uint16_t data[STATIC_2_END - STATIC_2_BEGIN + 1] = {
        0x050f, 0x340f, 0x070f, 0x350f, 0xa20f, 0x9848, 0xaa0f, 0x0b0f,
        0x770f, 0x90f3, 0x080f, 0x090f, 0x300f, 0x320f, 0x330f, 0x310f,
        0xd0d9, 0xe0d9, 0xfed9, 0xffd9, 0xe1d9, 0xe4d9, 0xe5d9, 0xe8d9,
        0xe9d9, 0xead9, 0xebd9, 0xecd9, 0xedd9, 0xeed9, 0xf0d9, 0xf1d9,
        0xf2d9, 0xf3d9, 0xf4d9, 0xf5d9, 0xf6d9, 0xf7d9, 0xf8d9, 0xf9d9,
        0xfad9, 0xfbd9, 0xfcd9, 0xfdd9, 0xd9de, 0xe9da, 0xe3db, 0xe2db,
    };

    debug_print("@y%s@-", operation_name[operation]);

    inset_immediate(true, D16, data[operation - STATIC_2_BEGIN]);

    return 0;
}

static uint32_t build_static_3(const uint32_t * restrict array) {
    const uint32_t operation = array[0];

    const uint8_t data[(STATIC_3_END - STATIC_3_BEGIN + 1) * 3] = {
        0x0f, 0x01, 0xc8,
        0x0f, 0x01, 0xc9,
        0x48, 0x0f, 0x07,
        0x48, 0x0f, 0x35,
        0x0f, 0xae, 0xe8,
        0x0f, 0xae, 0xf0,
        0x0f, 0xae, 0xf8,
        0x0f, 0x01, 0xf9,
        0x9b, 0xdb, 0xe3,
        0x9b, 0xdb, 0xe2,
    };

    debug_print("@y%s@-", operation_name[operation]);

    inset(true, data[(operation - STATIC_3_BEGIN) * 3 + 0]);
    inset(true, data[(operation - STATIC_3_BEGIN) * 3 + 1]);
    inset(true, data[(operation - STATIC_3_BEGIN) * 3 + 2]);

    return 0;
}

static uint32_t build_jump_if(const uint32_t * restrict array) {
    const uint32_t operation = array[0];
    const uint32_t size      = array[1];
    const uint32_t location  = array[3];

    debug_print("@y%s@- @b%s@- @crel@- %i",
                operation_name[operation],
                size_name[size],
                location);

    inset(far(location) && (size == D32), 0x0f);

    inset(far(location),  0x80 + operation - JUMP_IF_BEGIN);
    inset(near(location), 0x70 + operation - JUMP_IF_BEGIN);

    inset_memory(true, D32, location, unsorted());

    return 3;
}

static uint32_t build_move_if(const uint32_t * restrict array) {
    const uint32_t operation   = array[0];
    const uint32_t size        = array[1];
    const uint32_t to          = array[2];
    const uint32_t destination = array[3];
    const uint32_t from        = array[4];
    const uint32_t source      = array[5];

    debug_print("@y%s@- @b%s@- @c%s@- %i @c%s@- %i",
                operation_name[operation],
                size_name[size],
                operand_name[to],
                destination,
                operand_name[from],
                source);

    short_prefix(size);

    long_prefix(size, to, destination, from, source);

    inset(true, 0x0f);
    inset(true, 0x40 + operation - MOVE_IF_BEGIN);

    inset(transfer(to, from), mc0(destination, source));

    modify_memory(destination, to, from);

    return 5;
}

static uint32_t build_set_if(const uint32_t * restrict array) {
    const uint32_t operation   = array[0];
    const uint32_t to          = array[2];
    const uint32_t destination = array[3];

    debug_print("@y%s@- @bd8@- @c%s@- %i",
                operation_name[operation],
                operand_name[to],
                destination);

    inset((to == REG) && (front(destination)), 0x40);
    inset((to == REG) && (upper(destination)), 0x41);

    inset(true, 0x0f);
    inset(true, 0x90 + operation - SET_IF_BEGIN);

    inset(to == REG, 0xc0 + 0x01 * (destination & 7));
    inset(to == MEM, 0x05);

    inset_memory(to == MEM, D32, destination, relative());

    return 3;
}

static uint32_t build_jump(const uint32_t * restrict array) {
    const uint32_t size        = array[1];
    const uint32_t to          = array[2];
    const uint32_t destination = array[3];

    debug_print("@yjmp@- @b%s@- @c%s@- %i",
                size_name[size],
                operand_name[to],
                destination);

    inset((to == REG) && upper(destination), 0X41);

    inset(to == REL, 0xe9 + 0x02 * (size == D8));
    inset(to == REG, 0xff);
    inset(to == REG, 0xe0 + 0x01 * (destination & 7));
    inset(to == MEM, 0xff);
    inset(to == MEM, 0x25);

    inset_memory(to == REL, D32, destination, unsorted());
    inset_memory(to == MEM, D32, destination, 0x4010b0);

    return 3;
}

static uint32_t build_move(const uint32_t * restrict array) { /// X: ERROR PRONE.
    const uint32_t size        = array[1];
    const uint32_t to          = array[2];
    const uint32_t destination = array[3];
    const uint32_t from        = array[4];
    const uint32_t source      = array[5];
    const uint32_t extension   = array[6];

    debug_error(size > D64, "@rsize : move = %i;@-\n", size);
    debug_error(to   > MEM, "@rto   : move = %i;@-\n", to);
    debug_error(from > IMM, "@rfrom : move = %i;@-\n", from);

    debug_print("@ymov@- @b%s@- @c%s@- %i @c%s@- %i %i",
                size_name[size],
                operand_name[to],
                destination,
                operand_name[from],
                source,
                (size == D64) ? extension : 0);

    short_prefix(size);

    long_prefix(size, to, destination, from, source);

    inset((size == D8) && (front(destination)), 0x40);

    inset(transfer(to, from), 0x88 + 0x01 * (size != D8));
    inset(import(to, from), 0x8a + 0x01 * (size != D8));
    inset(export(to, from), 0x88 + 0x01 * (size != D8));

    modify_memory(destination, to, from);
    modify_memory(source,      to, from);

    inset(transfer(to, from), mc0(destination, source));

    inset((to == REG) && ((from == IMM) || (from == REL)), 0xb0 +
          0x08 * (size != D8) + 0x01 * (destination & 7));

    inset(assign(to, from), 0xc6 + 0x01 * (size != D8));
    inset(assign(to, from), 0x05);

    inset_memory(import(to, from), D32, source,      relative());
    inset_memory(export(to, from), D32, destination, relative());
    inset_memory(assign(to, from), D32, destination, relative());
    inset_memory(relate(to, from), D32, source,      0x4010b0);

    inset_immediate(attach(to, from) && (size <= D32), size, source);
    inset_immediate(assign(to, from) && (size <= D32), size, source);

    inset_immediate(attach(to, from) && (size == D64), D32, source);
    inset_immediate(attach(to, from) && (size == D64), D32, extension);
    inset_immediate(attach(to, from) && (size == D64), D32, 0);

    return 5 + (size == D64);
}

static uint32_t build_call(const uint32_t * restrict array) {
    const uint32_t from   = array[1];
    const uint32_t source = array[2];

    debug_print("@ycall@- @c%s@- %i", operand_name[from], source);

    inset((from == REG) && (upper(source)), 0x41);

    inset(from == REL, 0xe8);
    inset(from == REG, 0xff);

    inset_memory(from == REL, D32, source, unsorted());

    inset(from == REG, (0xd0 + 0x01 * (source & 7)));

    return 2;
}

static uint32_t build_enter(const uint32_t * restrict array) {
    const uint32_t dynamic_storage = array[1];
    const uint32_t nesting_level   = array[2];

    debug_print("@yenter@- %i %i", dynamic_storage, nesting_level);

    inset(true, 0xc8);

    inset_immediate(true, D16, dynamic_storage);
    inset_immediate(true, D8,  nesting_level & 0x1f);

    return 2;
}

static uint32_t build_shift(const uint32_t * restrict array) {
    const uint32_t operation   = array[0];
    const uint32_t size        = array[1];
    const uint32_t to          = array[2];
    const uint32_t destination = array[3];
    const uint32_t offset      = array[5];

    debug_print("@y%s@- @b%s@- @c%s@- %i @cimm@- %i",
                operation_name[operation],
                size_name[size],
                operand_name[to],
                destination,
                offset);

    short_prefix(size);

    long_prefix(size, to, destination, 0, 0);

    inset(true, 0xc0 + 0x01 * (size != D8));

    inset(to == REG, 0x05 + 0x08 * ((operation - SHIFT_BEGIN) & 7));
    inset(to == MEM, 0xc0 + 0x08 * ((operation - SHIFT_BEGIN) & 7));

    inset_memory(to == MEM, D32, destination, relative());

    inset_immediate(true, D8, offset);

    return 5;
}

static uint32_t build_float(const uint32_t * restrict array) { /// X: UNCHECKED.
    const uint32_t operation = array[0];
    const uint32_t size      = array[1];
    const uint32_t from      = array[2];
    const uint32_t source    = array[3]; /// X: REMOVE UNUSED STUFF.

    debug_print("@y%s@- @b%s@- @c%s@- %i",
                operation_name[operation],
                size_name[size],
                operand_name[from],
                source);

    inset(from == MEM, 0xd8 + 0x04 * (size == D64));

    modify_memory(operation - FLOAT_BEGIN, 0, from);

    inset_memory(from == MEM, size, source, 0);

    return 3;
}

static void m072445(uint32_t fuck) { /// THIS IS HORRIBLE.
    if (fuck == 4) {
        inset(true, 0x04);
        inset(true, 0x24);
    } else if (fuck == 5) {
        inset(true, 0x45);
        inset(true, 0x00);
    } else {
        inset(true, fuck & 7);
    }
}

static uint32_t build_floatl(const uint32_t * restrict array) { /// X: UNCHECKED.
    const uint32_t operation = array[0];
    const uint32_t size      = array[1];
    const uint32_t type      = array[2];
    const uint32_t from      = array[3];
    const uint32_t source    = array[4];

    // FLADD D32 INT DER 1 -> fiadd dword[eax]
    // FLADD D32 BCF DER 1 -> fadd  dword[eax]
    // FLADD D16 INT MEM # -> fiadd  word[x]
    // FLADD D32 INT MEM # -> fiadd dword[x]
    // FLADD D32 BCF MEM # -> fadd  dword[x]
    // FLADD D64 BCF MEM # -> fadd  qword[x]

    debug_print("@y%s@- @b%s@- @c%s@- @c%s@- %i",
                operation_name[operation],
                size_name[size],
                operand_name[type],
                operand_name[from],
                source);

    inset((from == DER) && (size == D32), 0x67);

    inset((from == DER) && (upper(source)), 0x41);

    inset((type == BCF) && (size == D32), 0xd8);
    inset((type == INT) && (size == D32), 0xda);
    inset((type == BCF) && (size == D64), 0xdc);
    inset((type == INT) && (size == D16), 0xde);

    if (from == DER) m072445(source); /// KILL THIS WITH FIRE.

    inset_memory(from == MEM, D32, source, relative());

    return 4;
}

static uint32_t build_fmove_if(const uint32_t * restrict array) { /// X: UNCHECKED.
    const uint32_t operation = array[0];
    const uint32_t source    = array[1];
    const uint32_t offset    = operation - FMOVE_IF_BEGIN;

    debug_print("@y%s@- @cst@- 0 @cst@- %i",
                operation_name[operation],
                source);

    debug_error (source > 7, "@r%s ! source %i > 7@-\n",
                             operation_name[operation],
                             source);

    inset(operation < FCMOVAE, 0xda);
    inset(operation > FCMOVU,  0xdb);

    inset(operation < FCMOVAE, 0xc0 + 0x08 * (offset - 0) + source);
    inset(operation > FCMOVU,  0xc0 + 0x08 * (offset - 4) + source);

    return 1;
}

static uint32_t build_in_out(const uint32_t * restrict array) {
    const uint32_t move = array[0];
    const uint32_t size = array[1];
    const uint32_t type = array[2];
    const uint32_t port = array[3];

    debug_print("@y%s@- @b%s@- @c%s@- %i",
                operation_name[move],
                size_name[size],
                operand_name[type],
                port);

    short_prefix(size);

    // Shorten and extend, I think we're not covering all cases.
    inset(true, 0xe4 +
             0x01 * (size != D8) +
             0x02 * (move == IN) +
             0x08 * (type == REG));

    inset_immediate(type == IMM, D8, port);

    return 3;
}

static uint32_t build_pop(const uint32_t * restrict array) {
    const uint32_t size        = array[1];
    const uint32_t to          = array[2];
    const uint32_t destination = array[3];

    debug_print("@ypop@- @b%s@- @c%s@- %i",
                size_name[size],
                operand_name[to],
                destination);

    short_prefix(size);

    inset((to == REG) && (upper(destination)), 0x41);

    inset(to == REG, 0x58 + 0x01 * (destination & 7));
    inset(to == MEM, 0x8f);
    inset(to == MEM, 0x05);

    inset_memory(to == MEM, D32, destination, 0);

    return 3;
}

static uint32_t build_push(const uint32_t * restrict array) {
    const uint32_t size   = array[1];
    const uint32_t from   = array[2];
    const uint32_t source = array[3];

    debug_print("@ypush@- @b%s@- @c%s@- %i",
                size_name[size],
                operand_name[from],
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

static uint32_t build_swap(const uint32_t * restrict array) {
    const uint32_t size        = array[1];
    const uint32_t destination = array[3];

    debug_print("@yswap@- @b%s@- @creg@- %i",
                size_name[size],
                destination);

    long_prefix(size, REG, destination, 0, 0);

    inset(true, 0x0f);
    inset(true, 0xc8 + 0x01 * (destination & 7));

    return 3;
}

static uint32_t build_bit_scan(const uint32_t * restrict array) {
    const uint32_t operation   = array[0];
    const uint32_t size        = array[1];
    const uint32_t destination = array[3];
    const uint32_t from        = array[4];
    const uint32_t source      = array[5];

    debug_print("@y%s@- @b%s@- @creg@- %i @c%s@- %i",
                operation_name[operation],
                size_name[size],
                destination,
                operand_name[from],
                source);

    short_prefix(size);

    long_prefix(size, REG, destination, from, source);

    inset_immediate(true, D16, 0xbc0f + 0x100 * (operation == BSR));

    inset(transfer(REG, from), mc0(destination, source));

    inset(from == MEM, 0x05 + 0x08 * destination);

    inset_memory(from == MEM, D32, source, relative());

    return 5;
}

static uint32_t build_bit_test(const uint32_t * restrict array) {
    const uint32_t operation   = array[0];
    const uint32_t size        = array[1];
    const uint32_t to          = array[2];
    const uint32_t destination = array[3];
    const uint32_t from        = array[4];
    const uint32_t source      = array[5];

    const uint32_t offset = operation - BT;

    debug_error(size > D64, "@rsize : bit test = %i;@-\n", size);
    debug_error(to   > MEM, "@rto   : bit test = %i;@-\n", to);
    debug_error(from > IMM, "@rfrom : bit test = %i;@-\n", from);

    debug_error(size   == D8,  "@rbit test ! size D8@-\n");
    debug_error(from   == MEM, "@rbit test ! from MEM@-\n");
    debug_error(source >= 64,  "@rbit test ! source %i@-\n", source);

    debug_print("@y%s@- @b%s@- @c%s@- %i @c%s@- %i",
                operation_name[operation],
                size_name[size],
                operand_name[to],
                destination,
                operand_name[from],
                source);

    short_prefix(size);

    long_prefix(size, to, destination, from, source);

    inset(true, 0x0f);

    inset(attach  (to, from) || assign(to, from), 0xba);
    inset(transfer(to, from) || export(to, from), 0xa3 + 0x08 * offset);

    inset (export  (to, from), m05(to));
    inset (assign  (to, from), m05(4 + offset));
    inset (attach  (to, from), mc0(destination, 4 + offset));
    inset (transfer(to, from), mc0(destination, source));

    inset_memory(from == MEM, D32, source, relative());

    inset(from == IMM, source);

    return 5;
}

static uint32_t build_loop(const uint32_t * restrict array) {
    const uint32_t operation = array[0];
    const uint32_t location  = array[3];

    debug_print("@y%s@- @bd8@- @crel@- %i",
                operation_name[operation],
                location);

    inset(operation == LOOPNE, 0xe0);
    inset(operation == LOOPE,  0xe1);
    inset(operation == LOOP,   0xe2);

    inset_memory(true, D8, location, -text_sector_size - 1);

    return 3;
}

static uint32_t (*build_instruction[OPERATION_END])(const uint32_t * restrict array) = {
    store_memory,   // ASMDIRMEM : LABEL
    store_relative, // ASMDIRREL : "IMPLEMENTED"
    store_immediate,// ASMDIRIMM : LITERAL
    NULL,           // ASMDIRREP : UNIMPLEMENTED
    build_double,   build_double,   build_double,   build_double,
    build_double,   build_double,   build_double,   build_double,
    build_single,   build_single,   build_single,   build_single,
    build_single,   build_single,   build_single,   build_single,
    build_shift,    build_shift,    build_shift,    build_shift,
    build_shift,    build_shift,    build_shift,    build_shift,
    build_float,    build_float,    build_float,    build_float,
    build_float,    build_float,    build_float,    build_float,
    build_floatl,   build_floatl,   build_floatl,   build_floatl,
    build_floatl,   build_floatl,   build_floatl,   build_floatl,
    build_fmove_if, build_fmove_if, build_fmove_if, build_fmove_if,
    build_fmove_if, build_fmove_if, build_fmove_if, build_fmove_if,
    build_static_1, build_static_1, build_static_1, build_static_1,
    build_static_1, build_static_1, build_static_1, build_static_1,
    build_static_1, build_static_1, build_static_1, build_static_1,
    build_static_1, build_static_1, build_static_1, build_static_1,
    build_static_1,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_2, build_static_2, build_static_2, build_static_2,
    build_static_3, build_static_3, build_static_3, build_static_3,
    build_static_3, build_static_3, build_static_3, build_static_3,
    build_static_3, build_static_3,
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
    //~NULL,           NULL,           NULL,           NULL,
    //~NULL,           NULL,           NULL,           NULL,
    //~NULL,           NULL,           NULL,           NULL,
    //~NULL,           NULL,           NULL,           NULL,
    //~LEA,            CWDE,           CDQE,
    //~CMC,            CLC,            CLD,            CLI,
    //~MOVBE,          STC,            STD,            STI,
    //~TEST,           UD2,            XADD,           XCHG,
    build_bit_test, build_bit_test, build_bit_test, build_bit_test,
    build_bit_scan, build_bit_scan, build_swap,
    build_loop,     build_loop,     build_loop,
    //~NULL,           NULL,           NULL,
    //~NULL,           NULL,           NULL,           NULL,
    //~NULL,           NULL,           NULL
    //~REP,            REPE,           REPNE,
    //~INS,            OUTS,           LODS,           STOS,
    //~MOVS,           CMPS,           SCAS
};

uint32_t   main_entry_point = 0;
uint32_t   text_sector_size = 0;
uint8_t  * text_sector_byte = NULL;
uint32_t   data_sector_size = 0;    // This is unused, and it should be used...
uint8_t  * data_sector_byte = NULL; // This is unused, and it should be used...

bool was_instruction_array_empty = false;

bool assemble(      uint32_t            count,
              const uint32_t * restrict array) {
    if ((!count) || (!array)) {
        was_instruction_array_empty = true;
        return false;
    }

    empty_array = aalloc(1024ul * sizeof(*empty_array)); /// X: STUPID
    empty_imbue = aalloc(1024ul * sizeof(*empty_imbue));
    empty_store = aalloc(1024ul * sizeof(*empty_store));

    for (uint32_t index = 0; index < count; ++index) {
        const uint32_t size = text_sector_size;

#if DEBUG == 1
        inset(array[index] > ASMDIRREP, 0x90);
#endif

        index += build_instruction[array[index]](&array[index]);

#if DEBUG == 1
        debug_print(" @a--@- ");
        for (uint32_t byte = size; byte < text_sector_size; ++byte) {
            debug_print("@p%02X@- ", (uint8_t)text_sector_byte[byte]);
        }
        debug_print("\n");
#endif
    }

    main_entry_point = empty_store[0];

    for (uint32_t index = 1; index < empty_holes; ++index) {
        uint32_t set = 0;
        uint32_t get = empty_array[index];

        replace((uint8_t*)&set, &text_sector_byte[get], sizeof(set));

        set += empty_store[empty_imbue[index]];

        replace(&text_sector_byte[get], (uint8_t*)&set, sizeof(get));
    }

    return true;
}
