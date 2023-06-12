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
