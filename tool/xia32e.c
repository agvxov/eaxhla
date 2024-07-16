/*
XX   XX   OOOOO   LL        AAAAA   TTTTTTTT  II  LL       EEEEEEE
 XX XX   OO   OO  LL       AA   AA     TT     II  LL       EE
  XXX    OO   OO  LL       AAAAAAA     TT     II  LL       EEEEE
 XX XX   OO   OO  LL       AA   AA     TT     II  LL       EE
XX   XX   OOOOO   LLLLLLL  AA   AA     TT     II  LLLLLLL  EEEEEEE
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "../source/assembler.c"
#include "../source/unix.c"

enum {
	token_add,          token_or,           token_adc,          token_sbb,          token_and,          token_sub,          token_xor,          token_cmp,
	token_inc,          token_dec,          token_not,          token_neg,          token_mul,          token_imul,         token_div,          token_idiv,
	token_nop,          token_retn,         token_retf,         token_leave,        token_lock,         token_hlt,
	token_sysenter,     token_sysexit,      token_syscall,      token_sysret,       token_pause,        token_cpuid,
	token_enter,        token_call,         token_in,           token_out,
	token_jmp,
	token_jo,           token_jno,          token_jb,           token_jae,          token_je,           token_jne,          token_jbe,          token_ja,
	token_js,           token_jns,          token_jpe,          token_jpo,          token_jl,           token_jge,          token_jle,          token_jg,
	token_mov,
	token_cmovo,        token_cmovno,       token_cmovb,        token_cmovae,       token_cmove,        token_cmovne,       token_cmovbe,       token_cmova,
	token_cmovs,        token_cmovns,       token_cmovpe,       token_cmovpo,       token_cmovl,        token_cmovge,       token_cmovle,       token_cmovg,
	token_push,         token_pop,          token_bswap,        token_test,
	token_rcl,          token_rcr,          token_rol,          token_ror,          token_shl,          token_shr,          token_sal,          token_sar,
	token_rep,          token_repe,         token_repne,        token_repz,
	token_loop,         token_loope,        token_loopne,       token_movbe,        token_xadd,         token_xchg,         token_lea,          token_popcnt,
	token_inti,         token_bsf,          token_bsr,          token_bound,
	token_fadd,         token_fsub,         token_fmul,         token_fdiv,         token_fnop,         token_fxam,         token_fabs,         token_fscale,
	token_fsin,         token_fcos,         token_fsqrt,        token_fchs,         token_fxch,         token_frem,         token_fldpi,        token_fldz,
	token_al,           token_cl,           token_dl,           token_bl,           token_spl,          token_bpl,          token_sil,          token_dil,
	token_r8b,          token_r9b,          token_r10b,         token_r11b,         token_r12b,         token_r13b,         token_r14b,         token_r15b,
	token_ax,           token_cx,           token_dx,           token_bx,           token_sp,           token_bp,           token_si,           token_di,
	token_r8w,          token_r9w,          token_r10w,         token_r11w,         token_r12w,         token_r13w,         token_r14w,         token_r15w,
	token_eax,          token_ecx,          token_edx,          token_ebx,          token_esp,          token_ebp,          token_esi,          token_edi,
	token_r8d,          token_r9d,          token_r10d,         token_r11d,         token_r12d,         token_r13d,         token_r14d,         token_r15d,
	token_rax,          token_rcx,          token_rdx,          token_rbx,          token_rsp,          token_rbp,          token_rsi,          token_rdi,
	token_r8,           token_r9,           token_r10,          token_r11,          token_r12,          token_r13,          token_r14,          token_r15,
	token_db,           token_dw,           token_dd,           token_dq,           token_end,          token_label,
	token_count
};

static char buffer [1024];

static unsigned int middle_amount;
static unsigned int middle_buffer [1024];

static          int  label_amount;
static unsigned long label_buffer [1024];

static unsigned long token_leq  [token_count];
static          int  token_size [token_count];

static char * token_string [token_count] = {
	"add",              "or",               "adc",              "sbb",              "and",              "sub",              "xor",              "cmp",
	"inc",              "dec",              "not",              "neg",              "mul",              "imul",             "div",              "idiv",
	"nop",              "retn",             "retf",             "leave",            "lock",             "hlt",
	"sysenter",         "sysexit",          "syscall",          "sysret",           "pause",            "cpuid",
	"enter",            "call",             "in",               "out",
	"jmp",
	"jo",               "jno",              "jb",               "jae",              "je",               "jne",              "jbe",              "ja",
	"js",               "jns",              "jpe",              "jpo",              "jl",               "jge",              "jle",              "jg",
	"mov",
	"cmovo",            "cmovno",           "cmovb",            "cmovae",           "cmove",            "cmovne",           "cmovbe",           "cmova",
	"cmovs",            "cmovns",           "cmovpe",           "cmovpo",           "cmovl",            "cmovge",           "cmovle",           "cmovg",
	"push",             "pop",              "bswap",            "test",
	"rcl",              "rcr",              "rol",              "ror",              "shl",              "shr",              "sal",              "sar",
	"rep",              "repe",             "repne",            "repz",
	"loop",             "loope",            "loopne",           "movbe",            "xadd",             "xchg",             "lea",              "popcnt",
	"inti",             "bsf",              "bsr",              "bound",
	"fadd",             "fsub",             "fmul",             "fdiv",             "fnop",             "fxam",             "fabs",             "fscale",
	"fsin",             "fcos",             "fsqrt",            "fchs",             "fxch",             "frem",             "fldpi",            "fldz",
	"al",               "cl",               "dl",               "bl",               "spl",              "bpl",              "sil",              "dil",
	"r8b",              "r9b",              "r10b",             "r11b",             "r12b",             "r13b",             "r14b",             "r15b",
	"ax",               "cx",               "dx",               "bx",               "sp",               "bp",               "si",               "di",
	"r8w",              "r9w",              "r10w",             "r11w",             "r12w",             "r13w",             "r14w",             "r15w",
	"eax",              "ecx",              "edx",              "ebx",              "esp",              "ebp",              "esi",              "edi",
	"r8d",              "r9d",              "r10d",             "r11d",             "r12d",             "r13d",             "r14d",             "r15d",
	"rax",              "rcx",              "rdx",              "rbx",              "rsp",              "rbp",              "rsi",              "rdi",
	"r8",               "r9",               "r10",              "r11",              "r12",              "r13",              "r14",              "r15",
	"db",               "dw",               "dd",               "dq",               "end",              "."
};

static void leq_string (char * string, unsigned long * leq, int * size) {
	* leq  = 0;
	* size = 0;

	do {
		if ((* string == ' ') || (* string == '\t') || (* string == '\n') || (* string == '\r')) break;

		* leq  <<= 8UL;
		* leq   |= (unsigned long) (* string);
		* size += 1;

		++string;
	} while (* string != '\0');
}

static void input (void) {
	int index = 0;

	do {
		read (STDIN_FILENO, & buffer [index], sizeof (* buffer));

		if (buffer [index] == '\0') break;
	} while (++index != 1024);

	printf ("%u\n", index);
}

static void labelize (unsigned long word) {
	label_buffer [label_amount] = word;

	++label_amount;
}

static void tokenize (int token) {
	middle_buffer [middle_amount] = token;

	++middle_amount;
}

int main (void) {
	unsigned long word   = 0;
	         int  offset = 0;
	         int  size   = 0;
	         int  index  = 0;
	         int  scope  = 0;

	int scope_instruction = 0;
	int scope_directive   = 0;
	int scope_label       = 0;

	do {
		leq_string (token_string [index], & token_leq [index], & token_size [index]);
	} while (++index < token_count);

	input ();

	printf ("%s\n", buffer);

	do {
		leq_string (& buffer [offset], & word, & size);

		index = 0;

		do {
			if (word == token_leq [index]) break;
		} while (++index != token_count);

		offset += size + 1;

		printf ("%016lx : %.*s : %3i : [%s]\n", word, size, & buffer [offset - size - 1], index, (index != token_count) ? token_string [index] : "--");

		if (index == token_label) {
			tokenize (ASMDIRMEM);
			scope_label = 1;
		} else if ((index >= token_add) && (index <= token_fldz)) {
			tokenize (index + ADD);
			scope_instruction = 1;
		} else if ((index >= token_al) && (index <= token_r15)) {
			if (scope_instruction) {
				tokenize ((index - token_al) / 16);
			}
			tokenize (REG);
			tokenize ((index - token_al) % 16);
		} else if ((index >= token_db) && (index <= token_dq)) {
			tokenize (ASMDIRIMM);
			tokenize (index - token_db);
			scope_directive = 1;
		} else if (index == token_count) {
			if (isdigit (buffer [offset - size - 1]) != 0) {
				if (scope_directive) {
					scope = atoi (& buffer [offset - size - 1]);
					tokenize (scope);
					do {
						leq_string (& buffer [offset], & word, & size);
						index = 0;
						do {
							if (word == token_leq [index]) break;
						} while (++index != token_count);
						offset += size + 1;
						tokenize (atoi (& buffer [offset - size - 1]));
					} while (--scope != 0);
					scope_directive = 0;
				} else {
					buffer [offset - 1] = '\0';
					tokenize (IMM);
					tokenize (atoi (& buffer [offset - size - 1]));
				}
			} else if (isalpha (buffer [offset - size - 1]) != 0) {
				if (scope_label) {
					tokenize (label_amount);
					labelize (word);
					scope_label = 0;
				} else if (scope_instruction) {
					tokenize (REL);
					scope = 0;
					do {
						if (label_buffer [scope] == word) break;
					} while (scope < label_amount);
					tokenize (scope);
					scope_instruction = 0;
				}
			} else {
				tokenize (12345678);
			}
		}
	} while (index != token_end);

	for (index = 0; index < middle_amount; ++index) printf ("%i\n", middle_buffer [index]);

	text_sector_byte = calloc (1440UL, sizeof (* text_sector_byte));

	assemble (middle_amount, middle_buffer);

	elf_main_header (1, 1, 1, 0);
	elf_text_sector (text_sector_size - 5);
	elf_data_sector (text_sector_size - 5, 5);

	FILE * file = fopen ("a.out", "w");

	fwrite (elf_main_header_byte, 1UL, ELF_MAIN_HEADER_SIZE, file);
	fwrite (elf_text_sector_byte, 1UL, ELF_TEXT_SECTOR_SIZE, file);
	fwrite (elf_data_sector_byte, 1UL, ELF_DATA_SECTOR_SIZE, file);

	fwrite (text_sector_byte, sizeof (* text_sector_byte), (size_t) text_sector_size, file);

	system ("chmod +x a.out");

	fclose (file);

	free (text_sector_byte);

	return (0);
}
