#include <kernel/assert.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/math.h>
#include <kernel/memory/mmap.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <kernel/string.h>
#include <kernel/task/scheduler.h>

struct process_vm *mmap_find_unmapped_area(uint32_t addr, uint32_t len)
{
    struct process_mm *memory = g_scheduler_process->memory;
    struct process_vm *virtual_mm = (struct process_vm *)calloc(1, sizeof(struct process_vm));
    virtual_mm->memory = memory;

    if (!addr || addr < memory->brk_end)
        addr = MAX(memory->cache, memory->brk_end);

    ASSERT(addr == PAGE_ALIGN(addr));
    len = PAGE_ALIGN(len);

    uint32_t found_addr = addr;
    if (dlist_empty(&memory->list))
        dlist_add(&virtual_mm->list, &memory->list);
    else
    {
        struct process_vm *iter = NULL;
        dlist_for_each_entry(iter, &memory->list, list)
        {
            struct process_vm *next = dlist_next_entry(iter, list);
            bool last_entry = dlist_is_last(&iter->list, &memory->list);
            if (addr + len <= iter->start)
            {
                dlist_add(&iter->list, &memory->list);
                break;
            }
            else if (addr >= iter->end && (last_entry || (!last_entry && addr + len <= next->start)))
            {
                dlist_add(&virtual_mm->list, &iter->list);
                break;
            }
            else if (!last_entry && (iter->end <= addr && addr < next->start) && next->start - iter->end >= len)
            {
                dlist_add(&virtual_mm->list, &iter->list);
                found_addr = next->start - len;
                break;
            }
        }
    }

    if (found_addr)
    {
        virtual_mm->start = found_addr;
        virtual_mm->end = found_addr + len;

        memory->cache = virtual_mm->end;
    }

    return virtual_mm;
}

int mmap_expand_area(struct process_vm *virtual_mm, uint32_t address)
{
    address = PAGE_ALIGN(address);
    if (address <= virtual_mm->end)
        return 0;

    if (dlist_is_last(&virtual_mm->list, &g_scheduler_process->memory->list))
        virtual_mm->end = address;
    else
    {
        struct process_vm *next = dlist_next_entry(virtual_mm, list);
        if (address <= next->start)
            virtual_mm->end = address;
        else
        {
            dlist_remove(&virtual_mm->list);
            struct process_vm *virtual_mm_expand = mmap_find_unmapped_area(0, address - virtual_mm->start);
            memcpy(virtual_mm, virtual_mm_expand, sizeof(struct process_vm));
            free(virtual_mm_expand);
        }
    }
    return 0;
}

struct process_vm *mmap_find_vm(struct process_mm *memory, uint32_t addr)
{
    struct process_vm *virtual_mm = NULL;
    dlist_for_each_entry(virtual_mm, &memory->list, list)
    {
        if (virtual_mm->start <= addr && addr < virtual_mm->end)
            return virtual_mm;
    }

    return NULL;
}

void *mmap(void *addr, size_t length, int, int fd, int, off_t)
{
    uint32_t p_addr = (uint32_t)addr;
    uint32_t aligned_addr = ALIGN_DOWN(p_addr, PAGE_SIZE);

    struct process *process = g_scheduler_process;
    struct process_mm *memory = process->memory;
    struct process_vm *virtual_mm = mmap_find_vm(memory, aligned_addr);
    if (!virtual_mm)
        virtual_mm = mmap_find_unmapped_area(aligned_addr, length);
    else if (virtual_mm->end < p_addr + length)
        mmap_expand_area(virtual_mm, p_addr + length);

    struct vfs_file *file = fd >= 0 ? process->files->fd[fd] : NULL;
    if (file)
    {
        file->op->mmap(file, virtual_mm);
        virtual_mm->file = file;
    }
    else
    {
        for (uint32_t virtual = virtual_mm->start; virtual < virtual_mm->end; virtual += PAGE_SIZE)
        {
            uint32_t physical = (uint32_t)physical_mm_allocate();
            virtual_mm_map(process->directory, physical, virtual, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE);
        }
    }

    return addr ? addr : (void *)virtual_mm->start;
}

int munmap(void *addr, size_t length)
{
    uint32_t p_addr = (uint32_t)addr;
    struct process_mm *memory = g_scheduler_process->memory;
    struct process_vm *virtual_mm = mmap_find_vm(memory, p_addr);
    if (!virtual_mm || virtual_mm->start < p_addr)
        return 0;

    length = PAGE_ALIGN(length);
    if (virtual_mm->end - virtual_mm->start >= length)
        virtual_mm->end = p_addr;
    else
        dlist_remove(&virtual_mm->list);

    return 0;
}
