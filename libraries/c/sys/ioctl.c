#include <stdarg.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>

int ioctl(int fd, unsigned long request, ...)
{
    va_list ap;
    va_start(ap, request);
    void* arg = va_arg(ap, void*);
    va_end(ap);

    return syscall(__NR_ioctl, fd, request, arg);
}
