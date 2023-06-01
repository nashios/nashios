#pragma once

#include <locale.h>
#include <stddef.h>
#include <sys/cdefs.h>

___BEGIN_DECLS

size_t strlen(const char *str);
size_t strnlen(const char *s, size_t maxlen);
int strcmp(const char *str1, const char *str2);
char *strcpy(char *s1, const char *s2);
char *strcat(char *s1, const char *s2);
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *str, int c, size_t n);
void *memchr(const void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memmove(void *s1, const void *s2, size_t n);
int strcoll(const char *s1, const char *s2);
size_t strcspn(const char *s1, const char *s2);
char *strrchr(const char *s, int c);
char *strchr(const char *s, int c);
char *strerror(int errnum);
char *strerror_l(int errnum, locale_t locale);
int strerror_r(int errnum, char *strerrbuf, size_t buflen);
char *strncat(char *s1, const char *s2, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *s1, const char *s2, size_t n);
size_t strspn(const char *s1, const char *s2);
char *strtok(char *s1, const char *s2);
char *strtok_r(char *s, const char *sep, char **lasts);
size_t strxfrm(char *s1, const char *s2, size_t n);
char *strpbrk(const char *s1, const char *s2);
char *strstr(const char *s1, const char *s2);

___END_DECLS
