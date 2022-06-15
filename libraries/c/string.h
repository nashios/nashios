#pragma once

#include <stddef.h>

void *memcpy(void *restrict s1, const void *restrict s2, size_t n);
void *memset(void *s, int c, size_t n);
char *strcpy(char *, const char *);
size_t strlen(const char *);
size_t strlen(const char *str);
char *strerror(int errnum);
char *strchr(const char *s, int c);
