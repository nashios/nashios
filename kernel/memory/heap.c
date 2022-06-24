/**
 * @file kernel/memory/heap.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Heap memory manager
 * @version 0.1
 * @date 2022-05-25
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
#include <kernel/math.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/physical.h>
#include <kernel/string.h>

static struct heap_block *heap_blocks = NULL;
static uint32_t heap_address = 0xD0000000;
static uint32_t heap_remaining = 0;

struct page_dir *heap_fork(struct page_dir *new_dir, struct page_dir *old_dir)
{
    char *aligned_object = heap_align(PAGE_SIZE);
    for (int i_dir = 0; i_dir < 768; i_dir++)
    {
        if (!PAGE_IS_ENABLED(old_dir->entries[i_dir]))
            continue;

        struct page_tbl *new_tbl = (struct page_tbl *)heap_address;
        uint32_t new_tbl_physical = (uint32_t)phys_mm_allocate(1);
        virt_mm_map_addr(old_dir, new_tbl_physical, (uint32_t)new_tbl,
                         PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);
        memset(new_tbl, 0x0, sizeof(struct page_tbl));

        heap_address += sizeof(struct page_tbl);
        struct page_tbl *tbl = (struct page_tbl *)(PAGE_TBL_FRAME + i_dir * PAGE_SIZE);
        for (int i_tbl = 0; i_tbl < PAGES_PER_TBL; i_tbl++)
        {
            if (!PAGE_IS_ENABLED(tbl->entries[i_tbl]))
                continue;

            char *forked_tbl = (char *)heap_address;
            char *forked_tbl_frame = forked_tbl + PAGE_SIZE;
            heap_address = (uint32_t)(forked_tbl_frame + PAGE_SIZE);
            uint32_t forked_tbl_physical = (uint32_t)phys_mm_allocate(1);

            virt_mm_map_addr(old_dir, tbl->entries[i_tbl], (uint32_t)forked_tbl,
                             PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);
            virt_mm_map_addr(old_dir, forked_tbl_physical, (uint32_t)forked_tbl_frame,
                             PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);

            memcpy(forked_tbl_frame, forked_tbl, PAGE_SIZE);

            virt_mm_unmap_addr(old_dir, (uint32_t)forked_tbl);
            virt_mm_unmap_addr(old_dir, (uint32_t)forked_tbl_frame);

            new_tbl->entries[i_tbl] = forked_tbl_physical | PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER;
        }

        virt_mm_unmap_addr(old_dir, (uint32_t)new_tbl);
        new_dir->entries[i_dir] = new_tbl_physical | PAGE_DIR_PRESENT | PAGE_DIR_WRITABLE | PAGE_DIR_USER;
    }

    if (aligned_object)
        heap_free(aligned_object);

    return new_dir;
}

struct heap_block *heap_expand(struct heap_block *last_block, size_t size)
{
    size_t request_size = size + sizeof(struct heap_block);
    void *p_heap = (void *)heap_address;

    if (heap_remaining <= request_size)
    {
        uint32_t physical = (uint32_t)phys_mm_allocate(DIV_ROUND_UP(request_size - heap_remaining, PAGE_SIZE));
        uint32_t virtual = DIV_ROUND_UP(heap_address, PAGE_SIZE) * PAGE_SIZE;

        while (virtual < heap_address + request_size)
        {
            virt_mm_map_addr(virt_mm_dir, physical, virtual, PAGE_TBL_PRESENT);
            virtual += PAGE_SIZE;
            physical += PAGE_SIZE;
        }

        heap_remaining = virtual - (heap_address + request_size);
    }
    else
        heap_remaining -= request_size;

    heap_address += request_size;
    memset(p_heap, 0x0, request_size);

    struct heap_block *block = p_heap;
    if (last_block)
        last_block->next = block;

    block->size = size;
    block->next = NULL;
    block->allocated = true;
    return block;
}

void *heap_align(size_t size)
{
    if (heap_address % size == 0)
        return NULL;

    uint32_t request_size = sizeof(struct heap_block) * 2;
    uint32_t padding = DIV_ROUND_UP(heap_address, size) * size - heap_address;

    while (padding <= 0xF0000000)
    {
        if (padding > request_size)
        {
            struct heap_block *last = heap_blocks;
            while (!last->next)
            {
                last = last->next;
            }

            struct heap_block *block = heap_expand(last, padding - request_size);
            return block + 1;
        }
        padding += size;
    }
    return NULL;
}

void *heap_allocate(size_t size)
{
    if (size <= 0)
        return NULL;

    struct heap_block *block = NULL;
    if (heap_blocks != NULL)
    {
        struct heap_block *last = heap_blocks;
        for (block = heap_blocks; block; block = block->next)
        {
            if (!block->allocated && block->size >= size)
                break;

            last = block;
        }

        if (!block)
            block = heap_expand(last, size);
        else
        {
            block->allocated = true;
            if (block->size > size + sizeof(struct heap_block))
            {
                struct heap_block *new_block = (struct heap_block *)((char *)block + sizeof(struct heap_block) + size);
                new_block->allocated = false;
                new_block->size = block->size - size - sizeof(struct heap_block);
                new_block->next = block->next;

                block->size = size;
                block->next = new_block;
            }
        }
    }
    else
    {
        block = heap_expand(NULL, size);
        heap_blocks = block;
    }

    if (!block)
        return NULL;

    return block + 1;
}

void heap_free(void *ptr)
{
    if (!ptr)
        return;

    struct heap_block *block = (struct heap_block *)ptr - 1;
    block->allocated = false;
}
