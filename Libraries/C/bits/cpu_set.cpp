#include <assert.h>
#include <bits/cpu_set.h>

extern "C"
{
    cpu_set_t *__libc_cpu_alloc(int num_cpus) { assert(false); }

    size_t __libc_cpu_alloc_size(int num_cpus) { assert(false); }

    void __libc_cpu_zero(const size_t setsize, cpu_set_t *set) { assert(false); }

    void __libc_cpu_set(const int cpu, const size_t setsize, cpu_set_t *set) { assert(false); }

    void __libc_cpu_clear(const int cpu, const size_t setsize, cpu_set_t *set) { assert(false); }

    int __libc_cpu_isset(const int cpu, const size_t setsize, const cpu_set_t *set) { assert(false); }

    int __libc_cpu_count(const size_t setsize, const cpu_set_t *set) { assert(false); }
}
