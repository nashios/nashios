#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>

void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
    return (void *)syscall(__NR_mmap, addr, len, prot, flags, fildes, off);
}

int munmap(void *addr, size_t len) { return syscall(__NR_munmap, addr, len); }

int shm_open(const char *name, int oflag, mode_t mode)
{
    static const char default_dir[] = "/dev/shm/";

    char *p_name;
    if (name[0] == '/')
        p_name = (char *)name;
    else
    {
        p_name = calloc(strlen(name) + sizeof(default_dir), sizeof(char));
        strcpy(p_name, default_dir);
        strcat(p_name, name);
    }

    return open(p_name, oflag, mode);
}
