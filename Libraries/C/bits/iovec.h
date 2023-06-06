#pragma once

#include <bits/types.h>

struct iovec
{
    void *iov_base;
    __libc_size iov_len;
};
