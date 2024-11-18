#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    char   * buffer;
    size_t   count;
    size_t   capacity;
} arena_block;

typedef struct {
    size_t          block_count;
    size_t          block_limit;
    arena_block * * block_array;
} arena_data;

//~typedef struct {
    //~char   * buffer;
    //~size_t   count;
    //~size_t   capacity;
//~} arena_data;

extern arena_block * arena_init(size_t limit);

extern void arena_deinit(void);

extern void * arena_allocate(size_t size);

//~extern void * arena_deallocate(void * data);

//~extern void * arena_reallocate(void * data, size_t size);

#endif

#ifdef ARENA_IMPLEMENTATION

//~typedef struct {
    //~char   * buffer;
    //~size_t   count;
    //~size_t   capacity;
    //~void   * next;
//~} arena_data;

static arena_data * arena;
static int open_arena = 0;

void arena_deinit(void) {
    for (size_t index = 0; index < arena->block_count; ++index) {
        free(arena->block_array[index]->buffer);
        free(arena->block_array[index]);
    }

    free(arena->block_array);
    free(arena);
}

arena_block * arena_init(size_t limit) {
    arena_block * block = NULL;

    block = calloc(1, sizeof(arena_block));

    //~arena->block_count++;

    //~arena->block_array = realloc(arena->block_array, arena->block_count * sizeof(*arena->block_array));

    arena->block_array[arena->block_count - 1] = block;

    //~*(arena->block_array[arena->block_count - 1]) = calloc(1, sizeof (arena_block));

    arena->block_array[arena->block_count - 1]->buffer = calloc(limit, 1);

    return block;
}

void * arena_allocate(size_t size) {
    //~if (arena->count + size >= arena->limit) {
        //~return NULL;
    //~}

    //~if (arena->count + size >= arena->limit) {
        //~arena->limit *= 2;

        //~arena->array = realloc(arena->array, arena->limit);

        //~memset((void *)&arena->array[arena->limit / 2], 0, arena->limit / 2);
    //~}

    //~char * current = (char*)arena->next;

    //~while (current != NULL) {
        //~current = (char*)arena->next;
    //~}

    //~;

    if (open_arena == 0) {
        atexit(arena_deinit);
        open_arena = 1;
        arena = calloc(1, sizeof(*arena));
    }

    if (arena->block_count == 0) {
        arena->block_count++;
        arena->block_array = realloc(arena->block_array, arena->block_count * sizeof(*arena->block_array));
        arena->block_array[arena->block_count - 1] = arena_init(13);
    }

    size_t spot = arena->block_array[arena->block_count - 1]->count;

    arena->block_array[arena->block_count - 1]->count += size;

    return (void*)&arena->block_array[arena->block_count - 1]->buffer[spot];
}

//~void * arena_deallocate(void * data) {

//~}

//~void * arena_reallocate(void * data, size_t size) {

//~}

#endif
