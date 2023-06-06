#pragma once

#include <bits/suseconds_t.h>
#include <bits/time_t.h>

struct timeval
{
    time_t tv_sec;
    suseconds_t tv_usec;
};
