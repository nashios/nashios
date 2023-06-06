#include <assert.h>
#include <sys/ioctl.h>

extern "C"
{
    int ioctl(int, unsigned long, ...) { assert(false); }
}
