/**
 * @file kernel/api/posix/types.h
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief System types
 * @version 0.1
 * @date 2022-06-12
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

typedef int ssize_t;
typedef unsigned long sector_t;
typedef unsigned long long ino_t;
typedef unsigned short umode_t;
typedef long long loff_t;
typedef int pid_t;
typedef int id_t;
typedef unsigned int uid_t;
