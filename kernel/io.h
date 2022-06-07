/**
 * @file kernel/io.h
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief x86 I/O port
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
#pragma once

#include <stdint.h>
#include <stddef.h>

static inline void outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(value), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t value)
{
    asm volatile("outw %0, %1"
                 :
                 : "a"(value), "Nd"(port));
}

static inline void outl(uint16_t port, uint32_t value)
{
    asm volatile("outl %0, %1"
                 :
                 : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t result;
    asm volatile("inb %1, %0"
                 : "=a"(result)
                 : "Nd"(port));
    return result;
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t result;
    asm volatile("inw %1, %0"
                 : "=a"(result)
                 : "Nd"(port));
    return result;
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t result;
    asm volatile("inl %1, %0"
                 : "=a"(result)
                 : "Nd"(port));
    return result;
}

static inline void insw(uint16_t port, void *buffer, size_t length)
{
    asm volatile("rep insw"
                 : "+D"(buffer), "+c"(length)
                 : "d"(port)
                 : "memory");
}

static inline void io_wait(void)
{
    outb(0x80, 0);
}
