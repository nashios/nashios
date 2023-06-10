#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#include <bits/sys/stat.h>

#if __cplusplus
extern "C"
{
#endif

    /**
     * @brief Get file status
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/fstat.html
     *
     * @param fildes The file descriptor of the file to get the status of
     * @param buf The buffer to store the status in
     * @return int On success, zero is returned. On error, -1 is returned and errno is set appropriately.
     */
    int fstat(int fildes, struct stat *buf);

#if __cplusplus
}
#endif

#endif
