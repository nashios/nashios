#include <sys/syscall.h>
#include <unistd.h>

_syscall1(exit, int);
void __attribute__((noreturn)) _exit(int status)
{
    syscall_exit(status);
    __builtin_unreachable();
}

_syscall0(fork);
pid_t fork(void) { SYSCALL_RETURN(syscall_fork()); }
