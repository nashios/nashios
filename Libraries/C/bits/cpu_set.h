#pragma once

#include <bits/size_t.h>
#include <limits.h>
#include <stdlib.h>

#define CPU_SETSIZE 1024
#define __NCPUBITS (8 * sizeof(__cpu_mask))

#ifdef __cplusplus
extern "C"
{
#endif

    typedef unsigned long __cpu_mask;

    typedef struct
    {
        __cpu_mask __bits[CPU_SETSIZE / __NCPUBITS];
    } cpu_set_t;

    cpu_set_t *__libc_cpu_alloc(int num_cpus);
    size_t __libc_cpu_alloc_size(int num_cpus);

    void __libc_cpu_zero(const size_t setsize, cpu_set_t *set);
    void __libc_cpu_set(const int cpu, const size_t setsize, cpu_set_t *set);
    void __libc_cpu_clear(const int cpu, const size_t setsize, cpu_set_t *set);
    int __libc_cpu_isset(const int cpu, const size_t setsize, const cpu_set_t *set);
    int __libc_cpu_count(const size_t setsize, const cpu_set_t *set);

#define CPU_ALLOC_SIZE(n) __libc_cpu_alloc_size((n))
#define CPU_ALLOC(n) __libc_cpu_alloc((n))
#define CPU_FREE(set) free((set))

#define CPU_ZERO_S(setsize, set) __libc_cpu_zero((setsize), (set))
#define CPU_ZERO(set) CPU_ZERO_S(sizeof(cpu_set_t), set)

#define CPU_SET_S(cpu, setsize, set) __libc_cpu_set((cpu), (setsize), (set))
#define CPU_SET(cpu, set) CPU_SET_S(cpu, sizeof(cpu_set_t), set)

#define CPU_CLR_S(cpu, setsize, set) __libc_cpu_clear((cpu), (setsize), (set))
#define CPU_CLR(cpu, set) CPU_CLR_S(cpu, sizeof(cpu_set_t), set)

#define CPU_ISSET_S(cpu, setsize, set) __libc_cpu_isset((cpu), (setsize), (set))
#define CPU_ISSET(cpu, set) CPU_ISSET_S(cpu, sizeof(cpu_set_t), set)

#define CPU_COUNT_S(setsize, set) __libc_cpu_count((setsize), (set))
#define CPU_COUNT(set) CPU_COUNT_S(sizeof(cpu_set_t), set)

#ifdef __cplusplus
}
#endif
