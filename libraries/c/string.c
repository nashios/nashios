#include <string.h>

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

void *memcpy(void *dest, const void *src, size_t n)
{
    char *p_dest = (char *)dest;
    const char *p_src = (const char *)src;
    for (size_t i = 0; i < n; i++)
        *p_dest++ = *p_src++;
    return dest;
}

void *memset(void *str, int c, size_t n)
{
    unsigned char *p_str = (unsigned char *)str;
    for (size_t i = 0; i < n; i++)
        *p_str++ = (char)c;
    return str;
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
        while (n--)
            *p_s1++ = *p_s2++;
    else
    {
        char *last_s2 = (char *)p_s2 + (n - 1);
        char *last_s1 = (char *)p_s1 + (n - 1);
        while (n--)
            *last_s1-- = *last_s2--;
    }
    return s1;
}

char *strcpy(char *s1, const char *s2) { return memcpy(s1, s2, strlen(s2) + 1); }

char *strcat(char *s1, const char *s2)
{
    strcpy(s1 + strlen(s1), s2);
    return s1;
}
