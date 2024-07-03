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

typedef signed   int  form;
typedef unsigned int  next;
typedef unsigned char byte;

static next   token_count;
static byte * token_array;

static void place (byte data) {
	token_array [token_count] = data;

	token_count += 1;
}

static form valid (form data) { return ((data >= 0) && (data <= 15)); }
static form lower (form data) { return ((data >= 0) && (data <=  7)); }
static form upper (form data) { return ((data >= 8) && (data <= 15)); }

static void build_short_prefix (void) {
	place (0X66);
}

static void build_long_prefix (size_index size,
                               type_index to,
                               form       destination,
                               type_index from,
                               form       source) {
	place ((byte) (0X40
	      + 0X01 * ((to   == REG) && (upper (destination)))
	      + 0X04 * ((from == REG) && (upper (source)))
	      + 0X08 *  (size == D64)));
}

static void build_register_direction (form destination,
                                      form source) {
	place ((byte) (0XC0 + 0X01 * (destination % 8) + 0X08 * (source % 8)));
}

static void build_register_redirection (form direction) {
	place ((byte) (0X05 + 0X08 * (direction % 8)));
}

static void build_constant (size_index size) {
	place ((byte) (0X80 + 0X01 * (size != D8)));
}

static void build_regular_instruction (byte       format,
                                       size_index size,
                                       type_index to,
                                       form       destination,
                                       type_index from) {
	place ((byte) (format
	      + destination % 8 * ((to == REG) && (from == IMM))
	      + 0X01 *  (size != D8)
	      + 0X02 * ((from == MEM) && (to == REG))
	      + 0X04 * ((from == IMM) && (to == MEM))
	      + 0XC0 * ((from == IMM) && (to == REG))));
}

static void assemble_enter (form dynamic_storage,
                            form nesting_level) {
	place (0XC8);
	place ((byte) (dynamic_storage /   1) % 256); /* FIX LATER */
	place ((byte) (dynamic_storage / 256) % 256);
	place ((byte) (nesting_level   /   1) % 256);
}

/* EXPERIMENTAL CODE */

static void add      (void) { place (0XFF); } /* REGULAR */
static void or       (void) { place (0XFF); }
static void adc      (void) { place (0XFF); }
static void sbb      (void) { place (0XFF); }
static void and      (void) { place (0XFF); }
static void sub      (void) { place (0XFF); }
static void xor      (void) { place (0XFF); }
static void cmp      (void) { place (0XFF); }
static void umul     (void) { place (0XFF); } /* IRREGULAR */
static void udiv     (void) { place (0XFF); } /* STDLIB CANCER FIX */
static void imul     (void) { place (0XFF); }
static void idiv     (void) { place (0XFF); }
static void inc      (void) { place (0XFF); }
static void dec      (void) { place (0XFF); }
static void not      (void) { place (0XFF); }
static void neg      (void) { place (0XFF); }
static void enter    (void) { place (0XFF); }
static void leave    (void) { place (0XC9); }
static void call     (void) { place (0XFF); }
static void ret      (void) { place (0XC3); } /* NEAR, NOT FAR (INTERSEGMENT) */
static void sysenter (void) { place (0X0F); place (0X34); }
static void sysexit  (void) { place (0X0F); place (0X35); }
static void syscall  (void) { place (0X0F); place (0X05); }
static void sysret   (void) { place (0X0F); place (0X07); }
static void jmp      (void) { place (0XFF); }
static void jpe      (void) { place (0XFF); }
static void js       (void) { place (0XFF); }
static void jpo      (void) { place (0XFF); }
static void je       (void) { place (0XFF); }
static void jne      (void) { place (0XFF); }
static void jz       (void) { place (0XFF); }
static void jnz      (void) { place (0XFF); }
static void ja       (void) { place (0XFF); }
static void jna      (void) { place (0XFF); }
static void jb       (void) { place (0XFF); }
static void jnb      (void) { place (0XFF); }
static void mov      (void) { place (0XFF); } /* EXTEND */
static void cmovpe   (void) { place (0XFF); }
static void cmovs    (void) { place (0XFF); }
static void cmovpo   (void) { place (0XFF); }
static void cmove    (void) { place (0XFF); }
static void cmovne   (void) { place (0XFF); }
static void cmovz    (void) { place (0XFF); }
static void cmovnz   (void) { place (0XFF); }
static void cmova    (void) { place (0XFF); }
static void cmovna   (void) { place (0XFF); }
static void cmovb    (void) { place (0XFF); }
static void cmovnb   (void) { place (0XFF); }
static void lock     (void) { place (0XF0); }
static void hlt      (void) { place (0XF4); }
static void in       (void) { place (0XFF); }
static void out      (void) { place (0XFF); }
static void push     (void) { place (0XFF); }
static void pop      (void) { place (0XFF); }
static void bswap    (void) { place (0XFF); }
static void test     (void) { place (0XFF); }
static void rcl      (void) { place (0XFF); }
static void rcr      (void) { place (0XFF); }
static void rol      (void) { place (0XFF); }
static void ror      (void) { place (0XFF); }
static void shl      (void) { place (0XFF); }
static void shr      (void) { place (0XFF); }
static void sal      (void) { place (0XFF); }
static void sar      (void) { place (0XFF); }
static void rep      (void) { place (0XFF); }
static void repe     (void) { place (0XFF); }
static void repne    (void) { place (0XFF); }
static void repz     (void) { place (0XFF); }
static void loop     (void) { place (0XFF); }
static void loope    (void) { place (0XFF); }
static void loopne   (void) { place (0XFF); }
static void pause    (void) { place (0XF3); place (0X90); }
static void xadd     (void) { place (0XFF); }
static void xchg     (void) { place (0XFF); }
static void lea      (void) { place (0XFF); }
static void popcnt   (void) { place (0XFF); }
static void inti     (void) { place (0XFF); }
static void bsf      (void) { place (0XFF); }
static void bsr      (void) { place (0XFF); }
static void bound    (void) { place (0XFF); }
static void fadd     (void) { place (0XFF); }
static void fsub     (void) { place (0XFF); }
static void fmul     (void) { place (0XFF); }
static void fdiv     (void) { place (0XFF); }
static void fnop     (void) { place (0XFF); }
static void fxam     (void) { place (0XFF); }
static void fabs     (void) { place (0XFF); }
static void fscale   (void) { place (0XFF); }
static void fsin     (void) { place (0XFF); }
static void fcos     (void) { place (0XFF); }
static void fsqrt    (void) { place (0XFF); }
static void fchs     (void) { place (0XFF); }
static void fxch     (void) { place (0XFF); }
static void frem     (void) { place (0XFF); }
static void fldpi    (void) { place (0XFF); }
static void fldz     (void) { place (0XFF); }
static void cpuid    (void) { place (0X0F); place (0XA2); }

static void assemble (operation_index operation,
                      size_index      size,
                      type_index      to,
                      form            destination,
                      type_index      from,
                      form            source) {
	byte code = 0X00;

	if (size == D16) {
		build_short_prefix ();
	}

	if ((size == D64)
	|| ((to   == REG) && (upper (destination)))
	|| ((from == REG) && (upper (source)))) {
		build_long_prefix (size, to, destination, from, source);
	}

	if (from == IMM) {
		build_constant (size);
	}

	if (operation <= CMP) {
		code = (byte) (0X08 * operation); /* FIX LATER */
	}

	build_regular_instruction (code, size, to, destination, from);

	if ((to == REG) && (from == REG)) {
		build_register_direction (destination, source);
	}

	if ((to == REG) && (from == MEM)) {
		build_register_redirection (destination);
	}

	if ((to == MEM) && (from == REG)) {
		build_register_redirection (source);
	}
}
