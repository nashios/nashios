#include <stdarg.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>

extern "C"
{
    _syscall3(ioctl, int, unsigned long, unsigned long);
    int ioctl(int fd, unsigned long request, ...)
    {
        va_list ap;
        va_start(ap, request);
        unsigned long arg = va_arg(ap, unsigned long);
        va_end(ap);

        SYSCALL_RETURN(syscall_ioctl(fd, request, arg));
    }
}
