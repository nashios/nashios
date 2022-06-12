#include <unistd.h>
#include <syscall.h>

_syscall1(exit, int) void _exit(int status)
{
    syscall_exit(status);
    __builtin_unreachable();
}
