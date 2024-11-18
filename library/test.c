#define ARENA_IMPLEMENTATION
#define ARENA_BLOCK_ALLOCATION_SIZE (13)

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

	char * c;
	c = arena_allocate (6);
	strcpy(c, "Cyaa\n");
	printf("%s", c);

	char * d;
	d = arena_allocate (10);
	strcpy(d, "Holy crap\n");
	printf("%s", d);

	char * e;
	e = arena_allocate (8);
	strcpy(e, "Serbia\n");
	printf("%s", e);

	char * f;
	f = arena_allocate (6);
	strcpy(f, "Base\n");
	printf("%s", f);

	return (0);
}
