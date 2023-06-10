#include <sys/syscall.h>
#include <unistd.h>

int brk(void *addr) { return syscall(__NR_brk, addr); }

void *sbrk(intptr_t increment)
{
    void *value = (void *)brk(0);
    brk(value + increment);
    return value;
}

pid_t fork(void) { return syscall(__NR_fork); }
