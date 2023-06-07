#pragma once

#include <bits/clockid_t.h>
#include <bits/cpu_set.h>
#include <bits/signal.h>
#include <bits/size_t.h>

#define __LIBC_THREAD_CREATE_JOINABLE 0
#define __LIBC_THREAD_CREATE_DETACHED 1

#define __LIBC_THREAD_MUTEX_DEFAULT 0
#define __LIBC_THREAD_MUTEX_NORMAL 0
#define __LIBC_THREAD_MUTEX_ERRORCHECK 1
#define __LIBC_THREAD_MUTEX_RECURSIVE 2

#define __LIBC_THREAD_PROCESS_PRIVATE 0
#define __LIBC_THREAD_PROCESS_SHARED 1

#define __LIBC_THREAD_MUTEX_STALLED 0
#define __LIBC_THREAD_MUTEX_ROBUST 1

#define __LIBC_THREAD_PRIO_NONE 0
#define __LIBC_THREAD_PRIO_INHERIT 1
#define __LIBC_THREAD_PRIO_PROTECT 2

struct sched_param
{
    int sched_priority;
};

struct __libc_thread_data;

struct __libc_threadattr
{
    size_t __mlibc_guardsize;
    size_t __mlibc_stacksize;
    void *__mlibc_stackaddr;
    int __mlibc_detachstate;
    int __mlibc_scope;
    int __mlibc_inheritsched;
    struct sched_param __mlibc_schedparam;
    int __mlibc_schedpolicy;
    cpu_set_t *__mlibc_cpuset;
    size_t __mlibc_cpusetsize;
    sigset_t __mlibc_sigmask;
    int __mlibc_sigmaskset;
};

struct __libc_mutex
{
    unsigned int __mlibc_state;
    unsigned int __mlibc_recursion;
    unsigned int __mlibc_flags;
    int __mlibc_prioceiling;
};

struct __libc_mutexattr
{
    int __mlibc_type;
    int __mlibc_robust;
    int __mlibc_protocol;
    int __mlibc_pshared;
    int __mlibc_prioceiling;
};

struct __libc_cond
{
    unsigned int __mlibc_seq;
    unsigned int __mlibc_flags;
    clockid_t __mlibc_clock;
};

struct __libc_condattr
{
    int __mlibc_pshared;
    clockid_t __mlibc_clock;
};
