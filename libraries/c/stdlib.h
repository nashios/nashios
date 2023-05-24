#pragma once

#include <stddef.h>
#include <stdint.h>

void abort(void);
int atexit(void (*)(void));
int atoi(const char *);
void free(void *);
char *getenv(const char *);
void *malloc(size_t size);
void *calloc(size_t nelem, size_t elsize);
void exit(int status);
void _Exit(int status);
