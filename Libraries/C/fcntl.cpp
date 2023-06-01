#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/types.h>

extern "C"
{
    _syscall3(open, const char *, int, mode_t);
    int open(const char *path, int oflag, ...)
    {
        mode_t mode = 0;
        SYSCALL_RETURN(syscall_open(path, oflag, mode));
    }
}
