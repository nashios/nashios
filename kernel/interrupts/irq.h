/**
 * @file kernel/interrupts/irq.h
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief x86 Interrupt Request
 * @version 0.1
 * @date 2022-05-22
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

#define IRQ_ENTRIES 16
#define IRQ_DEPTH 4

#include <kernel/interrupts/handler.h>

void irq_init();
void irq_add_handler(uint8_t number, itr_handler_t handler);
void irq_remove_handler(uint8_t number);
