#pragma once

#include <stdint.h>

void abort(void);
int atexit(void (*)(void));
int atoi(const char *);
void free(void *);
char *getenv(const char *);
void *malloc(size_t);
void *calloc(size_t, size_t);
void exit(int status);
void _Exit(int status);
