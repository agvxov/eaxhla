#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>

// We only allocate memory, program is guaranteed to terminate.
extern void * aalloc(size_t size);

#endif

#ifdef ARENA_IMPLEMENTATION

#ifndef ARENA_CHUNK
// What memory chunk size would be enough for everyone...?
#define ARENA_CHUNK (640 * 1024)
#endif

static struct {
    size_t count;
    size_t limit;
    struct {
        char   * buffer;
        size_t   count;
        size_t   capacity;
    } * * array;
} * arena = NULL;

static
void arena_deinit(void) {
    for (size_t index = 0; index < arena->count; ++index) {
        free(arena->array[index]->buffer);
        free(arena->array[index]);
    }

    free(arena->array);
    free(arena);
}

static
void arena_reinit(void) {
    arena->count++;
    arena->array = realloc(arena->array, arena->count * sizeof(*arena->array));

    arena->array[arena->count - 1] = calloc(1, sizeof(*arena));

    arena->array[arena->count - 1]->buffer   = calloc(ARENA_CHUNK, 1);
    arena->array[arena->count - 1]->count    = 0;
    arena->array[arena->count - 1]->capacity = ARENA_CHUNK;
}

void * aalloc(size_t size) {
    size_t spot = 0;

    if (arena == NULL) {
        atexit(arena_deinit);
        arena = calloc(1, sizeof(*arena));
        arena_reinit();
    }

    if (arena->array[arena->count - 1]->count + size
    >=  arena->array[arena->count - 1]->capacity) {
        arena_reinit();
    }

    spot = arena->array[arena->count - 1]->count;

    arena->array[arena->count - 1]->count += size;

    return (void*)&arena->array[arena->count - 1]->buffer[spot];
}

#endif
