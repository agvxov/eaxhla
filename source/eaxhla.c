#include "eaxhla.h"

/* This source file is responsible for holding data
 *  that belongs neither to the scanner nor the parser.
 * It also facades "implementation details" such as
 *  the storage of variables.
 */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

tommy_hashtable variable_table;

char * scope = NULL;
int is_program_found = 0;

#if DEBUG == 1
static
void dump_variable(void * data) {
    variable_t * variable = (variable_t*)data;
    printf("{ .name = '%s', .value = '%ld', .size = %d }\n",
            variable->name,
            variable->value,
            variable->size
    );
}

static
void dump_variables(void) {
    tommy_hashtable_foreach(&variable_table, dump_variable);
}
#endif

static
int table_compare_unsigned(const void * arg, const void * obj) {
  return *(const unsigned *) arg != ((const variable_t*)obj)->_hash;
}

void add_variable(variable_t variable) {
    if (get_variable(variable.name)) {
        issue_error("variable declared twice");
        return;
    }
    // XXX this is cursed
    variable_t * heap_variable = malloc(sizeof(variable));
    memcpy(heap_variable, &variable, sizeof(variable));
    // */
    heap_variable->_hash = tommy_strhash_u32(0, heap_variable->name);
    tommy_hashtable_insert(&variable_table,
                            &heap_variable->_node,
                            heap_variable,
                            heap_variable->_hash
                        );
}

char * make_scoped_name(const char * const scope, char * name) {
    char * r;
    const long scl = strlen(scope);
    const long nml = strlen(name);
    r = malloc(2 + scl + 1 + nml + 1);
    r[0] = '_';
    r[1] = '_';
    strcat(r + 2, scope);
    r[2 + scl] = '_';
    strcat(r + 2 + scl + 1, name);
    free(name);
    return r;
}

variable_t * get_variable(const char * const name) {
    unsigned lookup_hash = tommy_strhash_u32(0, name);
    variable_t * r = tommy_hashtable_search(&variable_table,
                                            table_compare_unsigned,
                                            &lookup_hash,
                                            lookup_hash
                                        );
    return r;
}

int eaxhla_init(void) {
    tommy_hashtable_init(&variable_table, 256);
    return 0;
}

static
void free_variable(void * data) {
    variable_t * variable = (variable_t*)data;
    free(variable->name);
    free(variable);
}

int eaxhla_destroy(void) {
  #if DEBUG == 1
    dump_variables();
  #endif
    tommy_hashtable_foreach(&variable_table, free_variable);
    tommy_hashtable_done(&variable_table);
    return 0;
}

void issue_warning(const char * const format, ...) {
    extern char * yyfilename;
    extern int yylineno;

    va_list args;
    va_start(args, format);

    char * msg;
    const int ignore = vasprintf(&msg, format, args);
    (void)ignore;
    printf("\033[1m%s:%d:\033[0m \033[35mWarning\033[0m: %s.\n",
                yyfilename,
                yylineno,
                msg
            );
    free(msg);
}

void issue_error(const char * const format, ...) {
    extern char * yyfilename;
    extern int yylineno;

    va_list args;
    va_start(args, format);

    char * msg;
    const int ignore = vasprintf(&msg, format, args);
    (void)ignore;
    printf("\033[1m%s:%d:\033[0m \033[31mError\033[0m: %s.\n",
                yyfilename,
                yylineno,
                msg
            );
    free(msg);
}
