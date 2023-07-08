#ifndef _UNISTD_H
#define _UNISTD_H

#include <stddef.h>
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

    /**
     * @brief Close a file descriptor
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/close.html
     *
     * @param fildes The file descriptor to close
     * @return int On success, zero is returned. On error, -1 is returned, and errno is set appropriately.
     */
    int close(int fildes);

    /**
     * @brief Truncate a file to a specified length
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/ftruncate.html
     *
     * @param fildes The file descriptor of the file to truncate
     * @param length The length to truncate the file to
     * @return int On success, zero is returned. On error, -1 is returned, and errno is set appropriately.
     */
    int ftruncate(int fildes, off_t length);

    /**
     * @brief Get the process ID
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/getpid.html
     *
     * @return pid_t The process ID
     */
    pid_t getpid(void);

    /**
     * @brief Read from a file descriptor
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/read.html
     *
     * @param fildes The file descriptor to read from
     * @param buf The buffer to read into
     * @param nbyte The number of bytes to read
     * @return ssize_t On success, the number of bytes read is returned. On error, -1 is returned, and errno is set
     * appropriately.
     */
    ssize_t read(int fildes, void *buf, size_t nbyte);

    /**
     * @brief Write on a file descriptor
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/write.html
     *
     * @param fildes The file descriptor to write to
     * @param buf The buffer to write from
     * @param nbyte The number of bytes to write
     * @return ssize_t On success, the number of bytes written is returned. On error, -1 is returned, and errno is set
     * appropriately.
     */
    ssize_t write(int fildes, const void *buf, size_t nbyte);

    /**
     * @brief Duplicate an open file descriptor
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/dup.html
     *
     * @param fildes The file descriptor to duplicate
     * @return int On success, these system calls return the new file descriptor. On error, -1 is returned, and errno is
     * set appropriately.
     */
    int dup(int fildes);

    /**
     * @brief Duplicate an open file descriptor
     *
     * @param fildes The file descriptor to duplicate
     * @param fildes2 The file descriptor to duplicate to
     * @return int On success, these system calls return the new file descriptor. On error, -1 is returned, and errno is
     */
    int dup2(int fildes, int fildes2);

    /**
     * @brief Exit a process
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/exit.html
     *
     * @param status The exit status of the process
     */
    void _exit(int status);

#ifdef __cplusplus
}
#endif

#endif
