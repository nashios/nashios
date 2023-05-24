#include <sys/syscall.h>
#include <unistd.h>

_syscall1(exit, int);
void __attribute__((noreturn)) _exit(int status)
{
    syscall_exit(status);
    __builtin_unreachable();
}
