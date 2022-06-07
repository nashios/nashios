/**
 * @file kernel/system/sys.h
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief System controller
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

#define PANIC(message, registers) sys_panic(message, __FILE__, __PRETTY_FUNCTION__, __LINE__, registers);

#include <kernel/interrupts/handler.h>
#include <stdbool.h>

static inline void sys_cli()
{
    asm volatile("cli");
}

static inline void sys_sti()
{
    asm volatile("sti");
}

static inline void sys_hlt()
{
    asm volatile("hlt");
}

static inline void sys_stop()
{
    sys_cli();
    while (true)
        sys_hlt();
}

void sys_panic(const char *message, const char *file, const char *function, int line, struct itr_registers *registers);
