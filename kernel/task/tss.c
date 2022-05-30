/**
 * @file kernel/task/tss.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief x86 Task State Segment
 * @version 0.1
 * @date 2022-05-30
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
#include <kernel/task/tss.h>
#include <kernel/gdt.h>
#include <kernel/stdio.h>

static struct tss_entry tss_entry;

void tss_init(uint8_t index, uint16_t ss0, uint32_t esp0)
{
    uint32_t base = (uint32_t)&tss_entry;
    uint32_t limit = base + sizeof(struct tss_entry);

    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;
    tss_entry.cs = 0x0b;
    tss_entry.ss = 0x13;
    tss_entry.ds = 0x13;
    tss_entry.es = 0x13;
    tss_entry.fs = 0x13;
    tss_entry.gs = 0x13;
    tss_entry.iomap_base = sizeof(struct tss_entry);

    gdt_add_entry(index, base, limit, 0xE9, 0x00);

    printf("TSS: Stack segment = 0x%x, pointer = 0x%x\n", ss0, esp0);
    printf("TSS: Initialized\n");
}
