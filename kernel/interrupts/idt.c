/**
 * @file kernel/interrupts/idt.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief x86 Interrupt Descriptor Table
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
#include <kernel/interrupts/idt.h>
#include <kernel/stdio.h>

static struct idt_ptr idt_pointer;
static struct idt_entry idt_entries[IDT_ENTRIES];

extern void idt_flush(uint32_t address);

void idt_add_entry(uint8_t index, uint32_t base, uint16_t selector, uint8_t flags)
{
    idt_entries[index].base_low = (base & 0xFFFF);
    idt_entries[index].base_high = (base >> 16) & 0xFFFF;
    idt_entries[index].selector = selector;
    idt_entries[index].always_zero = 0;
    idt_entries[index].flags = flags;
}

void idt_init()
{
    idt_pointer.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;
    idt_pointer.base = (uint32_t)&idt_entries;

    idt_flush((uint32_t)&idt_pointer);
    printf("IDT: Flushed address 0x%08x\n", &idt_pointer);

    printf("IDT: Initialized\n");
}
