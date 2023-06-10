#pragma once

#include <errno.h>
#include <kernel/api/posix/sys/syscall.h>

#define _syscall0(name)                                                                                                \
    static inline int syscall_##name()                                                                                 \
    {                                                                                                                  \
        int result;                                                                                                    \
        asm volatile("int $128" : "=a"(result) : "0"(__NR_##name));                                                    \
        return result;                                                                                                 \
    }

#define _syscall1(name, type1)                                                                                         \
    static inline int syscall_##name(type1 arg1)                                                                       \
    {                                                                                                                  \
        int result;                                                                                                    \
        asm volatile("int $128" : "=a"(result) : "0"(__NR_##name), "b"(arg1));                                         \
        return result;                                                                                                 \
    }

#define _syscall2(name, type1, type2)                                                                                  \
    static inline int syscall_##name(type1 arg1, type2 arg2)                                                           \
    {                                                                                                                  \
        int result;                                                                                                    \
        asm volatile("int $128" : "=a"(result) : "0"(__NR_##name), "b"(arg1), "c"(arg2));                              \
        return result;                                                                                                 \
    }

#define _syscall3(name, type1, type2, type3)                                                                           \
    static inline int syscall_##name(type1 arg1, type2 arg2, type3 arg3)                                               \
    {                                                                                                                  \
        int result;                                                                                                    \
        asm volatile("int $128" : "=a"(result) : "0"(__NR_##name), "b"(arg1), "c"(arg2), "d"(arg3));                   \
        return result;                                                                                                 \
    }

#define _syscall4(name, type1, type2, type3, type4)                                                                    \
    static inline int syscall_##name(type1 arg1, type2 arg2, type3 arg3, type4 arg4)                                   \
    {                                                                                                                  \
        int result;                                                                                                    \
        asm volatile("int $128" : "=a"(result) : "0"(__NR_##name), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4));        \
        return result;                                                                                                 \
    }

#define _syscall5(name, type1, type2, type3, type4, type5)                                                             \
    static inline int syscall_##name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5)                       \
    {                                                                                                                  \
        int result;                                                                                                    \
        asm volatile("int $128"                                                                                        \
                     : "=a"(result)                                                                                    \
                     : "0"(__NR_##name), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4), "D"(arg5));                       \
        return result;                                                                                                 \
    }

#define SYSCALL_RETURN(expr)                                                                                           \
    ({                                                                                                                 \
        int result = expr;                                                                                             \
        if (result < 0)                                                                                                \
            errno = -result;                                                                                           \
        return result;                                                                                                 \
    })

#define SYSCALL_RETURN_PTR(expr)                                                                                       \
    ({                                                                                                                 \
        int result = expr;                                                                                             \
        if ((int)0xC0000000 < result && result < 0)                                                                    \
        {                                                                                                              \
            errno = -result;                                                                                           \
            return NULL;                                                                                               \
        }                                                                                                              \
        return (void *)result;                                                                                         \
    })
