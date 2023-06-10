#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void abort(void);
    int atexit(void (*)(void));
    int atoi(const char *);
    void free(void *);
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

#ifdef __cplusplus
}
#endif

#endif
