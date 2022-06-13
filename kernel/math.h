/**
 * @file kernel/math.h
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Math helper library
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

#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

#define ALIGN_DOWN(address, length) ((address) & -((typeof(address))(length)))
#define ALIGN_UP(address, length) ALIGN_DOWN((address) + (length)-1, (length))

#define MIN(x, y) ({ typeof(x) p_x = (x); typeof(y) p_y = (y); (void) (&p_x == &p_y);	p_x < p_y ? p_x : p_y; })

#define MAX(x, y) ({ typeof(x) p_x = (x); typeof(y) p_y = (y); (void) (&p_x == &p_y); p_x > p_y ? p_x : p_y; })
