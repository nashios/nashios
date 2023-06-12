#include <kernel/arch/i686/interrupts/idt.h>
#include <kernel/arch/i686/interrupts/pic.h>
#include <kernel/cpu/processor.h>
#include <kernel/interrupts/handler.h>
#include <kernel/lib/stdio.h>

#define IDT_ENTRIES 256

struct idt_entry
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_pointer
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry s_idt_entries[IDT_ENTRIES] = {};
static struct idt_pointer s_idt_pointer = {};

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
void isr128();

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

void idt_flush(uint32_t address);

void idt_add(uint8_t index, uint16_t flags, uint16_t selector, idt_handler_t handler)
{
    uint32_t base = (uint32_t)handler;
    s_idt_entries[index].base_low = base & 0xFFFF;
    s_idt_entries[index].base_high = (base >> 16) & 0xFFFF;
    s_idt_entries[index].flags = flags;
    s_idt_entries[index].selector = selector;
    s_idt_entries[index].reserved = 0;
}

void idt_default_handler()
{
    DISABLE_INTERRUPTS();
    printf("IDT: Unknow interrupt number\n");
    PAUSE();
}

void idt_handler(struct itr_registers *registers)
{
    itr_handler(registers);

    if (registers->number >= 32 && registers->number <= 47)
        pic_send_eoi(registers->number - 32);
}

void idt_init()
{
    s_idt_pointer.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;
    s_idt_pointer.base = (uint32_t)&s_idt_entries;

    for (uint16_t i = 0; i < IDT_ENTRIES; i++)
        idt_add(i, 0x8E, 0x08, idt_default_handler);
    printf("IDT: Added default handlers\n");

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
    printf("IDT: Added interrupt service routines from 0..31\n");

    idt_add(128, 0xEE, 0x08, isr128);
    printf("IDT: Added interrupt service routines 128\n");

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

    uint32_t address = (uint32_t)&s_idt_pointer;
    idt_flush(address);
    printf("IDT: Flushed address = 0x%x\n", address);
    printf("IDT: Initialized\n");
}
