#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/irq.h>
#include <kernel/interrupts/pic.h>
#include <kernel/stdio.h>

#define IRQ_CHAIN_SIZE 16
#define IRQ_CHAIN_DEPTH 4

static itr_handler_t s_irq_handlers[IRQ_CHAIN_SIZE * IRQ_CHAIN_DEPTH] = {};

void irq0();
void irq1();
void irq2();
void irq3();
void irq4();
void irq5();
void irq6();
void irq7();
void irq8();
void irq9();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();

void irq_set_handler(uint8_t irq, itr_handler_t handler)
{
    for (uint8_t i = 0; i < IRQ_CHAIN_DEPTH; i++)
    {
        if (s_irq_handlers[i * IRQ_CHAIN_SIZE + irq])
            continue;

        s_irq_handlers[i * IRQ_CHAIN_SIZE + irq] = handler;
        break;
    }
}

void irq_unset_handler(uint8_t irq)
{
    for (uint8_t i = 0; i < IRQ_CHAIN_DEPTH; i++)
        s_irq_handlers[i * IRQ_CHAIN_SIZE + irq] = NULL;
}

void irq_init()
{
    idt_add(32, 0x8E, 0x08, irq0);
    idt_add(33, 0x8E, 0x08, irq1);
    idt_add(34, 0x8E, 0x08, irq2);
    idt_add(35, 0x8E, 0x08, irq3);
    idt_add(36, 0x8E, 0x08, irq4);
    idt_add(37, 0x8E, 0x08, irq5);
    idt_add(38, 0x8E, 0x08, irq6);
    idt_add(39, 0x8E, 0x08, irq7);
    idt_add(40, 0x8E, 0x08, irq8);
    idt_add(41, 0x8E, 0x08, irq9);
    idt_add(42, 0x8E, 0x08, irq10);
    idt_add(43, 0x8E, 0x08, irq11);
    idt_add(44, 0x8E, 0x08, irq12);
    idt_add(45, 0x8E, 0x08, irq13);
    idt_add(46, 0x8E, 0x08, irq14);
    idt_add(47, 0x8E, 0x08, irq15);
    printf("IRQ: Added interrupt request from 0..15\n");

    pic_remap(0x20, 0x28);
    printf("IRQ: Remaped PIC offset_1 = 0x%x, offset_2 = 0x%x\n", 0x20, 0x28);
    printf("IRQ: Initialized\n");
}

void irq_handler(struct registers *registers)
{
    if (registers->number <= 47 && registers->number >= 32)
    {
        for (size_t i = 0; i < IRQ_CHAIN_DEPTH; i++)
        {
            itr_handler_t handler = s_irq_handlers[i * IRQ_CHAIN_SIZE + (registers->number - 32)];
            if (!handler)
                break;

            if (handler(registers))
                return;
        }

        printf("IRQ: Unhandled interrupt number = %d\n", registers->number - 32);
        pic_send_eoi(registers->number - 32);
    }
}
