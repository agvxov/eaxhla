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
#include "arena.h"
#include "printf2.h"

unsigned * token_array = NULL;
unsigned   token_count = 0;

char * output_file_name = "a.out";

int compile_init(void) {
    // Funny hacky solution, we can use arenas here.
    // And a day later we did use them...
    token_array = aalloc(1440ul * sizeof(*token_array));
    return 0;
}

// Function not needful anymore sir!
// Arena handles the memory, no need to free!
int compile_deinit(void) {
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
    switch (system_type) {
        case UNIX: {
            // Header calculation
            int total_reserved_size = variable_size_sum();

            elf_main_header(main_entry_point, 1, 1, 1);
            elf_text_sector(text_sector_size, total_reserved_size);
            elf_data_sector(text_sector_size, total_reserved_size);

            // Writting
            checked_fwrite(elf_main_header_byte, 1UL, ELF_MAIN_HEADER_SIZE, file);
            checked_fwrite(elf_text_sector_byte, 1UL, ELF_TEXT_SECTOR_SIZE, file);
            checked_fwrite(elf_data_sector_byte, 1UL, ELF_DATA_SECTOR_SIZE, file);

            checked_fwrite(text_sector_byte, sizeof(*text_sector_byte), (size_t)text_sector_size, file);
        } break;
        case SHELL: {
            elf_main_header(main_entry_point, 1, 1, 1);
            elf_text_sector(text_sector_size, 0);
            elf_data_sector(text_sector_size, 0);

            checked_fwrite(text_sector_byte, sizeof(*text_sector_byte), (size_t)text_sector_size, file);
        } break;
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

void _append_instructions(const unsigned argc, ...) {
    // XXX: extension logic
    va_list ap;
    va_start(ap, argc);

    for (unsigned i = 0; i < argc; i++) {
        token_array [token_count] = va_arg(ap, int);
        token_count += 1;
    }

    va_end(ap);
}

int compile(void) {
    debug_puts("Compiling output...");

    dump_variables_to_assembler();

    text_sector_byte = aalloc(4096ul * sizeof(*text_sector_byte));

    if (!assemble(token_count, token_array)) {
        issue_internal_error();
        return 1;
    }

    FILE * output_file = fopen(output_file_name, "w");
      check(write_output(output_file));
    fclose(output_file);

    make_executable(output_file_name);

    return 0;
}
