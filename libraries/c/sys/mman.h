#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <bits/sys/mman.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Map files or devices into memory
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/mmap.html
     *
     * @param addr The address at which to place the mapping
     * @param len The length of the mapping
     * @param prot The protection to place on the mapping
     * @param flags The flags to place on the mapping
     * @param fildes The file descriptor of the file to map
     * @param off The offset into the file to map
     * @return void* On success, mmap() returns a pointer to the mapped area. On error, the value MAP_FAILED is
     * returned, and errno is set appropriately.
     */
    void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);

    /**
     * @brief Open a shared memory object
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/shm_open.html
     *
     * @param name The name of the shared memory object to create or open
     * @param oflag The flags to use when opening the shared memory object
     * @param mode The permissions to use when creating the shared memory object
     * @return int On success, shm_open() returns a non-negative integer, otherwise, -1 is returned and errno is set to
     * indicate the error.
     */
    int shm_open(const char *name, int oflag, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif
