#pragma once

#include <kernel/api/posix/types.h>

union sigval {
    int sival_int;
    void *sival_ptr;
};

typedef struct siginfo
{
    int si_signo;
    int si_code;
    int si_errno;
    pid_t si_pid;
    uid_t si_uid;
    void *si_addr;
    int si_status;
    int si_band;
    union sigval si_value;
} siginfo_t;
