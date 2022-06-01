/**
 * @file kernel/boot/multiboot.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Multiboot Specification
 * @version 0.1
 * @date 2022-05-24
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
#include <kernel/boot/multiboot.h>
#include <kernel/memory/virtual.h>
#include <kernel/system/sys.h>
#include <kernel/stdio.h>

struct multiboot_info *mbi;

void multiboot_init(uint32_t magic, uint32_t address)
{
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf("Multiboot: Invalid magic number = 0x%X\n", (unsigned)magic);
        PANIC("Invalid multiboot magic", NULL);
    }

    mbi = (struct multiboot_info *)address;
    printf("Multiboot: Flags = 0x%x\n", (unsigned)mbi->flags);

    if (MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_MEMORY_FLAG))
        printf("Multiboot: Memory lower = %uKB, upper = %uKB\n", (unsigned)mbi->mem_lower, (unsigned)mbi->mem_upper);

    if (MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_BOOT_DEVICE_FLAG))
        printf("Multiboot: Boot device = 0x%x\n", (unsigned)mbi->boot_device);

    if (MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_COMMAND_LINE_FLAG))
        printf("Multiboot: Cmdline = %s\n", (char *)PHYS_TO_VIRT(mbi->cmdline));

    if (MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_MODULES_FLAG))
    {
        printf("Multiboot: Modules count = %d, address = 0x%x\n", (int)mbi->mods_count, (int)mbi->mods_addr);

        struct multiboot_mod_list *mod;
        uint32_t i;
        for (i = 0, mod = (struct multiboot_mod_list *)mbi->mods_addr; i < mbi->mods_count; i++, mod++)
            printf("Multiboot: * Module start = 0x%x, end = 0x%x, cmdline = %s\n", (unsigned)mod->mod_start, (unsigned)mod->mod_end, (char *)mod->cmdline);
    }

    if (MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_SYMBOL_TBL_FLAG) && MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_HEADER_TBL_FLAG))
    {
        printf("Multiboot: Both bits 4 and 5 are set.\n");
        PANIC("Multiboot bits 4 and 5 are mutually exclusive", NULL);
    }

    if (MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_SYMBOL_TBL_FLAG))
    {
        struct multiboot_aout_symbol_table *multiboot_aout_sym = &(mbi->u.aout_sym);
        printf("Multiboot: Aout symbol table tabsize = 0x%0x, strsize = 0x%x, address = 0x%x\n", (unsigned)multiboot_aout_sym->tabsize, (unsigned)multiboot_aout_sym->strsize, (unsigned)multiboot_aout_sym->addr);
    }

    if (MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_HEADER_TBL_FLAG))
    {
        struct multiboot_elf_section_header_table *multiboot_elf_sec = &(mbi->u.elf_sec);
        printf("Multiboot: Elf section num = %u, size = 0x%x, address = 0x%x, shndx = 0x%x\n", (unsigned)multiboot_elf_sec->num, (unsigned)multiboot_elf_sec->size, (unsigned)multiboot_elf_sec->addr, (unsigned)multiboot_elf_sec->shndx);
    }

    if (MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_MMAP_FLAG))
    {
        printf("Multiboot: MMap address = 0x%x, length = 0x%x\n", (unsigned)mbi->mmap_addr, (unsigned)mbi->mmap_length);
        for (struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry *)PHYS_TO_VIRT(mbi->mmap_addr); (unsigned long)mmap < PHYS_TO_VIRT(mbi->mmap_addr) + mbi->mmap_length; mmap = (struct multiboot_mmap_entry *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
            printf("Multiboot: * Map size = 0x%x, base_addr = 0x%x%08x, length = 0x%x%08x, type = 0x%x\n", (unsigned)mmap->size, (unsigned)(mmap->addr >> 32), (unsigned)(mmap->addr & 0xffffffff), (unsigned)(mmap->len >> 32), (unsigned)(mmap->len & 0xffffffff), (unsigned)mmap->type);
    }

    if (MULTIBOOT_CHECK_FLAG(mbi->flags, MULTIBOOT_FRAMEBUFFER_FLAG))
    {
        printf("Multiboot: Framebuffer address = 0x%x, type = %d\n", mbi->framebuffer_addr, mbi->framebuffer_type);
        printf("Multiboot: Framebuffer width = %d, height = %d, bpp = %d\n", mbi->framebuffer_width, mbi->framebuffer_height, mbi->framebuffer_bpp);
        printf("Multiboot: Framebuffer palette address = 0x%x, colors = %d\n", mbi->framebuffer_palette_addr, mbi->framebuffer_palette_num_colors);
        printf("Multiboot: Framebuffer red field position = 0x%x, mask size = %d\n", mbi->framebuffer_red_field_position, mbi->framebuffer_red_mask_size);
        printf("Multiboot: Framebuffer green field position = 0x%x, mask size = %d\n", mbi->framebuffer_green_field_position, mbi->framebuffer_green_mask_size);
        printf("Multiboot: Framebuffer blue field position = 0x%x, mask size = %d\n", mbi->framebuffer_blue_field_position, mbi->framebuffer_blue_mask_size);
    }

    printf("Multiboot: Initialized\n");
}
