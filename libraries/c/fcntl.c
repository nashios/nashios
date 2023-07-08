/**
 * @file libraries/c/fcntl.c
 * @author Saullo Bretas Silva (saullo.silva55@outlook.com)
 * @brief
 * @version 0.0.1
 * @date 2023-07-08
 *
 * @copyright Nashi Operating System
 * Copyright (C) 2023 Saullo Bretas Silva <saullo.silva55@outlook.com>
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
#include <fcntl.h>
#include <stdarg.h>
#include <sys/syscall.h>
#include <sys/types.h>

int open(const char *path, int oflag, ...)
{
    mode_t mode = 0;
    if (oflag & O_CREAT)
    {
        va_list args;
        va_start(args, oflag);
        mode = va_arg(args, mode_t);
        va_end(args);
    }

    return syscall(__NR_open, path, oflag, mode);
}

int fcntl(int fildes, int cmd, ...)
{
    va_list ap;
    va_start(ap, cmd);
    void *args = va_arg(ap, void *);
    va_end(ap);

    return syscall(__NR_fcntl, fildes, cmd, args);
}
