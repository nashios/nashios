/**
 * @file kernel/string.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Libc string implementation for kernel
 * @version 0.1
 * @date 2022-05-19
 *
 * @copyright Copyright (C) 2022 Saullo Bretas Silva
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include <kernel/stdlib.h>
#include <kernel/string.h>
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

void *memmove(void *s1, const void *s2, size_t n)
{
    char *p_s1 = s1;
    const char *p_s2 = s2;

    if (p_s1 < p_s2)
    {
        for (size_t i = 0; i < n; i++)
            p_s1[i] = p_s2[i];
    }
    else
    {
        char *last_s1 = p_s1 + (n - 1);
        const char *last_s2 = p_s2 + (n - 1);

        for (size_t i = 0; i < n; i++)
            last_s1[i] = last_s2[i];
    }

    return s1;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p_s1 = s1;
    const uint8_t *p_s2 = s2;

    for (size_t i = 0; i < n; i++)
    {
        if (*p_s1 != *p_s2)
            return *p_s1 - *p_s2;

        p_s1++;
        p_s2++;
    }

    return 0;
}

size_t strlen(const char *str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

size_t strnlen_s(const char *str, size_t strsz)
{
    const char *p_str = str;
    for (size_t i = 0; *p_str && i < strsz; i++)
        p_str++;
    return p_str - str;
}

int strcmp(const char *s1, const char *s2)
{
    const uint8_t *p_s1 = (const uint8_t *)s1;
    const uint8_t *p_s2 = (const uint8_t *)s2;
    uint8_t ch1;
    uint8_t ch2;

    do
    {
        ch1 = *p_s1++;
        ch2 = *p_s2++;

        if (ch1 == '\0')
            break;
    } while (ch1 == ch2);

    return ch1 - ch2;
}

char *strdup(const char *s)
{
    size_t length = strlen(s) + 1;
    void *buffer = calloc(length, sizeof(char));
    if (!buffer)
        return NULL;

    return memcpy(buffer, s, length);
}
