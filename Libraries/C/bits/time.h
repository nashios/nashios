#pragma once

#include <bits/timeval.h>

struct itimerval
{
    struct timeval it_interval;
    struct timeval it_value;
};

#define ITIMER_REAL 0
#define ITIMER_VIRTUAL 1
#define ITIMER_PROF 2
