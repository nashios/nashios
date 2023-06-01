#include <stddef.h>
#include <sys/syscall.h>
#include <unistd.h>

extern "C"
{
    char **environ = NULL;

    _syscall1(exit, int);
    void __attribute__((noreturn)) _exit(int status)
    {
        syscall_exit(status);
        __builtin_unreachable();
    }

    _syscall0(fork);
    pid_t fork(void) { SYSCALL_RETURN(syscall_fork()); }

    int execl(const char *, const char *, ...) { return -1; }

    int execle(const char *, const char *, ...) { return -1; }

    int execlp(const char *, const char *, ...) { return -1; }

    int execv(const char *, char *const[]) { return -1; }

    _syscall3(execve, const char *, char *const *, char *const *);
    int execve(const char *path, char *const argv[], char *const envp[])
    {
        SYSCALL_RETURN(syscall_execve(path, argv, envp));
    }

    int execvp(const char *, char *const[]) { return -1; }

    int fexecve(int, char *const[], char *const[]) { return -1; }

    _syscall1(brk, void *);
    int brk(void *addr) { SYSCALL_RETURN(syscall_brk(addr)); }

    void *sbrk(intptr_t incr)
    {
        auto value = brk(0);
        brk((void *)(value + incr));
        return (void *)value;
    }

    _syscall3(read, int, void *, size_t);
    ssize_t read(int fildes, void *buf, size_t nbyte) { SYSCALL_RETURN(syscall_read(fildes, buf, nbyte)); }

    _syscall1(close, int);
    int close(int fildes) { SYSCALL_RETURN(syscall_close(fildes)); }

    _syscall2(ftruncate, int, off_t);
    int ftruncate(int fildes, off_t length) { SYSCALL_RETURN(syscall_ftruncate(fildes, length)); }

    _syscall2(truncate, const char *, off_t);
    int truncate(const char *path, off_t length) { SYSCALL_RETURN(syscall_truncate(path, length)); }

    _syscall0(getpid);
    pid_t getpid(void) { SYSCALL_RETURN(syscall_getpid()); }
}
