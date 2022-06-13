/**
 * @file kernel/mm/mmap.h
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
#pragma once

#include <kernel/dlist.h>
#include <stdint.h>

struct mmap_mm
{
    uint32_t brk_start;
    uint32_t brk_middle;
    uint32_t brk_end;
    uint32_t cache;
    struct dlist_head list;
};

struct mmap_area
{
    uint32_t start;
    uint32_t end;
    struct dlist_head list;
};

uint32_t mmap_map(uint32_t address, uint32_t length);
