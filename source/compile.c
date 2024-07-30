#include "compile.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>

#include "eaxhla.tab.h"

#include "eaxhla.h"
#include "assembler.h"
#include "unix.h"
#include "safety.h"
#include "debug.h"
#include "printf2.h"

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
    if (variable->symbol_type != VARIABLE_SYMBOL) {
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
    // XXX portability
	checked_fwrite(elf_main_header_byte, 1UL, ELF_MAIN_HEADER_SIZE, file);
	checked_fwrite(elf_text_sector_byte, 1UL, ELF_TEXT_SECTOR_SIZE, file);
	checked_fwrite(elf_data_sector_byte, 1UL, ELF_DATA_SECTOR_SIZE, file);

	//text
	checked_fwrite(text_sector_byte, sizeof(*text_sector_byte), (size_t)text_sector_size, file);

    return 0;
}

static
int create_header() {
    if (system_type == UNIX) {
        int total_reserved_size = variable_size_sum();

        elf_main_header(1, 1, 1);
        elf_text_sector(text_sector_size, total_reserved_size);
        elf_data_sector(text_sector_size, total_reserved_size);
    }

    return 0;
}

static
int make_executable(const char * const filename) {
    int r = 0;

  #if defined(__unix__)
    r = chmod(filename, 0755);
  #endif

    return r;
}

int compile(void) {
    debug_puts("Compiling output...");

    dump_variables_to_assembler();

    // Anon: Example usage, delete or modify it...
    printf2("[@yTest@-] Begining assembling @c%c%u@- process... @b@@%f@-\n", 'A', 6, 0.666);

    // Anon: I moved memory management of text+data sections here.
    // Assembler shouldn't control how much memory it has, user should!
    text_sector_byte = calloc (4096UL, sizeof (* text_sector_byte));

    if (assemble(token_count, token_array)) {
        issue_internal_error();
        return 1;
    }

    create_header();

    FILE * output_file = fopen(output_file_name, "w");
    check(write_output(output_file));
    fclose(output_file);

    make_executable(output_file_name);

    free (text_sector_byte);

    return 0;
}

void append_instructions_from_mem(void * src, unsigned n) {
    memcpy(token_array + token_count, src, n);
    token_count += n;
}

void _append_instructions(const unsigned argc, ...) {
    va_list ap;
    va_start(ap, argc);

    for (unsigned i = 0; i < argc; i++) {
        token_array [token_count] = va_arg(ap, int);
        token_count += 1;
    }

    va_end(ap);
}

void append_exit(int code) {
    if (system_type == UNIX) {
        append_instructions(MOV, D32, REG, GR0, IMM, 60,
                            MOV, D32, REG, GR7, IMM, code,
                            SYSCALL
                        );
    }
}

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
