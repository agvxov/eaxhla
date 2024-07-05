#ifndef TABLE_H_

#include <tommyds/tommyhashtbl.h>

typedef struct {
  short      size;
  long       address;
  unsigned   hash;
  void     * data;
  tommy_node node;
} variable_t;

#define TABLE_H_
#endif
