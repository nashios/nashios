/**
 * @file kernel/interrupts/irq.c
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
#include <kernel/interrupts/irq.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/pic.h>
#include <kernel/stdio.h>
#include <stddef.h>

static itr_handler_t irq_entries[IRQ_ENTRIES * IRQ_DEPTH];

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void irq_add_handler(uint8_t number, itr_handler_t handler)
{
    for (uint8_t i = 0; i < IRQ_DEPTH; i++)
    {
        if (irq_entries[i * IRQ_ENTRIES + number])
            continue;

        irq_entries[i * IRQ_ENTRIES + number] = handler;
        break;
    }
}

void irq_remove_handler(uint8_t number)
{
    for (uint8_t i = 0; i < IRQ_DEPTH; i++)
        irq_entries[i * IRQ_ENTRIES + number] = NULL;
}

void irq_handler(struct itr_registers *registers)
{
    if (registers->int_no <= 47 && registers->int_no >= 32)
    {
        uint32_t itr_number = registers->int_no - 32;
        for (uint8_t i = 0; i < IRQ_DEPTH; i++)
        {
            itr_handler_t handler = irq_entries[i * IRQ_ENTRIES + (itr_number)];
            if (!handler)
                break;

            handler(registers);
        }
        printf("IRQ: Unhandled interrupt number = %d\n", itr_number);
        pic_eoi(itr_number);
    }
}

void irq_init()
{
    pic_remap(0x20, 0x28);
    printf("IRQ: Remaped PIC\n");

    idt_add_entry(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_add_entry(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_add_entry(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_add_entry(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_add_entry(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_add_entry(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_add_entry(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_add_entry(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_add_entry(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_add_entry(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_add_entry(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_add_entry(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_add_entry(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_add_entry(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_add_entry(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_add_entry(47, (uint32_t)irq15, 0x08, 0x8E);
    printf("IRQ: Added 0..15 interrupts\n");

    printf("IRQ: Initialized\n");
}
