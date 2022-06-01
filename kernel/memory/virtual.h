/**
 * @file kernel/memory/virtual.h
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
#pragma once

#define PAGE_DIR_FRAME 0xFFFFF000
#define PAGE_TBL_FRAME 0xFFC00000

#define PAGE_DIR_INDEX(addr) (((addr) >> 22) & 0x3FF)
#define PAGE_TBL_INDEX(addr) (((addr) >> 12) & 0x3FF)

#define PAGES_PER_TBL 1024
#define PAGES_PER_DIR 1024

#define PAGE_SIZE 4096
#define PAGE_MASK (~(PAGE_SIZE - 1))
#define PAGE_ALIGN(addr) (((addr) + PAGE_SIZE - 1) & PAGE_MASK)
#define PAGE_IS_ENABLED(addr) (addr & 0x1)

#include <stdint.h>

enum page_dir_flags
{
    PAGE_DIR_PRESENT = 1,
    PAGE_DIR_WRITABLE = 2
};

enum page_tbl_flags
{
    PAGE_TBL_PRESENT = 1,
    PAGE_TBL_WRITABLE = 2
};

struct page_tbl
{
    uint32_t entries[PAGES_PER_TBL];
};

struct page_dir
{
    uint32_t entries[PAGES_PER_DIR];
};

extern struct page_dir *virt_mm_dir;

void virt_mm_init();
void virt_mm_map_addr(struct page_dir *dir, uint32_t physical, uint32_t virtual, uint32_t flags);
uint32_t virt_mm_get_phys_addr(uint32_t virtual);
