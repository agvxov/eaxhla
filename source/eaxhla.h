#ifndef EAXHLA_H
#define EAXHLA_H

#include <tommyds/tommyds/tommyhashtbl.h>

#define WORD_SIZE_IN_BYTES (64/8)

// XXX these should be private
typedef enum {
    VARIABLE_SYMBOL,
    LABEL_SYMBOL,
} symbol_type_t;

typedef struct {
    symbol_type_t symbol_type;
    union {
        struct { // VARIABLE_SYMBOL
            int type;
            size_t elements;
            union {
                long   value;
                void * array_value;
            };
        };
        struct { // LABEL_SYMBOL
            int is_resolved;
        };
    };
    char *     name;
    unsigned   _id;
    unsigned   _hash;
    tommy_node _node;
} symbol_t;

/* private:
symbol_t * new_symbol(const char * const name);
void free_symbol(void * name);
 */

extern tommy_hashtable symbol_table;

typedef struct {
    unsigned number : 6;
    unsigned size   : 2;
} cpuregister_t;

extern int system_type;

/* Used for error discovery
 */
extern int is_program_found;
extern int has_encountered_error;

/* Used for error reporting
 */
extern char * yyfilename;

extern int eaxhla_init(void);
extern int eaxhla_deinit(void);

extern int variable_size_sum(void);

// Language constructs
extern void add_program(const char * const name);
extern void add_scope(const char * const name);
extern void add_variable(unsigned type, const char * const name);
extern void add_variable_with_value(unsigned type, const char * const name, size_t value);
extern void add_array_variable(unsigned type, const char * const name, size_t size);
extern void add_array_variable_with_value(unsigned type, const char * const name, size_t size, void * value, size_t value_size);
extern void add_literal(void * data, size_t size);
extern void add_label(const char * const name, int is_resolved);
extern void add_procedure(const char * const name);
extern void add_fastcall(const char * const destination);
extern void fin_procedure(void);
extern void fin_hla(void);
/* Not implemented
extern symbol_t * add_function(symbol_t function);
extern symbol_t * get_function(const char * const name);
*/

// Asm value constructs
/* These functions MUST return a valid symbol_t or
 *  we segv and catch fire.
 * Unresolved symbol errors are handled internally.
 */
extern symbol_t * get_relative(const char * const name);
extern symbol_t * get_variable(const char * const name);

// XXX: move to assembler.h,
//  delete the switch bullshit, you can get away with artimetrics,
//  also typedef the D\d+ type
extern int type2size(int type);        // XXX: the return type is an anon enum
extern int size2bytes(const int size); // XXX: size is an anon enum

// Error reporting
extern void issue_warning(const char * format, ...);
extern void issue_error(const char * format, ...);

#endif
