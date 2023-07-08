#include <stdlib.h>
#include <string.h>

void *memcpy(void *restrict s1, const void *restrict s2, size_t n)
{
    char *p_s1 = (char *)s1;
    const char *p_s2 = (const char *)s2;

    for (size_t i = 0; i < n; i++)
        *p_s1++ = *p_s2++;

    return s1;
}

void *memset(void *s, int c, size_t n)
{
    unsigned char *p_s = (unsigned char *)s;
    for (size_t i = 0; i < n; i++)
        *p_s++ = (char)c;
    return s;
}

size_t strlen(const char *str)
{
    size_t length = 0;
    while (str[length])
        length++;
    return length;
}

size_t strnlen(const char *s, size_t maxlen)
{
    size_t length;
    for (length = 0; length < maxlen; length++)
    {
        if (s[length] == '\0')
            break;
    }
    return length;
}

char *strcpy(char *restrict s1, const char *restrict s2) { return memcpy(s1, s2, strlen(s2) + 1); }

char *strcat(char *restrict s1, const char *restrict s2)
{
    strcpy(s1 + strlen(s1), s2);
    return s1;
}

int strcmp(const char *str1, const char *str2)
{
    const unsigned char *p_str1 = (const unsigned char *)str1;
    const unsigned char *p_str2 = (const unsigned char *)str2;
    unsigned char c1;
    unsigned char c2;

    do
    {
        c1 = (unsigned char)*p_str1++;
        c2 = (unsigned char)*p_str2++;

        if (c1 == '\0')
            return c1 - c2;
    } while (c1 == c2);

    return c1 - c2;
}

char *strchr(const char *s, int c)
{
    do
    {
        if (*s == c)
            return (char *)s;
    } while (*s++);

    return NULL;
}

char *strdup(const char *s)
{
    size_t length = strlen(s) + 1;
    char *new_s = (char *)calloc(length, sizeof(char));
    if (!new_s)
        return NULL;

    return (char *)memcpy(new_s, s, length);
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    unsigned char c1;
    unsigned char c2;

    for (size_t i = 0; i < n; i++)
    {
        c1 = (unsigned char)s1[i];
        c2 = (unsigned char)s2[i];

        if (c1 != c2)
            return c1 - c2;

        if (c1 == '\0')
            return 0;
    }
    return 0;
}

char *strrchr(const char *s, int c)
{
    char *result = 0;

    do
    {
        if (*s == c)
            result = (char *)s;
    } while (*s++);

    return result;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p_s1 = (const unsigned char *)s1;
    const unsigned char *p_s2 = (const unsigned char *)s2;

    for (size_t i = 0; i < n; i++)
    {
        if (*p_s1++ != *p_s2++)
            return p_s1[-1] < p_s2[-1] ? -1 : 1;
    }

    return 0;
}

void *memmove(void *s1, const void *s2, size_t n)
{
    char *p_s1 = (char *)s1;
    const char *p_s2 = (const char *)s2;
    if (p_s1 < p_s2)
    {
        for (size_t i = 0; i < n; i++)
            *p_s1++ = *p_s2++;
    }
    else
    {
        char *last_s2 = p_s2 + (n - 1);
        char *last_s1 = p_s1 + (n - 1);

        for (size_t i = 0; i < n; i++)
            *last_s1-- = *last_s2--;
    }
    return s1;
}