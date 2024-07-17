#ifndef  DEBUG_H
#define DEBUG_H

#if DEBUG == 1
#include <stdio.h>
#include <stdarg.h>
#include "eaxhla.h"

# define debug_puts(msg) do { puts(msg); } while (0)

static  // this is less horid than macro varargs
void debug_printf(const char * const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

static
void dump_variable(void * data) {
    symbol_t * variable = (symbol_t*)data;
    if (variable->symbol_type != VARIABLE) { return; }

    if (variable->elements == 1) {
        printf("{ .name = '%s', .value = '%ld' }\n",
                variable->name,
                variable->value
        );
    } else {
        printf("{ .name = '%s', .elements = '%llu', .array_value = \"%.*s\" }\n",
                variable->name,
                variable->elements,
                (int)variable->elements,
                (char*)variable->array_value
        );
    }
}

static
void debug_dump_variables(void) {
    puts("# Variables:");
    tommy_hashtable_foreach(&symbol_table, dump_variable);
}

static
void dump_function(void * data) {
    symbol_t * function = (symbol_t*)data;
    if (function->symbol_type != FUNCTION) { return; }

    printf("{ .name = '%s' }\n",
            function->name
    );
}

static 
void debug_dump_functions(void) {
    puts("# Functions:");
    tommy_hashtable_foreach(&symbol_table, dump_function);
}

static
void debug_dump_symbols(void) {
    debug_dump_variables();
    debug_dump_functions();
    printf("# Total variable size: '%d'\n", variable_size_sum());
}

static
void debug_token_dump(void) {
    extern unsigned int * token_array;
    extern unsigned int   token_count;
    FILE * o = fopen("token_dump", "wb");
    fwrite(token_array, sizeof(int), token_count, o);
    fclose(o);
}

#else

# define debug_puts(msg)
# define debug_printf(...)
# define debug_dump_variables() do {} while (0)
# define debug_dump_functions() do {} while (0)
# define debug_dump_symbols() do {} while (0)
# define debug_token_dump() do {} while (0)

#endif

#endif
