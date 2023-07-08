#include <fcntl.h>
#include <stdarg.h>
#include <sys/syscall.h>
#include <sys/types.h>

int open(const char *path, int oflag, ...)
{
    mode_t mode = 0;
    if (oflag & O_CREAT)
    {
        va_list args;
        va_start(args, oflag);
        mode = va_arg(args, mode_t);
        va_end(args);
    }

    return syscall(__NR_open, path, oflag, mode);
}

int fcntl(int fildes, int cmd, ...)
{
    va_list ap;
    va_start(ap, cmd);
    unsigned long args = va_arg(ap, unsigned long);
    va_end(ap);

    return syscall(__NR_fcntl, fildes, cmd, args);
}
