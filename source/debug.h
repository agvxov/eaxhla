#ifndef  DEBUG_H
#define DEBUG_H

#if DEBUG == 1
#include <stdio.h>

# define debug_puts(msg) do { puts(msg); } while (0)

static  // this is less horid than macro varargs
void debug_printf(const char * const fmt, ...) {
    va_list args;
    va_start(args, count);
    vprintf(fmt, args);
    va_end(args);
}

static
void dump_variable(void * data) {
    variable_t * variable = (variable_t*)data;
    printf("{ .name = '%s', .value = '%ld', .elements = %d }\n",
            variable->name,
            variable->value,
            variable->elements
    );
}

static
void debug_dump_variables(void) {
    tommy_hashtable_foreach(&variable_table, dump_variable);
}

#else

# define debug_puts(msg)
# define debug_printf(...)
# define debug_dump_variables() do {} while (0)

#endif

#endif
