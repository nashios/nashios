/**
 * @file kernel/memory/heap.h
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
#pragma once

#include <kernel/memory/virtual.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct heap_block
{
    bool allocated;
    size_t size;
    struct heap_block *next;
};

void *heap_allocate(size_t size);
void heap_free(void *ptr);
void *heap_align(size_t size);
struct page_dir *heap_fork(struct page_dir *new_dir, struct page_dir *old_dir);
