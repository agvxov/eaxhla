#ifndef EAXHLA_H

#include <tommyds/tommyds/tommyhashtbl.h>

#define WORD_SIZE_IN_BYTES (64/8)

typedef enum {
    VARIABLE,
    FUNCTION,
} symbol_type_t;

typedef struct {
    symbol_type_t symbol_type;
    union {
        struct { // VARIABLE
            int type;
            unsigned long long elements;
            union {
                long   value;
                void * array_value;
            };
        };
        struct { // FUNCTION
            void * unused;
        };
    };
    char *     name;
    unsigned   _id;
    unsigned   _hash;
    tommy_node _node;
} symbol_t;

extern tommy_hashtable symbol_table;

// Used for naming variables constructed from literals
extern unsigned long long anon_variable_counter;

typedef struct {
    unsigned number : 6;
    unsigned size   : 2;
} cpuregister_t;

extern int system_type;

extern int is_program_found;
extern int has_encountered_error;

extern char * scope;
extern void empty_out_scope(void);

// Used for error reporting
extern char * yyfilename;

extern int eaxhla_init(void);
extern int eaxhla_deinit(void);

extern char * make_scoped_name(const char * const scope, const char * const name);
extern int can_fit(const int type, const long long value);
extern int validate_array_size(const int size);

extern void add_variable(symbol_t variable);
extern symbol_t * get_variable(const char * const name);

extern symbol_t * get_symbol(const char * const name);
//extern void add_function(symbol_t function);
extern void add_procedure(symbol_t procedure);
extern symbol_t * get_function(const char * const name);
extern void add_program(const char * const name);

extern void add_fastcall(const char * const destination);

extern int type2size(int type);
extern int size2bytes(const int size);
extern int variable_size_sum(void);

extern void issue_warning(const char * format, ...);
extern void issue_error(const char * format, ...);

#define EAXHLA_H
#endif
