#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>

extern void * arena_allocate(size_t size);

#endif

#ifdef ARENA_IMPLEMENTATION

#include <string.h>

#ifndef AREA_BLOCK_ALLOCATION_MAX_CHUNK_SIZE
#define AREA_BLOCK_ALLOCATION_MAX_CHUNK_SIZE (4096)
#endif

//~typedef struct {
    //~char   * buffer;
    //~size_t   count;
    //~size_t   capacity;
//~} arena_block;

//~typedef struct {
    //~size_t          block_count;
    //~size_t          block_limit;
    //~arena_block * * block_array;
//~} arena_data;

//~static arena_data * arena;

static struct {
    size_t          block_count;
    size_t          block_limit;
    struct {
        char   * buffer;
        size_t   count;
        size_t   capacity;
    } * * block_array;
} * arena;

static int open_arena = 0;

static
void arena_deinit(void) {
    for (size_t index = 0; index < arena->block_count; ++index) {
        free(arena->block_array[index]->buffer);
        free(arena->block_array[index]);
    }

    free(arena->block_array);
    free(arena);
}

static
void * arena_init(size_t limit) {
    void * block = NULL;

    block = calloc(1, 24);

    arena->block_array[arena->block_count - 1] = block;

    arena->block_array[arena->block_count - 1]->buffer   = calloc(limit, 1);
    arena->block_array[arena->block_count - 1]->count    = 0;
    arena->block_array[arena->block_count - 1]->capacity = limit;

    return block;
}

void * arena_allocate(size_t size) {
    if (open_arena == 0) {
        atexit(arena_deinit);
        open_arena = 1;
        arena = calloc(1, sizeof(*arena));
        arena->block_count++;
        arena->block_array = realloc(arena->block_array, arena->block_count * sizeof(*arena->block_array));
        arena->block_array[arena->block_count - 1] = arena_init(AREA_BLOCK_ALLOCATION_MAX_CHUNK_SIZE);
    }

    if (arena->block_array[arena->block_count - 1]->count + size >= arena->block_array[arena->block_count - 1]->capacity) {
        arena->block_count++;
        arena->block_array = realloc(arena->block_array, arena->block_count * sizeof(*arena->block_array));
        arena->block_array[arena->block_count - 1] = arena_init(AREA_BLOCK_ALLOCATION_MAX_CHUNK_SIZE);
    }

    size_t spot = arena->block_array[arena->block_count - 1]->count;

    arena->block_array[arena->block_count - 1]->count += size;

    return (void*)&arena->block_array[arena->block_count - 1]->buffer[spot];
}

#endif
