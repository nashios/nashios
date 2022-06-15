#include <unistd.h>
#include <syscall.h>

_syscall1(brk, void *);
int brk(void *addr)
{
    SYSCALL_RETURN(syscall_brk(addr));
}

void *sbrk(intptr_t increment)
{
    void *address = (void *)brk(0x0);
    if (increment > 0)
        brk(address + increment);
    return address;
}

_syscall1(exit, int);
void _exit(int status)
{
    syscall_exit(status);
    __builtin_unreachable();
}
