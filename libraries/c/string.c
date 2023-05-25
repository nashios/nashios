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
