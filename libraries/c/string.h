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

    /**
     * @brief Copy the string pointed to by s2 into the object pointed to by s1
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/strcpy.html
     *
     * @param s1 The pointer to the destination array where the content is to be copied
     * @param s2 The string to be copied
     * @return char* A pointer to the destination string s1
     */
    char *strcpy(char *restrict s1, const char *restrict s2);

    /**
     * @brief Get length of string
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/strlen.html
     *
     * @param s The string to get the length of
     * @return size_t The length of the string
     */
    size_t strlen(const char *s);

    /**
     * @brief Get length of string limited by maxlen
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/strnlen.html
     *
     * @param s The string to get the length of
     * @param maxlen The maximum number of characters to check
     * @return size_t The length of the string
     */
    size_t strnlen(const char *s, size_t maxlen);

    /**
     * @brief Concatenate strings
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/strcat.html
     *
     * @param s1 The pointer to the destination array, which should contain a C string, and be large enough to contain
     * the concatenated resulting string
     * @param s2 The string to be appended
     * @return char* A pointer to the resulting string s1
     */
    char *strcat(char *restrict s1, const char *restrict s2);

    /**
     * @brief Compare two strings
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/strcmp.html
     *
     * @param str1 The first string to compare
     * @param str2 The second string to compare
     * @return int Returns an integral value indicating the relationship between the strings:
     */
    int strcmp(const char *str1, const char *str2);

    /**
     * @brief String scanning operation
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/strchr.html
     *
     * @param s The string to be scanned
     * @param c The character to search for
     * @return char* A pointer to the first occurrence of the character c in the string s
     */
    char *strchr(const char *s, int c);

    /**
     * @brief Duplicate a string
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/strdup.html
     *
     * @param s The string to duplicate
     * @return char* A pointer to a new string, which is a duplicate of the string s
     */
    char *strdup(const char *s);

    /**
     * @brief Compare part of two strings
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/strncmp.html
     *
     * @param s1 First string to compare
     * @param s2 Second string to compare
     * @param n Number of characters to compare
     * @return int Returns an integral value indicating the relationship between the strings:
     */
    int strncmp(const char *s1, const char *s2, size_t n);

    /**
     * @brief String scanning operation
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/strrchr.html
     *
     * @param s The string to be scanned
     * @param c The character to search for
     * @return char* A pointer to the last occurrence of the character c in the string s
     */
    char *strrchr(const char *s, int c);

    /**
     * @brief Compare byte in memory
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/memcmp.html
     *
     * @param s1 The first memory block to compare
     * @param s2 The second memory block to compare
     * @param n The number of bytes to compare
     * @return int Returns an integral value indicating the relationship between the strings:
     */
    int memcmp(const void *s1, const void *s2, size_t n);

    /**
     * @brief Copy bytes in memory with overlapping areas
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/memmove.html
     *
     * @param s1 The destination memory block
     * @param s2 The source memory block
     * @param n The number of bytes to copy
     * @return void* A pointer to the destination memory block
     */
    void *memmove(void *s1, const void *s2, size_t n);

#ifdef __cplusplus
}
#endif

#endif
