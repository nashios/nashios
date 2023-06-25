#include <kernel/memory/mmap.h>
#include <kernel/system/syscall.h>

int syscall_brk(void *addr)
{
    uint32_t p_addr = (uint32_t)addr;
    if (p_addr == 0)
        return g_scheduler_process->memory->brk_middle;
    return mmap_brk(p_addr);
}
