#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Copy n bytes from the object pointed to by s2 into the object pointed to by s1
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/memcpy.html
     *
     * @param s1 Pointer to the destination where the content is to be copied, type-casted to a pointer of type void*
     * @param s2 Pointer to the source of data to be copied, type-casted to a pointer of type void*
     * @param n Number of bytes to copy
     *
     * @return void* A pointer to the destination, which is s1
     */
    void *memcpy(void *restrict s1, const void *restrict s2, size_t n);

    /**
     * @brief Set n bytes of s to the value of c
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/memset.html
     *
     * @param s The pointer to the block of memory to fill
     * @param c The value to be set
     * @param n The number of bytes to be set to the value
     * @return void* A pointer to the memory area s
     */
    void *memset(void *s, int c, size_t n);
    char *strcpy(char *, const char *);
    size_t strlen(const char *);

#ifdef __cplusplus
}
#endif

#endif
