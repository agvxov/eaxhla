#ifndef EAXHLA_H

#include <tommyds/tommyhashtbl.h>

#define WORD_SIZE_IN_BYTES (64/8)

typedef struct {
  int        size;
  long       value;
  char *     name;
  unsigned   _hash;
  tommy_node _node;
} variable_t;

extern char * scope;

extern int eaxhla_init(void);
extern int eaxhla_destroy(void);

extern char * make_scoped_name(const char * const scope, char * name);
extern int can_fit(int type, long long value);

extern void add_variable(variable_t variable);
extern variable_t * get_variable(const char * const name);

extern void issue_warning(const char * format, ...);
extern void issue_error(const char * format, ...);

extern int is_program_found;

#define EAXHLA_H
#endif
