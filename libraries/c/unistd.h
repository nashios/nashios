#ifndef _UNISTD_H
#define _UNISTD_H

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    int execv(const char *, char *const[]);
    int execve(const char *, char *const[], char *const[]);
    int execvp(const char *, char *const[]);
    pid_t fork(void);

    /**
     * @brief Change the data segment size
     *
     * @note See more at: https://man7.org/linux/man-pages/man2/brk.2.html
     *
     * @param addr The new end of the data segment
     * @return int On sucess, returns zero. On error, -1 is returned, and errno is set to ENOMEM.
     */
    int brk(void *addr);

    /**
     * @brief Increase program data space
     * 
     * @note See more at: https://man7.org/linux/man-pages/man2/brk.2.html
     *
     * @param increment The number of bytes to increase the data segment by, 0 to return the current location of the
     * program break
     * @return void* On success, returns the previous program break. On error, (void*) -1 is returned, and errno is set
     */
    void *sbrk(intptr_t increment);

#ifdef __cplusplus
}
#endif

#endif
