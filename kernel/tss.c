#include <kernel/gdt.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/tss.h>

struct tss_entry
{
    uint32_t previous_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap;
} __attribute__((packed));

static struct tss_entry s_tss_entry = {};

void tss_flush();

void tss_set_stack(uint32_t esp) { s_tss_entry.esp0 = esp; }

void tss_add(uint32_t index, uint32_t ss, uint32_t esp)
{
    uint32_t base = (uint32_t)&s_tss_entry;
    memset(&s_tss_entry, 0x00, sizeof(struct tss_entry));

    uint32_t limit = base + sizeof(struct tss_entry);
    gdt_add(index, base, limit, 0xE9, 0x00);

    s_tss_entry.ss0 = ss;
    s_tss_entry.esp0 = esp;
    s_tss_entry.cs = 0x0B;
    s_tss_entry.ss = 0x13;
    s_tss_entry.es = 0x13;
    s_tss_entry.ds = 0x13;
    s_tss_entry.fs = 0x13;
    s_tss_entry.gs = 0x13;
    s_tss_entry.iomap = sizeof(struct tss_entry);
}

void tss_init()
{
    tss_flush();
    printf("TSS: Flushed address = 0x%x\n", &s_tss_entry);
    printf("TSS: Initialized\n");
}
