#ifndef  DEBUG_H
#define DEBUG_H

#if DEBUG == 1
#include <stdio.h>
#include <stdarg.h>
#include "eaxhla.h"

__attribute__((unused))
static
void breakpoint(void) { ; }

# define debug_puts(msg) do { puts(msg); } while (0)

__attribute__((unused))
static
void debug_printf(const char * const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

__attribute__((unused))
static
void dump_variable(void * data) {
    symbol_t * variable = (symbol_t*)data;
    if (variable->symbol_type != VARIABLE_SYMBOL) { return; }

    if (variable->elements == 1) {
        printf("{ .name = '%s', .id = %d, .value = '%ld' }\n",
                variable->name,
                variable->_id,
                variable->value
        );
    } else {
        printf("{ .name = '%s', .id = %d, .elements = '%lu', .array_value = \"%.*s\" }\n",
                variable->name,
                variable->_id,
                variable->elements,
                (int)variable->elements,
                (char*)variable->array_value
        );
    }
}

__attribute__((unused))
static
void debug_dump_variables(void) {
    puts("# Variables:");
    tommy_hashtable_foreach(&symbol_table, dump_variable);
}

__attribute__((unused))
static
void dump_function(void * data) {
    symbol_t * function = (symbol_t*)data;
    if (function->symbol_type != LABEL_SYMBOL) { return; }

    printf("{ .name = '%s', .id = %d }\n",
            function->name,
            function->_id
    );
}

__attribute__((unused))
static 
void debug_dump_functions(void) {
    puts("# Functions:");
    tommy_hashtable_foreach(&symbol_table, dump_function);
}

__attribute__((unused))
static
void debug_dump_symbols(void) {
    debug_dump_variables();
    debug_dump_functions();
    printf("# Total variable size: '%d'\n", variable_size_sum());
}

__attribute__((unused))
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
