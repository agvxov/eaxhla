#include <stdio.h>

static char * fa [] = {
	"add", "or", "adc", "sbb", "and", "sub", "xor", "cmp"
};

static char * faa [] = {
	"inc", "dec", "not", "neg", "mul", "imul", "div", "idiv"
};

static char * fr [] = {
	"rax",    "rcx",    "rdx",    "rbx",    "rsp",    "rbp",    "rsi",    "rdi",
	"r8",     "r9",     "r10",    "r11",    "r12",    "r13",    "r14",    "r15",
	"eax",    "ecx",    "edx",    "ebx",    "esp",    "ebp",    "esi",    "edi",
	"r8d",    "r9d",    "r10d",   "r11d",   "r12d",   "r13d",   "r14d",   "r15d",
	"ax",     "cx",     "dx",     "bx",     "sp",     "bp",     "si",     "di",
	"r8w",    "r9w",    "r10w",   "r11w",   "r12w",   "r13w",   "r14w",   "r15w",
	"al",     "cl",     "dl",     "bl",     "spl",    "bpl",    "sil",    "dil",
	"r8b",    "r9b",    "r10b",   "r11b",   "r12b",   "r13b",   "r14b",   "r15b"
};

static char * fm [] = {
	"byte[b]", "word[w]", "dword[d]", "qword[q]",
};

static char * fi [] = {
	"11223344h", "11223344h", "1122h", "11h",  // "1122334455667788h",
};

int main (void) {
	int a, w, t, d, f, s;

	printf ("format ELF64 executable 3\n");
	printf ("segment readable executable\n");
	printf ("entry $\n");
	printf ("nop\n");
	printf ("nop\n");
	printf ("nop\n");

	// INR REG REG
	//~for (a = 0; a < (int) (sizeof (faa) / sizeof (* faa)); ++a) {
		//~for (w = 0; w < 4; ++w) {
			//~for (d = 0; d < 16; ++d) {
				//~printf ("nop\n");
				//~printf ("%s %s\n", faa [a], fr [d + 16 * w]);
			//~}
		//~}
	//~}

	// INR REG IMM
	for (a = 0; a < (int) (sizeof (fa) / sizeof (* fa)); ++a) {
		for (d = 0; d < 64; ++d) {
			printf ("nop\n");
			printf ("%s %s, %s\n", fa [a], fr [d], fi [d / 16]);
		}
	}

	// INI REG
	for (a = 0; a < (int) (sizeof (faa) / sizeof (* faa)); ++a) {
		for (w = 0; w < 4; ++w) {
			for (d = 0; d < 16; ++d) {
				printf ("nop\n");
				printf ("%s %s\n", faa [a], fr [d + 16 * w]);
			}
		}
	}

	printf ("nop\n");
	printf ("nop\n");
	printf ("nop\n");
	printf ("segment readable writable\n");
	printf ("b: db 0\n");
	printf ("w: dw 0\n");
	printf ("d: dd 0\n");
	printf ("q: dq 0\n");

	return (0);
}
