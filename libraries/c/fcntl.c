#include <fcntl.h>
#include <sys/syscall.h>

int open(const char *path, int oflag, ...) { return syscall(__NR_open, path, oflag); }
