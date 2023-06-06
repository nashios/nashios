#pragma once

#include <bits/types.h>

typedef struct
{
    union {
        __libc_uint8 __elems[128];
        __libc_uint8 fds_bits[128];
    };
} fd_set;
