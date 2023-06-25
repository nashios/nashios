#include <kernel/filesystem/virtual.h>
#include <kernel/system/syscall.h>

int syscall_ioctl(int fd, unsigned long request, void *arg) { return virtual_fs_ioctl(fd, request, arg); }
