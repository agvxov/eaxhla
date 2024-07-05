#ifndef TABLE_H_

#include <tommyds/tommytrie.h>

typedef struct {
  short   size;
  int   * hash;
  void  * data;
  long    address;

  tommy_node node;
} value_table_t;

#define TABLE_H_
#endif
