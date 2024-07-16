#include "compile.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "eaxhla.h"
#include "assembler.h"
#include "unix.h"
#include "debug.h"

unsigned int * token_array = NULL;
unsigned int   token_count = 0;

char * output_file_name = "a.out";

int compile_init(void) {
	token_array = calloc(1440UL, sizeof(*token_array));
    return 0;
}

int compile_deinit(void) {
	free(token_array);
    return 0;
}

static
void dump_variable_to_assembler(void * data) {
    symbol_t * variable = (symbol_t*)data;
    if (variable->symbol_type != VARIABLE) {
        return;
    }

    append_instructions(ASMDIRMEM, variable->_id, ASMDIRIMM, type2size(variable->type));

    append_instructions(variable->elements);

    if (variable->elements == 1) {
        append_instructions(variable->value);
    } else {
        for (unsigned long long i = 0; i < variable->elements; i++) {
            append_instructions((int)*((char*)variable->array_value + i));
        }
    }
}

static
void dump_variables_to_assembler(void) {
    tommy_hashtable_foreach(&symbol_table, dump_variable_to_assembler);
}

static
int write_output(FILE * file) {
    // XXX Error checking
	elf_main_header (1, 1, 1, 0);
	elf_text_sector (text_sector_size);
	elf_data_sector (text_sector_size, 12);

	fwrite (elf_main_header_byte, 1UL, ELF_MAIN_HEADER_SIZE, file);
	fwrite (elf_text_sector_byte, 1UL, ELF_TEXT_SECTOR_SIZE, file);
	fwrite (elf_data_sector_byte, 1UL, ELF_DATA_SECTOR_SIZE, file);

	//text
	fwrite (text_sector_byte, sizeof (* text_sector_byte),
	        (size_t) text_sector_size, file);

    return 0;
}

static
int make_executable(const char * const filename) {
    // XXX only works on *nix
    int r = chmod(filename, 0755);

    return r;
}

int compile(void) {
    debug_puts("Compiling output...");

    dump_variables_to_assembler();

    assemble(token_count, token_array);

	FILE * output_file = fopen (output_file_name, "w");
    write_output(output_file);
	fclose(output_file);

    make_executable(output_file_name);

    return 0;
}

void _append_instructions(unsigned argc, ...) {
    va_list ap;
    va_start(ap, argc);

    for (unsigned i = 0; i < argc; i++) {
        token_array [token_count] = va_arg(ap, int);
        token_count += 1;
    }

    va_end(ap);
}

// my_label:
void append_label (int rel) {
    append_instructions(ASMDIRMEM, rel);
}

// procedure my_procedure ... <argv> ... begin
// rel = my_procedure (some unique index)
// best if it's count of defined procedures!
// it must not be address of it, or huge number!
// optimally, it should be number 0 ... 140.
// for now, 140 procedures is enough, will expand later!
void append_fastcall_begin (int rel) {
	append_label(rel);
}

// end procedure
void append_fastcall_end (void) {
	append_instructions(RETN);
}

// append these at the end, postpone it!
// this function needs to be called after ALL instructions are parsed.
// it has to do with structure of every binary executable file!
// we can add it later, it's "triggered" on 'in'.
void append_fastcall_arguments (int rel, int wid, int imm) { // TODO
	append_instructions(ASMDIRMEM, rel, ASMDIRIMM, wid, imm);
}
