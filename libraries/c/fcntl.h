#ifndef _FCNTL_H
#define _FCNTL_H

#include <bits/fcntl.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Open a file
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/open.html
     *
     * @param path The path to the file to open
     * @param oflag The flags to open the file with
     * @param ... The mode to open the file with
     * @return int On success, the file descriptor of the file is returned. On error, -1 is returned, and errno is set
     * appropriately.
     */
    int open(const char *path, int oflag, ...);

#ifdef __cplusplus
}
#endif

#endif
