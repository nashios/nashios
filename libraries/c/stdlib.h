#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct malloc_block
{
    bool allocated;
    size_t size;
    struct malloc_block *next;
};

void abort(void);
int atexit(void (*)(void));
int atoi(const char *);
void free(void *ptr);
char *getenv(const char *);
void *malloc(size_t size);
void *calloc(size_t nelem, size_t elsize);
void exit(int status);
void _Exit(int status);
