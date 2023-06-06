#pragma once

#include <stddef.h>

size_t strlen(const char *str);
size_t strnlen(const char *s, size_t maxlen);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);
char *strdup(const char *string);
char *strrstr(const char *haystack, const char *needle);
char *strncpy(char *s1, const char *s2, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *str, int c, size_t n);
