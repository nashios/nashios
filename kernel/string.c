#include <kernel/string.h>

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

void *memcpy(void *dest, const void *src, size_t n)
{
    char *p_dest = (char *)dest;
    const char *p_src = (const char *)src;
    for (size_t i = 0; i < n; i++)
        *p_dest++ = *p_src++;
    return dest;
}