/**
 * @file kernel/string.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Libc string implementation for kernel
 * @version 0.1
 * @date 2022-05-19
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
#include <kernel/string.h>
#include <stdint.h>

void *memset(void *s, int c, size_t n)
{
    uint8_t *p_s = s;

    for (size_t i = 0; i < n; i++)
        p_s[i] = c;

    return s;
}
