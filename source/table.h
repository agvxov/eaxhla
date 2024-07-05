#ifndef TABLE_H_

#include <tommyds/tommytrie.h>

typedef struct {
  /* size_t   size; */
  /* int    * hash; */
  /* void   * data; */

  tommy_node node;
} value_table_t;

#define TABLE_H_
#endif
