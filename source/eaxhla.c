#include "eaxhla.h"

/* This source file is responsible for holding data
 *  that belongs neither to the scanner nor the parser.
 * It also facades "implementation details" such as
 *  the storage of variables.
 */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "eaxhla.tab.h"

#include "debug.h"
#include "assembler.h"
#include "compile.h"

int has_encountered_error = 0;
int is_program_found      = 0;

char * yyfilename = "";

/* Used for ensuring that `get_*` calls recieve a valid symbol,
 *  not segfaulting the parser in the process
 */
static symbol_t * undeclared_symbol;

/* Used for naming variables constructed from literals
 */
static size_t anon_variable_counter = 0;
/* Used to check whether all labels without
 *  previous declarations (forward jumps)
 *  have been declared later in code
 */
static size_t unresolved_label_counter = 0;

/* Used for storing `repeat` implicit label information
 * For each loop we allocate 2 ids. 1 for the start and 1 for the end,
 *  the second is implicitly ((the value stored in the stack) + 1)
*/
static int    repeat_stack[MAX_REPEAT_NESTING];
static size_t repeat_stack_empty_top = 0;

static int    if_stack[MAX_IF_NESTING];
static size_t if_stack_empty_top = 0;

static unsigned symbol_id = 1;
tommy_hashtable symbol_table;

void add_if(void) {
    if_stack[if_stack_empty_top] = symbol_id;
    ++if_stack_empty_top;
    ++symbol_id;
}

void fin_if(void) {
    append_instructions(ASMDIRMEM, if_stack[if_stack_empty_top]);
    --if_stack_empty_top;
}

/* NOTE: these functions should accept any type and *probably* figure out combos that
 *        are not asm valid
 */
void add_logic(cpuregister_t * c1, cpuregister_t * c2, logic_t logic) {
    append_instructions(CMP, c1->size, REG, c1->number, REG, c2->number);
    int instrunction;
    switch (logic) {
        case EQUALS:       instrunction = JNE; break;
        case NOT_EQUALS:   instrunction = JE;  break;
        case LESSER_THAN:  instrunction = JNL; break;
        case GREATER_THAN: instrunction = JNG; break;
        default:           issue_internal_error();
    }
    append_instructions(instrunction, D32, REL, if_stack[if_stack_empty_top-1]);
}

void add_repeat(void) {
    if (repeat_stack_empty_top == MAX_REPEAT_NESTING) {
        issue_error("this implementation only support a maximum"
                    " number of %d levels of repeat nesting",
                    MAX_REPEAT_NESTING
                );
        return;
    }

    append_instructions(ASMDIRMEM, symbol_id);
    repeat_stack[repeat_stack_empty_top] = symbol_id;
    ++repeat_stack_empty_top;
    symbol_id += 2;
}

void fin_repeat(void) {
    --repeat_stack_empty_top;
    append_instructions(JMP, D32, REL, repeat_stack[repeat_stack_empty_top]);
    append_instructions(ASMDIRMEM, repeat_stack[repeat_stack_empty_top]+1);
}

void add_continue(unsigned i) {
    if (!repeat_stack_empty_top) {
        issue_error("'continue' is only valid inside 'repeat'");
        return;
    }
    if (((int)repeat_stack_empty_top - (int)i) < 0) {
        issue_error("'continue %u' is too deep inside just %d level(s) of nesting",
                        i,
                        repeat_stack_empty_top
                );
        return;
    }

    append_instructions(JMP, D32, REL, repeat_stack[repeat_stack_empty_top-i]);
}

void add_break(unsigned i) {
    if (!repeat_stack_empty_top) {
        issue_error("'break' is only valid inside 'repeat'");
        return;
    }
    if (((int)repeat_stack_empty_top - (int)i) < 0) {
        issue_error("'break %u' is too deep inside just %d level(s) of nesting",
                        i,
                        repeat_stack_empty_top
                );
        return;
    }

    append_instructions(JMP, D32, REL, repeat_stack[repeat_stack_empty_top-i]+1);
}

static char * scope = NULL;
void empty_out_scope(void) {
    free(scope);
    scope = NULL;
}


int eaxhla_init(void) {
    undeclared_symbol = (symbol_t *)calloc(sizeof(symbol_t), 1);
    tommy_hashtable_init(&symbol_table, 256);
    return 0;
}

/* NOTE: This function takes ownership of its `name` argument
 */
symbol_t * new_symbol(char * name) {
    symbol_t * r;

    r = (symbol_t *)calloc(sizeof(symbol_t), 1);
    r->name = name;

    return r;
}

void free_symbol(void * data) {
    symbol_t * variable = (symbol_t*)data;

    free(variable->name);

    if (variable->symbol_type == VARIABLE_SYMBOL
    &&  variable->elements != 1) {
        free(variable->array_value);
    }

    free(variable);
}

int eaxhla_deinit(void) {
    empty_out_scope();

    free(undeclared_symbol);
    tommy_hashtable_foreach(&symbol_table, free_symbol);
    tommy_hashtable_done(&symbol_table);

    return 0;
}

/* Are these literals ugly? yes.
 * However it would be much more painful to calculate the values inline.
 */
int can_fit(const int type, const long long value) {
    size_t max = 0;
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
    return value > 0 ? (size_t)value <= max : value >= min;
}

int validate_array_size(const int size) {
    if (size < 1) {
        issue_error("cannot create an array of size '%d', because its less than 1", size);
        return 1;
    }
    return 0;
}

static
int table_compare_unsigned(const void * arg, const void * obj) {
    return *(const unsigned *) arg != ((const symbol_t*)obj)->_hash;
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

static
void symbol_insert(symbol_t * symbol) {
    symbol->_hash = tommy_strhash_u32(0, symbol->name);
    tommy_hashtable_insert(&symbol_table,
                           &symbol->_node,
                            symbol,
                            symbol->_hash
                        );
}

void add_scope(const char * const name){
    free(scope);
    scope = strdup(name);
}

// XXX: alternative version on the stack
static
char * make_scoped_name(const char * const scope, const char * const name) {
    if (!scope) {
        return (char*)name;
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

    return r;
}

symbol_t * get_symbol(const char * const name) {
    symbol_t * r;
    r = symbol_lookup(name);
    if (r) {
        return r;
    }

    char * alternative_name = make_scoped_name(scope, name);
    r = symbol_lookup(alternative_name);
    free(alternative_name);

    return r;
}

void add_program(const char * const name) {
    (void)name;

    if (is_program_found) {
        issue_error("only 1 entry point is allowed and a program block was already found");
    }
    is_program_found = 1;

    append_instructions(ASMDIRMEM, 0);
}

static
void _add_variable(unsigned type, const char * const name, size_t size, void * value) {
    char * full_name = make_scoped_name(scope, name);
    if (get_symbol(name)) {
        issue_error("symbol '%s' redeclared as new variable", name);
        return;
    }

    symbol_t * variable = new_symbol(full_name);
    variable->elements = size;
    variable->type = type;

    if (size == 1) {
        variable->value = (long)value;
        if (!can_fit(type, variable->value)) {
            issue_warning("the value \033[1m'%lld'\033[0m will overflow in assignement",
                            variable->value
            );
        }
    } else {
        variable->array_value = value;
        validate_array_size(size);
    }

    variable->_id = symbol_id++;
    variable->symbol_type = VARIABLE_SYMBOL;

    symbol_insert(variable);

}

void add_variable(unsigned type, const char * const name) {
    _add_variable(type, name, 1, 0);
}

void add_variable_with_value(unsigned type, const char * const name, size_t value) {
    _add_variable(type, name, 1, (void *)value);
}

void add_array_variable(unsigned type, const char * const name, size_t size) {
    _add_variable(type, name, size, 0);
}

void add_array_variable_with_value(unsigned type, const char * const name, size_t size, void * value, size_t value_size) {
    if (size < value_size) {
        issue_warning("declared array size is smaller than assigned literal, this will cause truncation");
    }
    _add_variable(type, name, size, value);
}

void add_procedure(const char * const name) {
    if (get_symbol(name)) {
        issue_error("symbol '%s' redeclared as new function", name);
        return;
    }

    symbol_t * procedure = new_symbol(strdup(name));

    procedure->_id = symbol_id++;
    procedure->symbol_type = LABEL_SYMBOL;
    procedure->is_resolved = true;

    symbol_insert(procedure);

    append_instructions(ASMDIRMEM, procedure->_id);
}

void add_literal(void * data, size_t size) {
    char * name;
    int ignore = asprintf(&name, "_anon_%lu", anon_variable_counter++);
    (void)ignore;

    symbol_t * literal = (symbol_t *)calloc(sizeof(symbol_t), 1);
    literal->name = name;
    literal->elements = size;
    literal->array_value = data;
}

static
symbol_t * _add_label(const char * const name, int is_resolved) {
    char * full_name = make_scoped_name(scope, name);

    symbol_t * label = get_symbol(full_name);
    
    if (label) {
        if (label->is_resolved) {
            issue_error("symbol '%s' redeclared as new label", name);
        } else {
            label->is_resolved = true;
            --unresolved_label_counter;
        }
        free(full_name);
        return label;
    }

    label = new_symbol(full_name);

    label->_id = symbol_id++;
    label->symbol_type = LABEL_SYMBOL;
    label->is_resolved = is_resolved;

    if (!is_resolved) {
        ++unresolved_label_counter;
    }

    symbol_insert(label);

    return label;
}

void add_label(const char * const name, int is_resolved) {
    symbol_t * label = _add_label(name, is_resolved);
    append_instructions(ASMDIRMEM, label->_id);
}

void add_fastcall(const char * const destination) {
    symbol_t * function = get_symbol(destination);
    if (!function) {
        issue_error("can't fastcall '%s', no such known symbol", destination);
        return;
    }

    append_instructions(CALL, REL, function->_id);
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

    issue_internal_error();
    return -1;
}

int size2bytes(const int size) {
    switch (size) {
        case D8:  return 1;
        case D16: return 2;
        case D32: return 4;
        case D64: return 8;
    }

    issue_internal_error();
    return -1;
}

static
void _variable_size_sum_iteration(void * i, void * data) {
    symbol_t * variable = (symbol_t*)data;
    if (variable->symbol_type != VARIABLE_SYMBOL) { return; }

    int * sum = i;

    *sum += variable->elements * size2bytes(type2size(variable->type));
}

int variable_size_sum(void) {
    int r = 0;
    tommy_hashtable_foreach_arg(&symbol_table, _variable_size_sum_iteration, &r);
    return r;
}

symbol_t * get_variable(const char * const name) {
    symbol_t * r;
    char * varname = make_scoped_name(scope, name);

    r = symbol_lookup(varname);
    if (r
    &&  r->symbol_type != VARIABLE_SYMBOL) {
        issue_error("the symbol '%s' is not a variable", name);
    }

    if (!r) {
        r = undeclared_symbol;
    }

    free(varname);
    return r;
}

symbol_t * get_function(const char * const name) {
    symbol_t * r;
    r = symbol_lookup(name);
    if (r
    &&  r->symbol_type != LABEL_SYMBOL) {
        issue_error("the symbol '%s' is not a function", name);
        return NULL;
    }
    return r;
}

symbol_t * get_relative(const char * const name) {
    symbol_t * r;

    r = get_symbol(name);
    if (r) {
        return r;
    }

    r = _add_label(name, false);

    return r;
}

void fin_procedure(void) {
    append_instructions(RETN);
}

static
void unresolved_check(void * data) {
    symbol_t * label = (symbol_t*)data;
    if (label->type != LABEL_SYMBOL) {
        return;
    }
    if (!label->is_resolved) {
        issue_error("the label '%s' was never resolved.", label->name);
    }
}

void fin_hla(void) {
    if (anon_variable_counter) {
        tommy_hashtable_foreach(&symbol_table, unresolved_check);
    }
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

void issue_internal_error(void) {
    issue_error("internal error");
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
