#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>

extern "C"
{
    _syscall5(mmap, void *, size_t, int, int, int);
    void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t)
    {
        SYSCALL_RETURN_PTR(syscall_mmap(addr, len, prot, flags, fildes));
    }

    _syscall2(munmap, void *, size_t);
    int munmap(void *addr, size_t len) { SYSCALL_RETURN(syscall_munmap(addr, len)); }

    int shm_open(const char *name, int oflag, mode_t mode)
    {
        static const char default_dir[] = "/dev/shm/";

        char *p_name;
        if (name[0] == '/')
            p_name = (char *)name;
        else
        {
            p_name = (char *)calloc(strlen(name) + sizeof(default_dir), sizeof(char));
            strcpy(p_name, default_dir);
            strcat(p_name, name);
        }

        return open(p_name, oflag, mode);
    }
}
