#include <kernel/gdt.h>
#include <kernel/stdio.h>
#include <kernel/tss.h>

#define GDT_ENTRIES 6

struct gdt_entry
{
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_pointer
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct gdt_entry s_gdt_entries[GDT_ENTRIES] = {};
struct gdt_pointer s_gdt_pointer = {};

void gdt_flush(uint32_t);

void gdt_add(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
    s_gdt_entries[index].base_low = (base & 0xFFFF);
    s_gdt_entries[index].base_middle = (base >> 16) & 0xFF;
    s_gdt_entries[index].base_high = (base >> 24) & 0xFF;
    s_gdt_entries[index].limit = (limit & 0xFFFF);
    s_gdt_entries[index].granularity = (limit >> 16) & 0X0F;
    s_gdt_entries[index].granularity |= (granularity & 0xF0);
    s_gdt_entries[index].access = access;

    printf("GDT: Added index = %d, base = 0x%x, limit = 0x%x, access = 0x%x, granularity = 0x%x\n", index, base, limit,
           access, granularity);
}

void gdt_init()
{
    s_gdt_pointer.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    s_gdt_pointer.base = (uint32_t)&s_gdt_entries;

    gdt_add(0, 0x00, 0x00000000, 0x00, 0x00);
    gdt_add(1, 0x00, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_add(2, 0x00, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_add(3, 0x00, 0xFFFFFFFF, 0xFA, 0xCF);
    gdt_add(4, 0x00, 0xFFFFFFFF, 0xF2, 0xCF);
    tss_add(5, 0x10, 0x00);

    uint32_t address = (uint32_t)&s_gdt_pointer;
    gdt_flush(address);
    printf("GDT: Flushed address = 0x%x\n", address);

    tss_init();

    printf("GDT: Initialized\n");
}
