/* @BAKE gcc -fsanitize=address,undefined -Wall -Wextra -Wpedantic table.c -I . -I ../library/tommyds/ -DTEST_TABLE ../library/tommyds/tommy.o && ./a.out @STOP */
#include "table.h"

#include <string.h>

#if 0
int table_compare_string(const void * arg, const void * obj) {
  return strcmp((char *) arg, (char *) obj);
}
#endif

int table_compare_unsigned(const void * arg, const void * obj) {
  return *(const unsigned *) arg != ((const variable_t*)obj)->hash;
}

#ifdef TEST_TABLE
#include <stdio.h>

int main (void) {
  /* table setup */
  tommy_hashtable tbl;
  tommy_hashtable_init(&tbl, 256);
  /* object setup */
  variable_t * var = malloc(sizeof(variable_t));
  var->size = 0;
  var->address = 0xDEADBABE;
  var->hash = tommy_strhash_u32(0, "testvar");
  /* insert object into table */
  tommy_hashtable_insert(&tbl, &var->node, var, var->hash);
  /* searches and prints the value of the addr */
  unsigned findme = tommy_strhash_u32(0, "testvar");
  variable_t * found = tommy_hashtable_search(&tbl, table_compare_unsigned, &findme, findme);
  if (found) { printf("found %lx\n", found->address); }
  else { printf("did not find %u from %u\n", findme, var->hash); }
  /* frees */
  tommy_hashtable_done(&tbl);
  free(var);
}
#endif
