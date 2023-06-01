#pragma once

#include <kernel/api/posix/sys/types.h>

#define S_IFMT 00170000
#define S_IFREG 0100000
#define S_IFBLK 0060000
#define S_IFDIR 0040000
#define S_IFCHR 0020000
#define S_IFIFO 0010000

#define S_ISREG(m) (((m)&S_IFMT) == S_IFREG)
#define S_ISBLK(m) (((m)&S_IFMT) == S_IFBLK)
#define S_ISDIR(m) (((m)&S_IFMT) == S_IFDIR)
#define S_ISCHR(m) (((m)&S_IFMT) == S_IFCHR)
#define S_ISFIFO(m) (((m)&S_IFMT) == S_IFIFO)

#define S_IWUSR 00200

struct stat
{
    unsigned long st_dev;
    unsigned long st_ino;
    unsigned short st_mode;
    unsigned short st_nlink;
    unsigned long st_size;
    unsigned long st_blksize;
    unsigned long st_blocks;
    unsigned long st_mtime;
};
