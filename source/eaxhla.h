#ifndef EAXHLA_H

#include <tommyds/tommyds/tommyhashtbl.h>

#define WORD_SIZE_IN_BYTES (64/8)

typedef struct {
  union {
      long   value;
      void * array_value;
  };
  unsigned long long elements;
  char *     name;
  int        type;
  unsigned   _id;
  unsigned   _hash;
  tommy_node _node;
} variable_t;

extern tommy_hashtable variable_table;

extern unsigned long long anon_variable_counter;

typedef struct {
    unsigned number : 6;
    unsigned size   : 2;
} cpuregister_t;

extern int system_type;

extern int is_program_found;
extern int has_encountered_error;

extern char * scope;

// Used for error reporting
extern char * yyfilename;

extern int eaxhla_init(void);
extern int eaxhla_destroy(void);

extern char * make_scoped_name(const char * const scope, char * name);
extern int can_fit(int type, long long value);
extern int validate_array_size(int size);

extern void add_variable(variable_t variable);
extern variable_t * get_variable(const char * const name);

extern int type2size(int type);

extern void issue_warning(const char * format, ...);
extern void issue_error(const char * format, ...);

#define EAXHLA_H
#endif
