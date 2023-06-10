#ifndef _UNISTD_H
#define _UNISTD_H

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    int execv(const char *, char *const[]);

    /**
     * @brief Execute a file
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/exec.html
     *
     * @param path The path to the file to execute
     * @param argv The arguments to pass to the file
     * @param envp The environment variables to pass to the file
     * @return int On success, execve() does not return, on error -1 is returned, and errno is set appropriately.
     */
    int execve(const char *path, char *const argv[], char *const envp[]);
    int execvp(const char *, char *const[]);

    /**
     * @brief Create a new process
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/fork.html
     *
     * @return pid_t On success, the PID of the child process is returned in the parent, and 0 is returned in the child.
     */
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
