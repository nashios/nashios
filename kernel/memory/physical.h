/**
 * @file kernel/memory/physical.h
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
#pragma once

#define PHYS_MM_FRAMES_PER_BYTE 8
#define PHYS_MM_FRAMES_SIZE 4096

#include <stddef.h>

void phys_mm_init();
void *phys_mm_allocate(size_t size);
