#include <kernel/interrupts/idt.h>
#include <kernel/stdio.h>

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

void idt_default_handler() { printf("IDT: Undefined interrupt number\n"); }

void idt_init()
{
    s_idt_pointer.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;
    s_idt_pointer.base = (uint32_t)&s_idt_entries;

    for (uint16_t i = 0; i < IDT_ENTRIES; i++)
        idt_add(i, 0x8E, 0x08, idt_default_handler);
    printf("IDT: Added default handlers\n");

    uint32_t address = (uint32_t)&s_idt_pointer;
    idt_flush(address);
    printf("IDT: Flushed address = 0x%x\n", address);
    printf("IDT: Initialized\n");
}