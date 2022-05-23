/**
 * @file kernel/interrupts/isr.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief x86 Interrupt Service Routines
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
#include <kernel/interrupts/isr.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/handler.h>
#include <kernel/stdio.h>
#include <stddef.h>

static itr_handler_t isr_entries[ISR_ENTRIES];
static const char *isr_messages[32] = {
    "Divide-by-zero",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double fault",
    "Reserved",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment check",
    "Machine check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"};

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr127();

void isr_add_handler(uint8_t number, itr_handler_t handler)
{
    isr_entries[number] = handler;
}

void isr_remove_handler(uint8_t number)
{
    isr_entries[number] = NULL;
}

void isr_handler(struct itr_registers *registers)
{
    itr_handler_t handler = isr_entries[registers->int_no];
    if (handler)
        handler(registers);
    else
        printf("ISR: Unhandled exception number = %d, message = %s\n", registers->int_no, isr_messages[registers->int_no]);
}

void isr_init()
{
    idt_add_entry(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_add_entry(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_add_entry(2, (uint32_t)isr2, 0x08, 0x8E);
    idt_add_entry(3, (uint32_t)isr3, 0x08, 0x8E);
    idt_add_entry(4, (uint32_t)isr4, 0x08, 0x8E);
    idt_add_entry(5, (uint32_t)isr5, 0x08, 0x8E);
    idt_add_entry(6, (uint32_t)isr6, 0x08, 0x8E);
    idt_add_entry(7, (uint32_t)isr7, 0x08, 0x8E);
    idt_add_entry(8, (uint32_t)isr8, 0x08, 0x8E);
    idt_add_entry(9, (uint32_t)isr9, 0x08, 0x8E);
    idt_add_entry(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_add_entry(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_add_entry(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_add_entry(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_add_entry(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_add_entry(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_add_entry(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_add_entry(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_add_entry(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_add_entry(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_add_entry(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_add_entry(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_add_entry(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_add_entry(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_add_entry(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_add_entry(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_add_entry(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_add_entry(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_add_entry(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_add_entry(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_add_entry(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_add_entry(31, (uint32_t)isr31, 0x08, 0x8E);
    idt_add_entry(127, (uint32_t)isr127, 0x08, 0x8E);
    printf("ISR: Added 0..31 and 127 interrupts\n");

    printf("ISR: Initialized\n");
}
