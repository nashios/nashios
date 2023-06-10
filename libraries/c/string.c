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
}

char *strcpy(char *restrict s1, const char *restrict s2) { return memcpy(s1, s2, strlen(s2) + 1); }
