#include "eaxhla.h"

/* This source file is responsible for holding data
 *  that belongs neither to the scanner nor the parser.
 * It also facades "implementation details" such as
 *  the storage of variables.
 */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "debug.h"
#include "eaxhla.tab.h"
#include "assembler.h"

unsigned long long anon_variable_counter = 0;

tommy_hashtable variable_table;

int has_encountered_error = 0;

char * scope = NULL;
int is_program_found = 0;

static
int table_compare_unsigned(const void * arg, const void * obj) {
  return *(const unsigned *) arg != ((const variable_t*)obj)->_hash;
}

void add_variable(variable_t variable) {
    static unsigned vid = 0;
    if (get_variable(variable.name)) {
        // XXX: this should say the varname, but this function does not know it
        //       in fact this source file should not be reporting errors,
        //       it should be returning an error and the parser should check.
        issue_error("variable declared twice");
        return;
    }
    variable._id = vid++;
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

int type2size(int type) {
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

    return -1;
}

int validate_array_size(int size) {
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
    debug_dump_variables();
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

static
void dump_variable_to_assembler(void * data) {
    variable_t * variable = (variable_t*)data;
    append_instruction_t4(ASMDIRMEM, variable->_id, ASMDIRIMM, type2size(variable->type));
    if (variable->elements == 1) {
        append_instruction_t1(variable->value);
    } else {
        memcpy(t_array + t_count, variable->array_value, variable->elements);
        t_count += variable->elements;
    }
}

void dump_variables_to_assembler(void) {
    tommy_hashtable_foreach(&variable_table, dump_variable);
}

static
void append_token (int t) {
    // XXX rewrite this and use memcpy
	t_array [t_count] = t;
	t_count += 1;
}

void append_instruction_t1 (int t1) {
	append_token (t1); // operation
}

void append_instruction_t4 (int t4, int w, int d, int r) {
	append_token (t4); // operation
	append_token (w);  // width
	append_token (d);  // destination
	append_token (r);  // register
}

void append_instruction_t6 (int t6, int w, int d, int r, int s, int i) {
	append_token (t6); // operation
	append_token (w);  // width
	append_token (d);  // destination
	append_token (r);  // register
	append_token (s);  // source
	append_token (i);  // immediate
}

// my_label:
void append_label (int rel) {
	append_instruction_t1 (ASMDIRMEM);
	append_instruction_t1 (rel);
}

// procedure my_procedure ... <argv> ... begin
// rel = my_procedure (some unique index)
// best if it's count of defined procedures!
// it must not be address of it, or huge number!
// optimally, it should be number 0 ... 140.
// for now, 140 procedures is enough, will expand later!
void append_fastcall_begin (int rel) {
	append_label (rel);
}

// end procedure
void append_fastcall_end (void) {
	append_instruction_t1 (RETN);
}

// append these at the end, postpone it!
// this function needs to be called after ALL instructions are parsed.
// it has to do with structure of every binary executable file!
// we can add it later, it's "triggered" on 'in'.
void append_fastcall_arguments (int rel, int wid, int imm) { // TODO
	append_instruction_t1 (ASMDIRMEM);
	append_instruction_t1 (rel);
	append_instruction_t1 (ASMDIRIMM);
	append_instruction_t1 (wid);
	append_instruction_t1 (imm);
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
