#include <kernel/system/syscall.h>

void syscall_exit(int status) { scheduler_exit(status); }
