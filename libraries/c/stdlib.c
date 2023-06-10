#include <stdbool.h>
#include <stdlib.h>
#include <sys/syscall.h>

void exit(int status)
{
    syscall(__NR_exit_group, status);
    while (true)
        syscall(__NR_exit, status);
}
