#include <sys/mman.h>
#include <sys/syscall.h>

void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
    return syscall(__NR_mmap, addr, len, prot, flags, fildes, off);
}
