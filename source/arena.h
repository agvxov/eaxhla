/*
xolatile@bunsenlabs:~/Git/eaxcc$ grep -rn malloc source/*
source/eaxhla.c:276:    r = malloc(2 + scl + 1 + nml + 1);
source/eaxhla.l:268:        yylval.blobval.data = malloc(sdslen(string_literal_buffer));
xolatile@bunsenlabs:~/Git/eaxcc$ grep -rn calloc source/*
source/arena.h:44:    arena->array[arena->count - 1] = calloc(1, sizeof(*arena));
source/arena.h:46:    arena->array[arena->count - 1]->buffer   = calloc(ARENA_CHUNK, 1);
source/arena.h:57:        arena = calloc(1, sizeof(*arena));
source/assembler.c:778:    empty_array = calloc(1024, sizeof(*empty_array));
source/assembler.c:779:    empty_imbue = calloc(1024, sizeof(*empty_imbue));
source/assembler.c:780:    empty_store = calloc(1024, sizeof(*empty_store));
source/compile.c:25:    token_array = calloc(1440UL, sizeof(*token_array));
source/compile.c:117:    text_sector_byte = calloc(4096UL, sizeof(*text_sector_byte));
source/eaxhla.c:149:    undeclared_symbol = (symbol_t *)calloc(sizeof(symbol_t), 1);
source/eaxhla.c:159:    r = (symbol_t *)calloc(sizeof(symbol_t), 1);
source/eaxhla.c:384:    symbol_t * literal = (symbol_t *)calloc(sizeof(symbol_t), 1);
xolatile@bunsenlabs:~/Git/eaxcc$ grep -rn realloc source/*
source/arena.h:42:    arena->array = realloc(arena->array, arena->count * sizeof(*arena->array));
xolatile@bunsenlabs:~/Git/eaxcc$ grep -rn free source/*
source/arena.h:31:        free(arena->array[index]->buffer);
source/arena.h:32:        free(arena->array[index]);
source/arena.h:35:    free(arena->array);
source/arena.h:36:    free(arena);
source/assembler.c:810:    free(empty_array);
source/assembler.c:811:    free(empty_imbue);
source/assembler.c:812:    free(empty_store);
source/compile.c:30:    free(token_array);
source/compile.c:130:    free(text_sector_byte);
source/compile.h:19: * A variadic argument interface would be ideal except theres not free
source/eaxhla.c:143:    free(scope);
source/eaxhla.c:165:void free_symbol(void * data) {
source/eaxhla.c:168:    free(variable->name);
source/eaxhla.c:172:        free(variable->array_value);
source/eaxhla.c:175:    free(variable);
source/eaxhla.c:181:    free(undeclared_symbol);
source/eaxhla.c:182:    tommy_hashtable_foreach(&symbol_table, free_symbol);
source/eaxhla.c:262:    free(scope);
source/eaxhla.c:296:    free(alternative_name);
source/eaxhla.c:403:        free(full_name);
source/eaxhla.c:499:    free(varname);
source/eaxhla.c:563:    free(msg);
source/eaxhla.c:583:    free(msg);
source/eaxhla.h:41:void free_symbol(void * name);
source/eaxhla.l:358:void yyfree_leftovers(void) {
source/eaxhla.l:365:    sdsfree(string_literal_buffer);
source/eaxhla.y:21:    extern void yyfree_leftovers(void);
source/eaxhla.y:120:        free($3);
source/eaxhla.y:142:        free($3);
source/eaxhla.y:158:        free($3);
source/eaxhla.y:162:        free($3);
source/eaxhla.y:166:        free($6);
source/eaxhla.y:170:        free($6);
source/eaxhla.y:174:        free($5);
source/eaxhla.y:200:        free($1);
source/eaxhla.y:216:        free($2);
source/eaxhla.y:224:        free($1);
source/eaxhla.y:233:        free($1);
source/eaxhla.y:255:        free($1);
source/eaxhla.y:331:    | machine_code IDENTIFIER { free($2); }
source/eaxhla.y:336:        free($2);
source/eaxhla.y:341:    | IDENTIFIER       arguments { free($1); }
source/main.c:18:    extern void yyfree_leftovers(void);
source/main.c:20:    yyfree_leftovers();
xolatile@bunsenlabs:~/Git/eaxcc$
*/

#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>

extern void * aalloc(size_t size);

#endif

#ifdef ARENA_IMPLEMENTATION

#include <string.h>

#ifndef ARENA_CHUNK
#define ARENA_CHUNK (4096)
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
        open_arena = 1;
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
