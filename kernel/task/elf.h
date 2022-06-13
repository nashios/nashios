/**
 * @file kernel/task/elf.h
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Executable and Linkable Format
 * @version 0.1
 * @date 2022-06-13
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

#define ELFMAG0 0x7F
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define ELFCLASS32 1

#define ELFDATA2LSB 1

#define EV_CURRENT 1

#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_NIDENT 16

#define EM_386 3

#define ET_EXEC 2

#define PT_LOAD 1

#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

#include <stdint.h>

struct elf_layout
{
    uint32_t entry;
    uint32_t stack;
};

struct elf_ehdr
{
    uint8_t e_ident[EI_NIDENT];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

struct elf_phdr
{
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
};

enum elf_verify
{
    ELF_SUCCESS,
    ELF_NOT_ELF_FILE,
    ELF_NOT_SUPPORTED_PLATFORM,
    ELF_NOT_SUPPORTED_ENCODING,
    ELF_NOT_SUPPORTED_TYPE,
    ELF_WRONG_VERSION,
};

struct elf_layout *elf_open(const char *path);
