#pragma once

#define _syscall0(name)                                                                                                \
    static inline int syscall_##name()                                                                                 \
    {                                                                                                                  \
        int ret;                                                                                                       \
        asm volatile("int $127" : "=a"(ret) : "0"(__NR_##name));                                                       \
        return ret;                                                                                                    \
    }
#define _syscall1(name, type1)                                                                                         \
    static inline int syscall_##name(type1 arg1)                                                                       \
    {                                                                                                                  \
        int ret;                                                                                                       \
        asm volatile("int $127" : "=a"(ret) : "0"(__NR_##name), "b"(arg1));                                            \
        return ret;                                                                                                    \
    }

#define _syscall2(name, type1, type2)                                                                                  \
    static inline int syscall_##name(type1 arg1, type2 arg2)                                                           \
    {                                                                                                                  \
        int ret;                                                                                                       \
        asm volatile("int $127" : "=a"(ret) : "0"(__NR_##name), "b"(arg1), "c"(arg2));                                 \
        return ret;                                                                                                    \
    }

#define _syscall3(name, type1, type2, type3)                                                                           \
    static inline int syscall_##name(type1 arg1, type2 arg2, type3 arg3)                                               \
    {                                                                                                                  \
        int ret;                                                                                                       \
        asm volatile("int $127" : "=a"(ret) : "0"(__NR_##name), "b"(arg1), "c"(arg2), "d"(arg3));                      \
        return ret;                                                                                                    \
    }

#define _syscall4(name, type1, type2, type3, type4)                                                                    \
    static inline int syscall_##name(type1 arg1, type2 arg2, type3 arg3, type4 arg4)                                   \
    {                                                                                                                  \
        int ret;                                                                                                       \
        asm volatile("int $127" : "=a"(ret) : "0"(__NR_##name), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4));           \
        return ret;                                                                                                    \
    }

#define _syscall5(name, type1, type2, type3, type4, type5)                                                             \
    static inline int syscall_##name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5)                       \
    {                                                                                                                  \
        int ret;                                                                                                       \
        asm volatile("int $127"                                                                                        \
                     : "=a"(ret)                                                                                       \
                     : "0"(__NR_##name), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4), "D"(arg5));                       \
        return ret;                                                                                                    \
    }

#define SYSCALL_RETURN(expr)                                                                                           \
    ({                                                                                                                 \
        int result = expr;                                                                                             \
        if (result < 0)                                                                                                \
        {                                                                                                              \
            return errno = -result, -1;                                                                                \
        }                                                                                                              \
        return result;                                                                                                 \
    })

#include <errno.h>
#include <kernel/api/posix/unistd.h>
