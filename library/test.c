#define ARENA_IMPLEMENTATION
#include "arena.h"
#include <stdio.h>

int main (void) {
	char * a;
	a = arena_allocate (12);
	strcpy(a, "Heyo world\n");
	printf("%s", a);
	char * b;
	b = arena_allocate (6);
	strcpy(b, "Heyo\n");
	printf("%s", b);
	return (0);
}
