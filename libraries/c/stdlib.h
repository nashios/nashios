#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Generate an abnormal program termination.
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/abort.html
     */
    void abort(void);
    int atexit(void (*)(void));
    int atoi(const char *);
    char *getenv(const char *);

    /**
     * @brief Allocate a block of memory.
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html
     *
     * @param size The size of the memory block to allocate.
     * @return void* A pointer to the allocated memory block.
     */
    void *malloc(size_t size);

    /**
     * @brief Allocate a block of memory and set it to zero.
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/calloc.html
     *
     * @param nitems The number of items to allocate.
     * @param size The size of each item.
     * @return void* A pointer to the allocated memory block.
     */
    void *calloc(size_t nitems, size_t size);

    /**
     * @brief Reallocate a block of memory.
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/realloc.html
     *
     * @param ptr A pointer to the memory block to reallocate.
     * @param size The new size of the memory block.
     * @return void* A pointer to the reallocated memory block.
     */
    void *realloc(void *ptr, size_t size);

    /**
     * @brief Free a block of memory.
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html
     *
     * @param ptr A pointer to the memory block to free.
     */
    void free(void *ptr);

    /**
     * @brief Exit the program with the given status.
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/exit.html
     *
     * @param status The exit status.
     */
    void exit(int status);

    /**
     * @brief Return an integer absolute value.
     *
     * @param i The integer to get the absolute value of.
     * @return int The absolute value of i.
     */
    int abs(int i);

    /**
     * @brief Convert a string to a unsigned long integer.
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/strtoul.html
     *
     * @param str The string to convert.
     * @param endptr The address of a pointer to set to the first invalid character.
     * @param base The base to use for the conversion.
     * @return unsigned long The converted value.
     */
    unsigned long strtoul(const char *restrict str, char **restrict endptr, int base);

#ifdef __cplusplus
}
#endif

#endif
