/**
 * @file kernel/memory/physical.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Physical Memory Manager
 * @version 0.1
 * @date 2022-05-24
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
#include <kernel/memory/physical.h>
#include <kernel/memory/virtual.h>
#include <kernel/boot/multiboot.h>
#include <kernel/bitmap.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/math.h>

static uint32_t *phys_mm_bitmap = (uint32_t *)KERNEL_END;
static uint32_t phys_mm_max_frames;
static uint32_t phys_mm_used_frames;

void *phys_mm_allocate(size_t size)
{
    if (phys_mm_used_frames >= phys_mm_max_frames)
        return NULL;

    if (phys_mm_max_frames - phys_mm_used_frames < size)
        return NULL;

    uint32_t frame = bitmap_first_free(phys_mm_bitmap, phys_mm_max_frames, size);
    if ((int)frame == BITMAP_FAILED)
        return NULL;

    if (size == 1)
    {
        bitmap_set(phys_mm_bitmap, frame);
        phys_mm_used_frames++;
    }
    else
    {
        for (uint32_t i = 0; i < size; i++)
        {
            bitmap_set(phys_mm_bitmap, frame + i);
            phys_mm_used_frames++;
        }
    }

    return (void *)(frame * PHYS_MM_FRAMES_SIZE);
}

void phys_mm_set_region(uint32_t address, uint32_t length)
{
    uint32_t frame = address / PHYS_MM_FRAMES_SIZE;

    for (uint32_t i = 0; i < DIV_ROUND_UP(length, PHYS_MM_FRAMES_SIZE); i++)
    {
        bitmap_unset(phys_mm_bitmap, frame + i);
        phys_mm_used_frames--;
    }

    bitmap_set(phys_mm_bitmap, 0);

    printf("Physical MM: Set region address = 0x%08x, length = 0x%08x\n", address, length);
}

void phys_mm_unset_region(uint32_t address, uint32_t length)
{
    uint32_t frame = address / PHYS_MM_FRAMES_SIZE;
    for (uint32_t i = 0; i < DIV_ROUND_UP(length, PHYS_MM_FRAMES_SIZE); i++)
    {
        bitmap_set(phys_mm_bitmap, frame + i);
        phys_mm_used_frames++;
    }

    printf("Physical MM: Unset region address = 0x%08x, length = 0x%08x\n", address, length);
}

void phys_mm_init()
{
    uint32_t memory_size = (mbi->mem_lower + mbi->mem_upper) * 1024;
    uint32_t bitmap_frames = DIV_ROUND_UP(memory_size, PHYS_MM_FRAMES_SIZE);
    uint32_t bitmap_size = DIV_ROUND_UP(bitmap_frames, PHYS_MM_FRAMES_PER_BYTE);
    memset(phys_mm_bitmap, 0x0, bitmap_size);

    phys_mm_used_frames = bitmap_frames;
    phys_mm_max_frames = bitmap_frames;

    for (struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry *)PHYS_TO_VIRT(mbi->mmap_addr); (unsigned long)mmap < PHYS_TO_VIRT(mbi->mmap_addr) + mbi->mmap_length; mmap = (struct multiboot_mmap_entry *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
    {
        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE)
            continue;

        if ((mmap->addr + mmap->len) > 0xffffffff)
            continue;

        uint32_t diff_addr = mmap->addr % PHYS_MM_FRAMES_SIZE;
        if (diff_addr != 0)
        {
            printf("Physical MM: Unaligned region address = 0x%08lx by %d bytes\n", (unsigned long)mmap->addr, diff_addr);
            diff_addr = PHYS_MM_FRAMES_SIZE - diff_addr;
            mmap->addr += diff_addr;
            mmap->len -= diff_addr;
        }

        uint32_t diff_len = mmap->len % PHYS_MM_FRAMES_SIZE;
        if (diff_len != 0)
        {
            printf("Physical MM: Unaligned region length = 0x%08lx by %d bytes\n", (unsigned long)mmap->len, diff_len);
            mmap->len -= diff_len;
        }

        if (mmap->len < PHYS_MM_FRAMES_SIZE)
        {
            printf("Physical MM: Small region length = 0x%08lx, expected = %d bytes\n", (unsigned long)mmap->len, PHYS_MM_FRAMES_SIZE);
            continue;
        }

        phys_mm_set_region(mmap->addr, mmap->len);
    }

    phys_mm_unset_region(0x0, KERNEL_BOOT);
    phys_mm_unset_region(KERNEL_BOOT, KERNEL_END - KERNEL_START + bitmap_size);

    printf("Physical MM: Bitmap address = 0x%x, size = 0x%x, frames = %d, used = %d\n", phys_mm_bitmap, bitmap_size, phys_mm_max_frames, phys_mm_used_frames);
    printf("Physical MM: Kernel start = 0x%X, end = 0x%X\n", KERNEL_START, KERNEL_END);
    printf("Physical MM: Initialized\n");
}
