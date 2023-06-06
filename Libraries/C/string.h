#ifndef _STRING_H
#define _STRING_H

#include <alloca.h>
#include <bits/null.h>
#include <bits/locale_t.h>
#include <bits/size_t.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void *memcpy(void *__restrict dest, const void *__restrict src, size_t size);
    void *memmove(void *dest, const void *src, size_t size);
    char *strcpy(char *__restrict dest, const char *src);
    char *strncpy(char *__restrict dest, const char *src, size_t max_size);
    char *strcat(char *__restrict dest, const char *__restrict src);
    char *strncat(char *__restrict dest, const char *__restrict src, size_t max_size);
    int memcmp(const void *a, const void *b, size_t size);
    int strcmp(const char *a, const char *b);
    int strcoll(const char *a, const char *b);
    int strncmp(const char *a, const char *b, size_t max_size);
    size_t strxfrm(char *__restrict dest, const char *__restrict src, size_t max_size);
    void *memchr(const void *s, int c, size_t size);
    char *strchr(const char *s, int c);
    size_t strcspn(const char *s, const char *chrs);
    char *strpbrk(const char *s, const char *chrs);
    char *strrchr(const char *s, int c);
    size_t strspn(const char *s, const char *chrs);
    char *strstr(const char *pattern, const char *s);
    char *strtok(char *__restrict s, const char *__restrict delimiter);
    char *strchrnul(const char *, int);
    void *memset(void *dest, int c, size_t size);
    char *strerror(int errnum);
    size_t strlen(const char *s);
    int strerror_r(int, char *, size_t);
    void *mempcpy(void *, const void *, size_t);
    int strverscmp(const char *l0, const char *r0);
    int ffsl(long i);
    int ffsll(long long i);
    void *memmem(const void *, size_t, const void *, size_t);
    char *strdup(const char *string);
    char *strndup(const char *, size_t);
    size_t strnlen(const char *, size_t);
    char *strtok_r(char *__restrict, const char *__restrict, char **__restrict);
    char *strsep(char **stringp, const char *delim);
    char *strsignal(int sig);
    char *stpcpy(char *__restrict, const char *__restrict);
    char *stpncpy(char *__restrict, const char *__restrict, size_t n);
    void *memccpy(void *__restrict dest, const void *__restrict src, int c, size_t n);
    int strcoll_l(const char *s1, const char *s2, locale_t locale);
    char *strcasestr(const char *, const char *);
    char *strdupa(const char *s);
    char *strndupa(const char *s, size_t n);
    size_t strlcpy(char *d, const char *s, size_t n);
    size_t strlcat(char *d, const char *s, size_t n);

#ifdef __cplusplus
}
#endif

#endif
