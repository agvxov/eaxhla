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

#include "debug.h"
#include "assembler.h"
#include "compile.h"

unsigned long long anon_variable_counter = 0;

static unsigned symbol_id = 1;
tommy_hashtable symbol_table;

int has_encountered_error = 0;
int is_program_found      = 0;

char * scope = NULL;
void empty_out_scope(void) {
    free(scope);
    scope = NULL;
}

char * yyfilename = "";


int eaxhla_init(void) {
    tommy_hashtable_init(&symbol_table, 256);
    return 0;
}

static
void free_symbol(void * data) {
    symbol_t * variable = (symbol_t*)data;
    free(variable->name);
    free(variable);
}

int eaxhla_deinit(void) {
    tommy_hashtable_foreach(&symbol_table, free_symbol);
    tommy_hashtable_done(&symbol_table);
    return 0;
}



static
int table_compare_unsigned(const void * arg, const void * obj) {
  return *(const unsigned *) arg != ((const symbol_t*)obj)->_hash;
}

void add_variable(symbol_t variable) {
    if (get_variable(variable.name)) {
        issue_error("symbol '%s' redeclared as new variable", variable.name);
        return;
    }

    variable._id = symbol_id++;
    variable.symbol_type = VARIABLE;

    symbol_t * heap_variable = malloc(sizeof(variable));
    memcpy(heap_variable, &variable, sizeof(variable));

    heap_variable->_hash = tommy_strhash_u32(0, heap_variable->name);
    tommy_hashtable_insert(&symbol_table,
                            &heap_variable->_node,
                            heap_variable,
                            heap_variable->_hash
                        );
}

void add_procedure(symbol_t procedure) {
    if (get_function(procedure.name)) {
        issue_error("symbol '%s' redeclared as new function", procedure.name);
        return;
    }

    procedure._id = symbol_id++;
    procedure.symbol_type = FUNCTION;

    symbol_t * heap_procedure = malloc(sizeof(procedure));
    memcpy(heap_procedure, &procedure, sizeof(procedure));

    heap_procedure->_hash = tommy_strhash_u32(0, heap_procedure->name);
    tommy_hashtable_insert(&symbol_table,
                            &heap_procedure->_node,
                            heap_procedure,
                            heap_procedure->_hash
                        );
    //
    append_instructions(ASMDIRMEM, procedure._id);
}

/* Are these literals ugly? yes.
 * However it would be much more painful to calculate the values inline.
 */
int can_fit(const int type, const long long value) {
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

int type2size(const int type) {
    switch (type) {
        case U8:
        case S8:
            return D8;
        case U16:
        case S16:
            return D16;
        case U32:
        case S32:
            return D32;
        case U64:
        case S64:
            return D64;
    }

    issue_error("internal error");
    return -1;
}

int size2bytes(const int size) {
    switch (size) {
        case D8:  return 1;
        case D16: return 2;
        case D32: return 4;
        case D64: return 8;
    }

    issue_error("internal error");
    return -1;
}

static
void _variable_size_sum_iteration(void * i, void * data) {
    symbol_t * variable = (symbol_t*)data;
    if (variable->symbol_type != VARIABLE) { return; }

    int * sum = i;

    *sum += variable->elements * size2bytes(type2size(variable->type));
}

int variable_size_sum(void) {
    int r = 0;
    tommy_hashtable_foreach_arg(&symbol_table, _variable_size_sum_iteration, &r);
    return r;
}

int validate_array_size(const int size) {
    if (size < 1) {
        issue_error("cannot create an array of size '%d', because its less than 1", size);
        return 1;
    }
    return 0;
}

char * make_scoped_name(const char * const scope, char * name) {
    if (!scope) {
        return name;
    }

    char * r;
    const long scl = strlen(scope);
    const long nml = strlen(name);
    r = malloc(2 + scl + 1 + nml + 1);
    r[0] = '_';
    r[1] = '_';
    memcpy(r + 2, scope, scl);
    r[2 + scl] = '_';
    memcpy(r + 2 + scl + 1, name, nml);
    r[2 + scl + 1 + nml] = '\0';
    free(name);
    return r;
}

static
void * symbol_lookup(const char * const name) {
    unsigned lookup_hash = tommy_strhash_u32(0, name);
    void * r = tommy_hashtable_search(&symbol_table,
                                      table_compare_unsigned,
                                      &lookup_hash,
                                      lookup_hash
                                    );
    return r;
}

symbol_t * get_variable(const char * const name) {
    symbol_t *r = symbol_lookup(name);
    if (r
    &&  r->symbol_type != VARIABLE) {
        issue_error("the symbol '%s' is not a variable", name);
        return NULL;
    }
    return r;
}

symbol_t * get_function(const char * const name) {
    symbol_t * r = symbol_lookup(name);
    if (r
    &&  r->symbol_type != FUNCTION) {
        issue_error("the symbol '%s' is not a function", name);
        return NULL;
    }
    return r;
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

    has_encountered_error = 1;

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
