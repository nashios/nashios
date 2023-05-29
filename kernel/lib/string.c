#include <kernel/lib/stdlib.h>
#include <kernel/lib/string.h>

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

int strncmp(const char *str1, const char *str2, size_t n)
{
    unsigned char p_s1;
    unsigned char p_s2;

    for (size_t i = 0; i < n; i++)
    {
        p_s1 = (unsigned char)*str1++;
        p_s2 = (unsigned char)*str2++;

        if (p_s1 != p_s2)
            return p_s1 - p_s2;

        if (p_s1 == '\0')
            return 0;
    }

    return 0;
}

char *strdup(const char *string)
{
    size_t length = strlen(string) + 1;
    char *new_string = (char *)calloc(length, sizeof(char));
    if (!new_string)
        return NULL;
    return (char *)memcpy(new_string, string, length);
}

char *strrstr(const char *haystack, const char *needle)
{
    size_t haystack_length = strlen(haystack);
    size_t needle_length = strlen(needle);
    if (needle_length > haystack_length)
        return NULL;

    for (const char *result = haystack + haystack_length - needle_length; result >= haystack; result--)
    {
        if (strncmp(result, needle, needle_length) == 0)
            return (char *)result;
    }

    return NULL;
}

char *strncpy(char *s1, const char *s2, size_t n)
{
    size_t size = strnlen(s2, n);
    if (size != n)
        memset(s1 + size, '\0', n - size);

    return memcpy(s1, s2, size);
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
