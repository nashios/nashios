#pragma once

#include <kernel/api/posix/fcntl.h>

int open(const char *path, int oflag, ...);
