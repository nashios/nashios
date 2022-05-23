/**
 * @file kernel/interrupts/pic.h
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief 8259 Programmable Interrupt Controller
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

#include <stdint.h>

#define PIC1 0x20
#define PIC1_DATA (PIC1 + 1)
#define PIC1_COMMAND PIC1

#define PIC2 0xA0
#define PIC2_DATA (PIC2 + 1)
#define PIC2_COMMAND PIC2

#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

void pic_eoi(uint8_t irq);
void pic_remap(uint8_t offset1, uint8_t offset2);
void pic_set_mask(uint8_t irq);
void pic_unset_mask(uint8_t irq);
