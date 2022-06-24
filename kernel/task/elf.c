/**
 * @file kernel/task/elf.c
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
#include <kernel/filesystem/virtual.h>
#include <kernel/memory/mmap.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <kernel/string.h>
#include <kernel/task/elf.h>
#include <kernel/task/scheduler.h>

int elf_verify_header(struct elf_ehdr *header)
{
    if (!(header->e_ident[EI_MAG0] == ELFMAG0 && header->e_ident[EI_MAG1] == ELFMAG1 &&
          header->e_ident[EI_MAG2] == ELFMAG2 && header->e_ident[EI_MAG3] == ELFMAG3))
        return ELF_NOT_ELF_FILE;

    if (header->e_ident[EI_CLASS] != ELFCLASS32)
        return ELF_NOT_SUPPORTED_PLATFORM;

    if (header->e_ident[EI_DATA] != ELFDATA2LSB)
        return ELF_NOT_SUPPORTED_ENCODING;

    if (header->e_ident[EI_VERSION] != EV_CURRENT)
        return ELF_WRONG_VERSION;

    if (header->e_machine != EM_386)
        return ELF_NOT_SUPPORTED_PLATFORM;

    if (header->e_type != ET_EXEC)
        return ELF_NOT_SUPPORTED_TYPE;

    return ELF_SUCCESS;
}

struct elf_layout *elf_open(const char *path)
{
    int fd = virt_fs_open(path, O_RDWR);
    if (fd < 0)
        return NULL;

    struct stat stat = {};
    if (virt_fs_fstat(fd, &stat) < 0)
        return NULL;

    char *buffer = calloc(stat.st_size, sizeof(char));
    if (!buffer)
        return NULL;

    if (virt_fs_read(fd, buffer, stat.st_size) < 0)
        return NULL;

    struct elf_ehdr *elf_header = (struct elf_ehdr *)buffer;
    if (elf_verify_header(elf_header) != ELF_SUCCESS)
        return NULL;

    struct elf_phdr *program_header = (struct elf_phdr *)(buffer + elf_header->e_phoff);
    while (program_header &&
           (char *)program_header < (buffer + elf_header->e_phoff + elf_header->e_phentsize * elf_header->e_phnum))
    {
        if (program_header->p_type != PT_LOAD)
            goto next;

        if ((program_header->p_flags & PF_X) != 0 && (program_header->p_flags & PF_R) != 0)
        {
            mmap_map(program_header->p_vaddr, program_header->p_memsz);
            printf("Elf: Text segment address = 0x%x\n", program_header->p_vaddr);
        }
        else if ((program_header->p_flags & PF_W) != 0 && (program_header->p_flags & PF_R) != 0)
        {
            mmap_map(program_header->p_vaddr, program_header->p_memsz);
            printf("Elf: Data segment address = 0x%x\n", program_header->p_vaddr);
        }

        memset((void *)program_header->p_vaddr, 0x0, program_header->p_memsz);
        memcpy((void *)program_header->p_vaddr, buffer + program_header->p_offset, program_header->p_filesz);

    next:
        program_header++;
    }

    struct process *process = sched_current_process();
    uint32_t heap = mmap_map(0x0, SCHED_HEAP);
    process->mm->brk_start = heap;
    process->mm->brk_middle = heap;
    process->mm->brk_end = 0x40000000;

    struct elf_layout *layout = calloc(1, sizeof(struct elf_layout));
    layout->entry = elf_header->e_entry;
    layout->stack = mmap_map(0x0, SCHED_STACK) + SCHED_STACK;

    return layout;
}

void elf_close()
{
    struct process *process = sched_current_process();

    struct mmap_area *area;
    struct mmap_area *next;
    dlist_foreach_entry_safe(area, next, &process->mm->list, list)
    {
        uint32_t virtual = area->start;

        while (virtual < area->end)
        {
            virt_mm_unmap_addr(process->page_dir, virtual);
            virtual += PAGE_SIZE;
        }

        dlist_remove(&area->list);
    }
    memset(process->mm, 0x0, sizeof(struct mmap_mm));
    dlist_head_init(&process->mm->list);
}
