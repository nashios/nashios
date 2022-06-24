/**
 * @file kernel/bitmap.h
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Memory Bitmap
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

#define BITMAP_FAILED -1
#define BITMAP_INDEX(bit) (bit / 32)
#define BITMAP_OFFSET(bit) (1 << (bit % 32))

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void bitmap_set(uint32_t *bitmap, uint32_t bit);
void bitmap_unset(uint32_t *bitmap, uint32_t bit);
bool bitmap_test(uint32_t *bitmap, uint32_t bit);
uint32_t bitmap_first_free(uint32_t *bitmap, uint32_t frames, size_t size);
