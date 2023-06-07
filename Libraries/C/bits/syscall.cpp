#include <bits/syscall.h>
#include <errno.h>

extern "C"
{
    __sc_word_t __do_syscall0(long sc)
    {
        __sc_word_t result;
        asm volatile("int $0x80" : "=a"(result) : "a"(sc) : "memory");
        return result;
    }

    __sc_word_t __do_syscall1(long sc, __sc_word_t arg1)
    {
        __sc_word_t result;
        asm volatile("int $0x80" : "=a"(result) : "a"(sc), "d"(arg1) : "memory");
        return result;
    }

    __sc_word_t __do_syscall2(long sc, __sc_word_t arg1, __sc_word_t arg2)
    {
        __sc_word_t result;
        asm volatile("int $0x80" : "=a"(result) : "a"(sc), "d"(arg1), "c"(arg2) : "memory");
        return result;
    }

    __sc_word_t __do_syscall3(long sc, __sc_word_t arg1, __sc_word_t arg2, __sc_word_t arg3)
    {
        __sc_word_t result;
        asm volatile("int $0x80" : "=a"(result) : "a"(sc), "d"(arg1), "c"(arg2), "D"(arg3) : "memory");
        return result;
    }

    __sc_word_t __do_syscall4(long sc, __sc_word_t arg1, __sc_word_t arg2, __sc_word_t arg3, __sc_word_t arg4)
    {
        __sc_word_t result;
        asm volatile("int $0x80" : "=a"(result) : "a"(sc), "d"(arg1), "c"(arg2), "D"(arg3), "S"(arg4) : "memory");
        return result;
    }

    __sc_word_t __do_syscall5(long sc, __sc_word_t arg1, __sc_word_t arg2, __sc_word_t arg3, __sc_word_t arg4,
                              __sc_word_t arg5)
    {
        __sc_word_t result;
        asm volatile("int $0x80"
                     : "=a"(result)
                     : "a"(sc), "d"(arg1), "c"(arg2), "D"(arg3), "S"(arg4), "g"(arg5)
                     : "memory");
        return result;
    }

    long __do_syscall_ret(unsigned long result)
    {
        if (result > -4096UL)
        {
            errno = -result;
            return -1;
        }
        return result;
    }
}
