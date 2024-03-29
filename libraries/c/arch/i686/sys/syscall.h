#ifndef _ARCH_i686_SYS_SYSCALL_H
#define _ARCH_i686_SYS_SYSCALL_H

static inline long __syscall0(long n)
{
    unsigned long result;
    asm volatile("int $128" : "=a"(result) : "a"(n) : "memory");
    return result;
}

static inline long __syscall1(long n, long a1)
{
    unsigned long result;
    asm volatile("int $128" : "=a"(result) : "a"(n), "b"(a1) : "memory");
    return result;
}

static inline long __syscall2(long n, long a1, long a2)
{
    unsigned long result;
    asm volatile("int $128" : "=a"(result) : "a"(n), "b"(a1), "c"(a2) : "memory");
    return result;
}

static inline long __syscall3(long n, long a1, long a2, long a3)
{
    unsigned long result;
    asm volatile("int $128" : "=a"(result) : "a"(n), "b"(a1), "c"(a2), "d"(a3) : "memory");
    return result;
}

static inline long __syscall4(long n, long a1, long a2, long a3, long a4)
{
    unsigned long result;
    asm volatile("int $128" : "=a"(result) : "a"(n), "b"(a1), "c"(a2), "d"(a3), "S"(a4) : "memory");
    return result;
}

static inline long __syscall5(long n, long a1, long a2, long a3, long a4, long a5)
{
    unsigned long result;
    asm volatile("int $128" : "=a"(result) : "a"(n), "b"(a1), "c"(a2), "d"(a3), "S"(a4), "D"(a5) : "memory");
    return result;
}

static inline long __syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
    unsigned long result;
    asm volatile("pushl %7 ; push %%ebp ; mov 4(%%esp),%%ebp ; "
                 "int $128"
                 " ; pop %%ebp ; add $4,%%esp"
                 : "=a"(result)
                 : "a"(n), "b"(a1), "c"(a2), "d"(a3), "S"(a4), "D"(a5), "g"(a6)
                 : "memory");
    return result;
}

#endif
