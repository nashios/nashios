/**
 * @file kernel/mm/mmap.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Kernel virtual memory mapping
 * @version 0.1
 * @date 2022-06-13
 *
 * @copyright Copyright (C) 2022 Saullo Bretas Silva
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include <kernel/api/posix/errno.h>
#include <kernel/math.h>
#include <kernel/memory/mmap.h>
#include <kernel/memory/physical.h>
#include <kernel/stdlib.h>
#include <kernel/string.h>
#include <kernel/task/scheduler.h>

struct mmap_area *mmap_find_area(uint32_t address, struct mmap_mm *mm)
{
    struct mmap_area *area;
    dlist_foreach_entry(area, &mm->list, list)
    {
        if (area->start <= address && address < area->end)
            return area;
    }
    return NULL;
}

struct mmap_area *mmap_create_area(uint32_t address, uint32_t length, struct process *process)
{
    struct mmap_area *area = calloc(1, sizeof(struct mmap_area));
    if (!area)
        return NULL;

    struct mmap_mm *mm = process->mm;
    if (!address || address < mm->brk_end)
        address = MAX(mm->cache, mm->brk_end);

    length = PAGE_ALIGN(length);
    uint32_t address_found = address;
    if (!dlist_is_empty(&mm->list))
    {
        struct mmap_area *found_area;
        dlist_foreach_entry(found_area, &mm->list, list)
        {
            struct mmap_area *next = dlist_next_entry(found_area, list);
            bool last_entry = dlist_is_last(&found_area->list, &mm->list);
            if (!last_entry && (found_area->end <= address && address < next->start) &&
                next->start - found_area->end >= length)
            {
                dlist_add(&area->list, &found_area->list);
                address_found = next->start - length;
                break;
            }
            else if (address >= found_area->end && (last_entry || (!last_entry && address + length <= next->start)))
            {
                dlist_add(&area->list, &found_area->list);
                break;
            }
            else if (address + length <= found_area->start)
            {
                dlist_add(&found_area->list, &mm->list);
                break;
            }
        }
    }
    else
        dlist_add(&area->list, &mm->list);

    if (address_found)
    {
        area->start = address_found;
        area->end = address_found + length;
        mm->cache = area->end;
    }

    return area;
}

struct mmap_area *mmap_expand_area(uint32_t address, struct mmap_area *area, struct process *process)
{
    address = PAGE_ALIGN(address);
    if (address <= area->end)
        return 0;

    if (!dlist_is_last(&area->list, &process->mm->list))
    {
        struct mmap_area *next_area = dlist_next_entry(area, list);
        if (address <= next_area->start)
            area->end = address;
        else
        {
            dlist_remove(&area->list);

            struct mmap_area *expanded_area = mmap_create_area(0x0, address - area->start, process);
            memcpy(area, expanded_area, sizeof(struct mmap_area));
            free(expanded_area);
        }
    }
    else
        area->end = address;
    return 0;
}

uint32_t mmap_map(uint32_t address, uint32_t length)
{
    struct process *process = sched_current_process();
    uint32_t address_aligned = ALIGN_DOWN(address, PAGE_SIZE);
    struct mmap_area *area = mmap_find_area(address_aligned, process->mm);
    if (!area)
        area = mmap_create_area(address_aligned, length, process);
    else if (area->end < address + length)
        area = mmap_expand_area(address + length, area, process);

    uint32_t virtual = area->start;
    while (virtual < area->end)
    {
        uint32_t physical = (uint32_t)phys_mm_allocate(1);
        virt_mm_map_addr(process->page_dir, physical, virtual, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);

        virtual += PAGE_SIZE;
    }

    return address ? address : area->start;
}

uint32_t mmap_brk(uint32_t address, uint32_t length)
{
    struct process *process = sched_current_process();
    struct mmap_area *area = mmap_find_area(address, process->mm);
    if (!area)
        return 0;

    uint32_t brk = PAGE_ALIGN(address + length);
    process->mm->brk_middle = brk;
    if (area->end >= brk)
        return 0;

    struct mmap_area *new_area = calloc(1, sizeof(struct mmap_area));
    memcpy(new_area, area, sizeof(struct mmap_area));

    if (brk > process->mm->brk_middle)
        mmap_expand_area(brk, new_area, process);
    else
        new_area->end = brk;

    if (new_area->end > area->end)
    {
        uint32_t physical = (uint32_t)phys_mm_allocate((new_area->end - area->end) / PAGE_SIZE);
        uint32_t virtual = area->end;
        while (virtual < new_area->end)
        {
            virt_mm_map_addr(process->page_dir, physical, virtual,
                             PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);

            virtual += PAGE_SIZE;
            physical += PAGE_SIZE;
        }
    }
    memcpy(area, new_area, sizeof(struct mmap_area));

    return 0;
}
