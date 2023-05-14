#include <kernel/memory/heap.h>
#include <kernel/stdlib.h>

void *malloc(size_t size) { return heap_malloc(size); }

void *calloc(size_t nitems, size_t size) { return heap_calloc(nitems, size); }

void *realloc(void *ptr, size_t size) { return heap_realloc(ptr, size); }

void free(void *ptr) { heap_free(ptr); }
