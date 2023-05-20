#pragma once

#include <kernel/api/posix/sys/types.h>

#define S_IFMT 00170000
#define S_IFDIR 0040000
#define S_IFREG 0100000

#define S_ISREG(m) (((m)&S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m)&S_IFMT) == S_IFDIR)

struct stat
{
    off_t st_size;
};