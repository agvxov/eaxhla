#include "eaxhla.h"

/* This source file is responsible for holding data
 *  that belongs neither to the scanner nor the parser.
 * It also facades "implementation details" such as
 *  the storage of variables.
 */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "eaxhla.tab.h"

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
        // XXX: this should say the varname, but this function does not know it
        //       in fact this source file should not be reporting errors,
        //       it should be returning an error and the parser should check.
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

/* Are these literals ugly? yes.
 * However it would be much more painful to calculate the values inline.
 */
int can_fit(int type, long long value) {
    unsigned long long max = 0;
    long long min = 0;
    switch (type) {
        case U8: {
            max = 255;
        } break;
        case U16: {
            max = 65535;
        } break;
        case U32: {
            max = 4294967295;
        } break;
        case U64: {
            max = 9223372036854775807;
        } break;
        case S8: {
            min = -128;
            max =  127;
        } break;
        case S16: {
            min = -256;
            max =  255;
        } break;
        case S32: {
            min = -65536;
            max =  65535;
        } break;
        case S64: {
            min = -4294967296;
            max =  4294967295;
        } break;
    }
    return value > 0 ? (unsigned long long)value <= max : value >= min;
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
    fprintf(stderr, "\033[1m%s:%d:\033[0m \033[35mWarning\033[0m: %s.\n",
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
    fprintf(stderr, "\033[1m%s:%d:\033[0m \033[31mError\033[0m: %s.\n",
                yyfilename,
                yylineno,
                msg
            );
    free(msg);
}

int system_type = 
  #if defined(__unix__)
    UNIX
  #elif defined(_WIN64)
    WIN64
  #else
    #error Your system was not recognized.
    0
  #endif
;
