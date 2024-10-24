#include <stdio.h>

static char * fw [] = {
	"D8", "D16", "D32", "D64"
};

static char * fa [] = {
	"ADD", "OR", "ADC", "SBB", "AND", "SUB", "XOR", "CMP"
};

static char * faa [] = {
	"INC", "DEC", "NOT", "NEG", "UMUL", "IMUL", "UDIV", "IDIV"
};

static char * fr [] = {
	"REG, R0", "REG, R1", "REG, R2",  "REG, R3",  "REG, R4",  "REG, R5",  "REG, R6",  "REG, R7",
	"REG, R8", "REG, R9", "REG, R10", "REG, R11", "REG, R12", "REG, R13", "REG, R14", "REG, R15"
};

static char * fi [] = {
	"IMM, 0x11", "IMM, 0x1122", "IMM, 0x11223344", "IMM, 0x11223344"
};

int main (void) {
	int a, w, t, d, f, s;

	for (a = 0; a < (int) (sizeof (fa) / sizeof (* fa)); ++a) {
		for (w = 0; w < 4; ++w) {
			for (d = 0; d < 16; ++d) {
				for (s = 0; s < 16; ++s) {
						printf ("NOP, %s, %s, %s, %s,\n", fa [a], fw [w], fr [d], fr [s]);
				}
			}
		}
	}

	for (a = 0; a < (int) (sizeof (fa) / sizeof (* fa)); ++a) {
		for (w = 0; w < 4; ++w) {
			for (d = 0; d < 16; ++d) {
					printf ("NOP, %s, %s, %s, %s,\n", fa [a], fw [w], fr [d], fi [w]);
			}
		}
	}

	for (a = 0; a < (int) (sizeof (faa) / sizeof (* faa)); ++a) {
		for (w = 0; w < 4; ++w) {
			for (d = 0; d < 16; ++d) {
				printf ("NOP, %s, %s, %s,\n", faa [a], fw [w], fr [d]);
			}
		}
	}

	return (0);
}
