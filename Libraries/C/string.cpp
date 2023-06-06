#include <assert.h>
#include <string.h>

extern "C"
{
    void *memcpy(void *__restrict, const void *__restrict, size_t) { assert(false); }

    void *memmove(void *, const void *, size_t) { assert(false); }

    char *strcpy(char *__restrict, const char *) { assert(false); }

    char *strncpy(char *__restrict, const char *, size_t) { assert(false); }

    char *strcat(char *__restrict, const char *__restrict) { assert(false); }

    char *strncat(char *__restrict, const char *__restrict, size_t) { assert(false); }

    int memcmp(const void *, const void *, size_t) { assert(false); }

    int strcmp(const char *, const char *) { assert(false); }

    int strcoll(const char *, const char *) { assert(false); }

    int strncmp(const char *, const char *, size_t) { assert(false); }

    size_t strxfrm(char *__restrict, const char *__restrict, size_t) { assert(false); }

    void *memchr(const void *, int, size_t) { assert(false); }

    char *strchr(const char *, int) { assert(false); }

    size_t strcspn(const char *, const char *) { assert(false); }

    char *strpbrk(const char *, const char *) { assert(false); }

    char *strrchr(const char *, int) { assert(false); }

    size_t strspn(const char *, const char *) { assert(false); }

    char *strstr(const char *, const char *) { assert(false); }

    char *strtok(char *__restrict, const char *__restrict) { assert(false); }

    char *strchrnul(const char *, int) { assert(false); }

    void *memset(void *, int, size_t) { assert(false); }

    char *strerror(int) { assert(false); }

    size_t strlen(const char *) { assert(false); }

    int strerror_r(int, char *, size_t) { assert(false); }

    void *mempcpy(void *, const void *, size_t) { assert(false); }

    int strverscmp(const char *, const char *) { assert(false); }

    int ffsl(long) { assert(false); }

    int ffsll(long long) { assert(false); }

    void *memmem(const void *, size_t, const void *, size_t) { assert(false); }

    char *strdup(const char *) { assert(false); }

    char *strndup(const char *, size_t) { assert(false); }

    size_t strnlen(const char *, size_t) { assert(false); }

    char *strtok_r(char *__restrict, const char *__restrict, char **__restrict) { assert(false); }

    char *strsep(char **, const char *) { assert(false); }

    char *strsignal(int) { assert(false); }

    char *stpcpy(char *__restrict, const char *__restrict) { assert(false); }

    char *stpncpy(char *__restrict, const char *__restrict, size_t) { assert(false); }

    void *memccpy(void *__restrict, const void *__restrict, int, size_t) { assert(false); }

    int strcoll_l(const char *, const char *, locale_t) { assert(false); }

    char *strcasestr(const char *, const char *) { assert(false); }

    char *strdupa(const char *) { assert(false); }

    char *strndupa(const char *, size_t) { assert(false); }

    size_t strlcpy(char *, const char *, size_t) { assert(false); }

    size_t strlcat(char *, const char *, size_t) { assert(false); }
}
