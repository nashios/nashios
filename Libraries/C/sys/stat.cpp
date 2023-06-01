#include <sys/stat.h>
#include <sys/syscall.h>

extern "C"
{
    _syscall2(stat, const char *, struct stat *);
    int stat(const char *path, struct stat *buf) { SYSCALL_RETURN(syscall_stat(path, buf)); }

    _syscall2(fstat, int, struct stat *);
    int fstat(int fildes, struct stat *buf) { SYSCALL_RETURN(syscall_fstat(fildes, buf)); }
}
