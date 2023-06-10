#ifndef _ARCH_i686_SYS_SYSCALL_H
#define _ARCH_i686_SYS_SYSCALL_H


#define SYSCALL_INSNS "int $128"
#define SYSCALL_INSNS_12 "xchg %%ebx,%%edx ; " SYSCALL_INSNS " ; xchg %%ebx,%%edx"

long __syscall0(long n)
{
    unsigned long __ret;
    asm volatile(SYSCALL_INSNS : "=a"(__ret) : "a"(n) : "memory");
    return __ret;
}

long __syscall1(long n, long a1)
{
    unsigned long __ret;
    asm volatile(SYSCALL_INSNS_12 : "=a"(__ret) : "a"(n), "d"(a1) : "memory");
    return __ret;
}

long __syscall2(long n, long a1, long a2)
{
    unsigned long __ret;
    asm volatile(SYSCALL_INSNS_12 : "=a"(__ret) : "a"(n), "d"(a1), "c"(a2) : "memory");
    return __ret;
}

long __syscall3(long n, long a1, long a2, long a3)
{
    unsigned long __ret;
#if !defined(__PIC__) || !defined(BROKEN_EBX_ASM)
    asm volatile(SYSCALL_INSNS : "=a"(__ret) : "a"(n), "b"(a1), "c"(a2), "d"(a3) : "memory");
#else
    asm volatile(SYSCALL_INSNS_34 : "=a"(__ret) : "a"(n), "D"(a1), "c"(a2), "d"(a3) : "memory");
#endif
    return __ret;
}

long __syscall4(long n, long a1, long a2, long a3, long a4)
{
    unsigned long __ret;
#if !defined(__PIC__) || !defined(BROKEN_EBX_ASM)
    asm volatile(SYSCALL_INSNS : "=a"(__ret) : "a"(n), "b"(a1), "c"(a2), "d"(a3), "S"(a4) : "memory");
#else
    asm volatile(SYSCALL_INSNS_34 : "=a"(__ret) : "a"(n), "D"(a1), "c"(a2), "d"(a3), "S"(a4) : "memory");
#endif
    return __ret;
}

long __syscall5(long n, long a1, long a2, long a3, long a4, long a5)
{
    unsigned long __ret;
#if !defined(__PIC__) || !defined(BROKEN_EBX_ASM)
    asm volatile(SYSCALL_INSNS : "=a"(__ret) : "a"(n), "b"(a1), "c"(a2), "d"(a3), "S"(a4), "D"(a5) : "memory");
#else
    asm volatile("pushl %2 ; push %%ebx ; mov 4(%%esp),%%ebx ; " SYSCALL_INSNS " ; pop %%ebx ; add $4,%%esp"
                 : "=a"(__ret)
                 : "a"(n), "g"(a1), "c"(a2), "d"(a3), "S"(a4), "D"(a5)
                 : "memory");
#endif
    return __ret;
}

long __syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
    unsigned long __ret;
#if !defined(__PIC__) || !defined(BROKEN_EBX_ASM)
    asm volatile("pushl %7 ; push %%ebp ; mov 4(%%esp),%%ebp ; " SYSCALL_INSNS " ; pop %%ebp ; add $4,%%esp"
                 : "=a"(__ret)
                 : "a"(n), "b"(a1), "c"(a2), "d"(a3), "S"(a4), "D"(a5), "g"(a6)
                 : "memory");
#else
    unsigned long a1a6[2] = {a1, a6};
    asm volatile("pushl %1 ; push %%ebx ; push %%ebp ; mov 8(%%esp),%%ebx ; mov 4(%%ebx),%%ebp ; mov (%%ebx),%%ebx "
                 "; " SYSCALL_INSNS " ; pop %%ebp ; pop %%ebx ; add $4,%%esp"
                 : "=a"(__ret)
                 : "g"(&a1a6), "a"(n), "c"(a2), "d"(a3), "S"(a4), "D"(a5)
                 : "memory");
#endif
    return __ret;
}


#endif
