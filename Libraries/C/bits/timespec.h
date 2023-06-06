#pragma once

#include <bits/time_t.h>

struct timespec
{
    time_t tv_sec;
    long tv_nsec;
};
