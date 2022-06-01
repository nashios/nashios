/**
 * @file kernel/bitmap.c
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
#include <kernel/bitmap.h>

void bitmap_set(uint32_t *bitmap, uint32_t bit)
{
    bitmap[BITMAP_INDEX(bit)] |= BITMAP_OFFSET(bit);
}

void bitmap_unset(uint32_t *bitmap, uint32_t bit)
{
    bitmap[BITMAP_INDEX(bit)] &= ~BITMAP_OFFSET(bit);
}

bool bitmap_test(uint32_t *bitmap, uint32_t bit)
{
    return bitmap[BITMAP_INDEX(bit)] & BITMAP_OFFSET(bit);
}

uint32_t bitmap_first_free(uint32_t *bitmap, uint32_t frames, size_t size)
{
    if (size == 0)
        return BITMAP_FAILED;

    for (uint32_t i = 0; i < BITMAP_INDEX(frames); i++)
    {
        if (bitmap[i] == 0xFFFFFFFF)
            continue;

        for (int j = 0; j < 32; j++)
        {
            if (!(bitmap[i] & (1 << j)))
            {
                uint32_t bit = i * 32 + j;
                if (size == 1)
                    return bit;

                uint32_t free_bits = 0;
                for (uint32_t count = 0; count <= size; count++)
                {
                    if (!bitmap_test(bitmap, bit + count))
                        free_bits++;

                    if (free_bits == size)
                        return bit;
                }
            }
        }
    }

    return BITMAP_FAILED;
}
