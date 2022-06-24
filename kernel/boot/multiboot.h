/**
 * @file multiboot.h
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
#pragma once

#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_PAGE_ALIGN 0x00000001
#define MULTIBOOT_MEMORY_INFO 0x00000002
#define MULTIBOOT_VIDEO_MODE 0x00000004
#define MULTIBOOT_AOUT_KLUDGE 0x00010000

#ifdef __ELF__
#define AOUT_KLUDGE 0
#else
#define AOUT_KLUDGE MULTIBOOT_AOUT_KLUDGE
#endif
#define MULTIBOOT_HEADER_FLAGS MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE | AOUT_KLUDGE

#ifndef ASM_FILE

#define MULTIBOOT_CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))

#define MULTIBOOT_SEARCH 8192
#define MULTIBOOT_HEADER_ALIGN 4

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

#define MULTIBOOT_MOD_ALIGN 0x00001000

#define MULTIBOOT_INFO_ALIGN 0x00000004

#define MULTIBOOT_INFO_MEMORY 0x00000001
#define MULTIBOOT_INFO_BOOTDEV 0x00000002
#define MULTIBOOT_INFO_CMDLINE 0x00000004
#define MULTIBOOT_INFO_MODS 0x00000008

#define MULTIBOOT_INFO_AOUT_SYMS 0x00000010
#define MULTIBOOT_INFO_ELF_SHDR 0X00000020

#define MULTIBOOT_INFO_MEM_MAP 0x00000040

#define MULTIBOOT_INFO_DRIVE_INFO 0x00000080

#define MULTIBOOT_INFO_CONFIG_TABLE 0x00000100

#define MULTIBOOT_INFO_BOOT_LOADER_NAME 0x00000200

#define MULTIBOOT_INFO_APM_TABLE 0x00000400

#define MULTIBOOT_INFO_VBE_INFO 0x00000800
#define MULTIBOOT_INFO_FRAMEBUFFER_INFO 0x00001000

#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB 1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT 2

#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5

#include <stdint.h>

struct multiboot_header
{
    uint32_t magic;

    uint32_t flags;

    uint32_t checksum;

    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;

    uint32_t mode_type;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
};

struct multiboot_aout_symbol_table
{
    uint32_t tabsize;
    uint32_t strsize;
    uint32_t addr;
    uint32_t reserved;
};

struct multiboot_elf_section_header_table
{
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
};

struct multiboot_info
{
    uint32_t flags;

    uint32_t mem_lower;
    uint32_t mem_upper;

    uint32_t boot_device;

    uint32_t cmdline;

    uint32_t mods_count;
    uint32_t mods_addr;

    union {
        struct multiboot_aout_symbol_table aout_sym;
        struct multiboot_elf_section_header_table elf_sec;
    } u;

    uint32_t mmap_length;
    uint32_t mmap_addr;

    uint32_t drives_length;
    uint32_t drives_addr;

    uint32_t config_table;

    uint32_t boot_loader_name;

    uint32_t apm_table;

    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    union {
        struct
        {
            uint32_t framebuffer_palette_addr;
            uint16_t framebuffer_palette_num_colors;
        };
        struct
        {
            uint8_t framebuffer_red_field_position;
            uint8_t framebuffer_red_mask_size;
            uint8_t framebuffer_green_field_position;
            uint8_t framebuffer_green_mask_size;
            uint8_t framebuffer_blue_field_position;
            uint8_t framebuffer_blue_mask_size;
        };
    };
};

struct multiboot_color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct multiboot_mmap_entry
{
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed));

struct multiboot_mod_list
{
    uint32_t mod_start;
    uint32_t mod_end;

    uint32_t cmdline;

    uint32_t pad;
};

struct multiboot_apm_info
{
    uint16_t version;
    uint16_t cseg;
    uint32_t offset;
    uint16_t cseg_16;
    uint16_t dseg;
    uint16_t flags;
    uint16_t cseg_len;
    uint16_t cseg_16_len;
    uint16_t dseg_len;
};

enum multiboot_flags
{
    MULTIBOOT_MEMORY_FLAG = 0,
    MULTIBOOT_BOOT_DEVICE_FLAG = 1,
    MULTIBOOT_COMMAND_LINE_FLAG = 2,
    MULTIBOOT_MODULES_FLAG = 3,
    MULTIBOOT_SYMBOL_TBL_FLAG = 4,
    MULTIBOOT_HEADER_TBL_FLAG = 5,
    MULTIBOOT_MMAP_FLAG = 6,
    MULTIBOOT_FRAMEBUFFER_FLAG = 12
};

extern struct multiboot_info *mbi;

void multiboot_init(uint32_t magic, uint32_t address);

#endif
