/**
 * @file kernel/gdt.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief x86 Global Descriptor Table
 * @version 0.1
 * @date 2022-05-19
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
#include <kernel/gdt.h>
#include <kernel/stdio.h>

static struct gdt_ptr gdt_pointer;
static struct gdt_entry gdt_entries[GDT_ENTRIES];

extern void gdt_flush(uint32_t address);

void gdt_add_entry(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
    gdt_entries[index].base_low = (base & 0xFFFF);
    gdt_entries[index].base_middle = (base >> 16) & 0xFF;
    gdt_entries[index].base_high = (base >> 24) & 0xFF;
    gdt_entries[index].limit_low = (limit & 0xFFFF);
    gdt_entries[index].granularity = (limit >> 16) & 0x0F;
    gdt_entries[index].granularity |= granularity & 0xF0;
    gdt_entries[index].access = access;
}

void gdt_init()
{
    gdt_pointer.limit = sizeof(struct gdt_entry) * GDT_ENTRIES - 1;
    gdt_pointer.base = (uint32_t)&gdt_entries;

    gdt_add_entry(0, 0, 0, 0, 0);
    gdt_add_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_add_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_add_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    gdt_add_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt_flush((uint32_t)&gdt_pointer);
    printf("GDT: Flushed address = 0x%08x\n", &gdt_pointer);

    printf("GDT: Initialized\n");
}
