#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/isr.h>
#include <kernel/panic.h>
#include <kernel/processor.h>
#include <kernel/stdio.h>

#define ISR_ENTRIES 256

static itr_handler_t s_isr_handlers[ISR_ENTRIES] = {};

static const char *s_isr_messages[32] = {"Division Error",
                                         "Debug",
                                         "Non-maskable Interrupt",
                                         "Breakpoint",
                                         "Overflow",
                                         "Bound Ranged Exceeded",
                                         "Invalid Opcode",
                                         "Device Not Available",
                                         "Double fault",
                                         "Coprocessor Segment Overrun",
                                         "Invalid TSS",
                                         "Segment Not Present",
                                         "Stack-Segment Fault",
                                         "General Protection Fault",
                                         "Page Fault",
                                         "Reserved",
                                         "x87 Floating-Point Exception",
                                         "Alignment Check",
                                         "Machine Check",
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

void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();
void isr127();

void isr_set_handler(uint8_t index, itr_handler_t handler) { s_isr_handlers[index] = handler; }

void isr_unset_handler(uint8_t index) { s_isr_handlers[index] = 0; }

void isr_init()
{
    idt_add(0, 0x8E, 0x08, isr0);
    idt_add(1, 0x8E, 0x08, isr1);
    idt_add(2, 0x8E, 0x08, isr2);
    idt_add(3, 0x8E, 0x08, isr3);
    idt_add(4, 0x8E, 0x08, isr4);
    idt_add(5, 0x8E, 0x08, isr5);
    idt_add(6, 0x8E, 0x08, isr6);
    idt_add(7, 0x8E, 0x08, isr7);
    idt_add(8, 0x8E, 0x08, isr8);
    idt_add(9, 0x8E, 0x08, isr9);
    idt_add(10, 0x8E, 0x08, isr10);
    idt_add(11, 0x8E, 0x08, isr11);
    idt_add(12, 0x8E, 0x08, isr12);
    idt_add(13, 0x8E, 0x08, isr13);
    idt_add(14, 0x8E, 0x08, isr14);
    idt_add(15, 0x8E, 0x08, isr15);
    idt_add(16, 0x8E, 0x08, isr16);
    idt_add(17, 0x8E, 0x08, isr17);
    idt_add(18, 0x8E, 0x08, isr18);
    idt_add(19, 0x8E, 0x08, isr19);
    idt_add(20, 0x8E, 0x08, isr20);
    idt_add(21, 0x8E, 0x08, isr21);
    idt_add(22, 0x8E, 0x08, isr22);
    idt_add(23, 0x8E, 0x08, isr23);
    idt_add(24, 0x8E, 0x08, isr24);
    idt_add(25, 0x8E, 0x08, isr25);
    idt_add(26, 0x8E, 0x08, isr26);
    idt_add(27, 0x8E, 0x08, isr27);
    idt_add(28, 0x8E, 0x08, isr28);
    idt_add(29, 0x8E, 0x08, isr29);
    idt_add(30, 0x8E, 0x08, isr30);
    idt_add(31, 0x8E, 0x08, isr31);
    printf("ISR: Added interrupt service routines from 0..31\n");

    idt_add(47, 0xEE, 0x08, isr127);
    printf("ISR: Added interrupt service routines 127\n");

    printf("ISR: Initialized\n");
}

void isr_handler(struct registers *registers)
{
    itr_handler_t handler = s_isr_handlers[registers->number];
    if (handler)
        handler(registers);
    else
    {
        printf("ISR: Unhandled exception number = %d, message = %s\n", registers->number,
               s_isr_messages[registers->number]);
        PANIC("Unhandled exception number = %d\n", registers->number);
    }
}
