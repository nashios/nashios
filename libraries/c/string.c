#include <string.h>
#include <stdint.h>

void *memset(void *s, int c, size_t n)
{
    uint8_t *p_s = s;

    for (size_t i = 0; i < n; i++)
        p_s[i] = c;

    return s;
}

void *memcpy(void *restrict s1, const void *restrict s2, size_t n)
{
    char *p_s1 = s1;
    const char *p_s2 = s2;

    for (size_t i = 0; i < n; i++)
        p_s1[i] = p_s2[i];

    return s1;
}
