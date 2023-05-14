#pragma once

#include <stddef.h>

void *heap_malloc(size_t size);
void *heap_calloc(size_t nitems, size_t size);
void *heap_realloc(void *ptr, size_t size);
void heap_free(void *ptr);