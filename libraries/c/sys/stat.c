#include <sys/stat.h>
#include <sys/syscall.h>

_syscall2(stat, const char *, struct stat *);
int stat(const char *path, struct stat *buf) { SYSCALL_RETURN(syscall_stat(path, buf)); }
