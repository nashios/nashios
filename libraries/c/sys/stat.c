#include <sys/stat.h>
#include <sys/syscall.h>

int fstat(int fildes, struct stat *buf) { return syscall(__NR_fstat, fildes, buf); }
