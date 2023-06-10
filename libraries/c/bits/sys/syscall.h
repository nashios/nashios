#ifndef _BITS_SYS_SYSCALL_H
#define _BITS_SYS_SYSCALL_H

#include <errno.h>
#include <kernel/api/posix/sys/syscall.h>

#if defined(__i386__)
#include <arch/i686/sys/syscall.h>
#else
#error "Unsupported architecture"
#endif

#define __scc(X) ((long)(X))
#define __syscall1(n, a) __syscall1(n, __scc(a))
#define __syscall2(n, a, b) __syscall2(n, __scc(a), __scc(b))
#define __syscall3(n, a, b, c) __syscall3(n, __scc(a), __scc(b), __scc(c))
#define __syscall4(n, a, b, c, d) __syscall4(n, __scc(a), __scc(b), __scc(c), __scc(d))
#define __syscall5(n, a, b, c, d, e) __syscall5(n, __scc(a), __scc(b), __scc(c), __scc(d), __scc(e))
#define __syscall6(n, a, b, c, d, e, f) __syscall6(n, __scc(a), __scc(b), __scc(c), __scc(d), __scc(e), __scc(f))
#define __syscall7(n, a, b, c, d, e, f, g)                                                                             \
    __syscall7(n, __scc(a), __scc(b), __scc(c), __scc(d), __scc(e), __scc(f), __scc(g))

#define __SYSCALL_NARGS_X(a, b, c, d, e, f, g, h, n, ...) n
#define __SYSCALL_NARGS(...) __SYSCALL_NARGS_X(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1, 0, )
#define __SYSCALL_CONCAT_X(a, b) a##b
#define __SYSCALL_CONCAT(a, b) __SYSCALL_CONCAT_X(a, b)
#define __SYSCALL_DISP(b, ...) __SYSCALL_CONCAT(b, __SYSCALL_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define __syscall(...) __SYSCALL_DISP(__syscall, __VA_ARGS__)
#define syscall(...) __syscall_result(__syscall(__VA_ARGS__))

static inline long __syscall_result(unsigned long result)
{
    if (result > -4096UL)
    {
        errno = -result;
        return -1;
    }
    return result;
}

#endif
