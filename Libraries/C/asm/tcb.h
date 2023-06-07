#pragma once

#include <bits/size_t.h>

struct tcb
{
    struct tcb *self_ptr;
    size_t dtv_size;
    void **dtv_ptrs;
    int tid;
};
