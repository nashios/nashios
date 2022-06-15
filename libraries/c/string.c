#include <string.h>
#include <stdint.h>

void *memset(void *s, int c, size_t n)
{
    uint8_t *p_s = s;

    for (size_t i = 0; i < n; i++)
        p_s[i] = c;

    return s;
}
