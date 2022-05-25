/**
 * @file kernel/memory/virtual.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Virtual memory manager
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
#include <kernel/memory/virtual.h>
#include <kernel/memory/physical.h>
#include <kernel/stdio.h>
#include <kernel/string.h>

static struct page_dir *virt_mm_dir;

extern void page_enable(uint32_t address);

void virt_mm_identity_map(struct page_dir *dir, uint32_t physical, uint32_t virtual)
{
    struct page_tbl *tbl = (struct page_tbl *)phys_mm_allocate(1);
    memset(tbl, 0x0, sizeof(struct page_tbl));

    for (uint32_t i = 0, i_physical = physical, i_virtual = virtual; i < PAGES_PER_TBL; i++, i_physical += PAGE_SIZE, i_virtual += PAGE_SIZE)
    {
        uint32_t *entry = &tbl->entries[PAGE_TBL_INDEX(i_virtual)];
        *entry |= PAGE_TBL_PRESENT;
        *entry = (*entry & ~PAGE_TBL_FRAME) | i_physical;
    }

    uint32_t *entry = &dir->entries[PAGE_DIR_INDEX(physical)];
    *entry |= PAGE_DIR_PRESENT;
    *entry = (*entry & ~PAGE_DIR_FRAME) | (uint32_t)tbl;

    printf("Virtual MM: Identity map physical address = 0x%08x to virtual = 0x%08x\n", physical, virtual);
}

void virt_mm_create_tbl(struct page_dir *dir, uint32_t virtual, uint32_t flags)
{
    uint32_t *entry = &dir->entries[PAGE_DIR_INDEX(virtual)];
    if (entry)
        return;

    uint32_t physical = (uint32_t)phys_mm_allocate(1);
    *entry = physical | flags;
    virt_mm_map_addr(dir, physical, virtual, flags);
}

void virt_mm_map_addr(struct page_dir *dir, uint32_t physical, uint32_t virtual, uint32_t flags)
{
    uint32_t *entry = &dir->entries[PAGE_DIR_INDEX(virtual)];
    if (!entry)
        virt_mm_create_tbl(dir, virtual, flags);

    uint32_t *tlb_entry = &((uint32_t *)(*entry & ~0xFFF))[PAGE_TBL_INDEX(virtual)];
    *tlb_entry = physical | flags;
    printf("Virtual MM: Mapped physical address = 0x%x to virtual = 0x%x, flags = 0x%x\n", physical, virtual, flags);
}

void virt_mm_init()
{
    struct page_dir *dir = (struct page_dir *)phys_mm_allocate(1);
    memset(dir, 0x0, sizeof(struct page_dir));

    virt_mm_identity_map(dir, 0x0, 0x0);
    virt_mm_identity_map(dir, KERNEL_BOOT, KERNEL_BOOT);

    dir->entries[PAGES_PER_DIR - 1] = ((uint32_t)dir & PAGE_DIR_FRAME) | PAGE_DIR_PRESENT;
    virt_mm_dir = dir;

    page_enable((uint32_t)&dir->entries);
    printf("Virtual MM: Enabled paging\n");

    printf("Virtual MM: Initialized\n");
}
