/**
 * @file kernel/stdlib.h
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Libc stdlib implementation for kernel
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

#include <stddef.h>

void *malloc(size_t size);
void *calloc(size_t nelem, size_t elsize);
void free(void *ptr);
