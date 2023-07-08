/**
 * @file libraries/c/unistd.c
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
#include <stdbool.h>
#include <sys/syscall.h>
#include <termios.h>
#include <unistd.h>

int brk(void *addr) { return syscall(__NR_brk, addr); }

void *sbrk(intptr_t increment)
{
    void *value = (void *)brk(0);
    brk(value + increment);
    return value;
}

pid_t fork(void) { return syscall(__NR_fork); }

int execve(const char *path, char *const argv[], char *const envp[]) { return syscall(__NR_execve, path, argv, envp); }

int close(int fildes) { return syscall(__NR_close, fildes); }

int ftruncate(int fildes, off_t length) { return syscall(__NR_ftruncate, fildes, length); }

pid_t getpid(void) { return syscall(__NR_getpid); }

ssize_t read(int fildes, void *buf, size_t nbyte) { return syscall(__NR_read, fildes, buf, nbyte); }

ssize_t write(int fildes, const void *buf, size_t nbyte) { return syscall(__NR_write, fildes, buf, nbyte); }

int dup(int fildes) { return syscall(__NR_dup, fildes); }

int dup2(int fildes, int fildes2) { return syscall(__NR_dup2, fildes, fildes2); }

void _exit(int status)
{
    while (true)
        syscall(__NR_exit, status);
}

int isatty(int fildes)
{
    struct termios termios;
    return tcgetattr(fildes, &termios) == 0;
}