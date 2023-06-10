#include <kernel/api/posix/sys/mman.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/lib/stdio.h>
#include <kernel/lib/stdlib.h>
#include <kernel/lib/string.h>
#include <kernel/memory/mmap.h>
#include <kernel/task/elf.h>
#include <kernel/task/scheduler.h>

#define EI_NIDENT 16

#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

#define PT_LOAD 1

#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6

#define EV_CURRENT 1

#define EM_386 3

#define ET_EXEC 2

#define ELF_MAG0 0x7F
#define ELF_MAG1 'E'
#define ELF_MAG2 'L'
#define ELF_MAG3 'F'
#define ELF_CLASS32 1
#define ELF_DATA2LSB 1

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

struct elf_ehdr
{
    unsigned char e_ident[EI_NIDENT];
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

bool elf_verify(struct elf_ehdr *header)
{
    if (!(header->e_ident[EI_MAG0] == ELF_MAG0 && header->e_ident[EI_MAG1] == ELF_MAG1 &&
          header->e_ident[EI_MAG2] == ELF_MAG2 && header->e_ident[EI_MAG3] == ELF_MAG3))
        return false;

    if (header->e_ident[EI_CLASS] != ELF_CLASS32)
        return false;

    if (header->e_ident[EI_DATA] != ELF_DATA2LSB)
        return false;

    if (header->e_ident[EI_VERSION] != EV_CURRENT)
        return false;

    if (header->e_machine != EM_386)
        return false;

    if (header->e_type != ET_EXEC)
        return false;

    return true;
}

char *elf_read(const char *path)
{
    int fd = virtual_fs_open(path, O_RDONLY, 0x00);
    if (fd < 0)
        return NULL;

    struct stat stat = {};
    if (virtual_fs_fstat(fd, &stat) < 0)
        return NULL;

    char *buffer = (char *)calloc(stat.st_size, sizeof(char));
    if (!buffer)
        return NULL;

    if (virtual_fs_read(fd, buffer, stat.st_size) < 0)
        return NULL;

    return buffer;
}

struct elf_layout *elf_load(const char *path)
{
    char *buffer = elf_read(path);
    if (!buffer)
        return NULL;
    struct elf_ehdr *eheader = (struct elf_ehdr *)buffer;
    if (!elf_verify(eheader) || eheader->e_phoff == 0)
        return NULL;

    struct process_mm *memory = g_scheduler_process->memory;
    for (struct elf_phdr *pheader = (struct elf_phdr *)(buffer + eheader->e_phoff);
         pheader && (char *)pheader < (buffer + eheader->e_phoff + eheader->e_phentsize * eheader->e_phnum); ++pheader)
    {
        if (pheader->p_type != PT_LOAD)
            continue;

        if ((pheader->p_flags & PF_X) != 0 && (pheader->p_flags & PF_R) != 0)
        {
            printf("Elf: Text segment address = 0x%x, size = 0x%x\n", pheader->p_vaddr, pheader->p_memsz);
            mmap((void *)pheader->p_vaddr, pheader->p_memsz, 0, 0, -1, 0);
            memory->code_start = pheader->p_vaddr;
            memory->code_end = pheader->p_vaddr + pheader->p_memsz;
        }
        else if ((pheader->p_flags & PF_W) != 0 && (pheader->p_flags & PF_R) != 0)
        {
            printf("Elf: Data segment address = 0x%x, size = 0x%x\n", pheader->p_vaddr, pheader->p_memsz);
            mmap((void *)pheader->p_vaddr, pheader->p_memsz, 0, 0, -1, 0);
            memory->data_start = pheader->p_vaddr;
            memory->data_end = pheader->p_memsz;
        }
        else
        {
            printf("Elf: Ehframe segment address = 0x%x, size = 0x%x\n", pheader->p_vaddr, pheader->p_memsz);
            mmap((void *)pheader->p_vaddr, pheader->p_memsz, 0, 0, -1, 0);
        }

        memset((char *)pheader->p_vaddr, 0, pheader->p_memsz);
        memcpy((char *)pheader->p_vaddr, buffer + pheader->p_offset, pheader->p_filesz);
    }

    uint32_t heap_start = (uint32_t)mmap(NULL, SCHED_HEAP_SIZE, 0, 0, -1, 0);
    memory->brk_start = heap_start;
    memory->brk_middle = heap_start;
    memory->brk_end = SCHED_HEAP_TOP;

    uint32_t stack_start = (uint32_t)mmap(NULL, SCHED_STACK_SIZE, 0, 0, -1, 0);

    struct elf_layout *layout = (struct elf_layout *)calloc(1, sizeof(struct elf_layout));
    layout->entry = eheader->e_entry;
    layout->stack = stack_start + SCHED_STACK_SIZE;

    return layout;
}

void elf_unload()
{
    struct process_vm *iter;
    struct process_vm *next;
    dlist_for_each_entry_safe(iter, next, &g_scheduler_process->memory->list, list)
    {
        if (!iter->file && (iter->flags & MAP_SHARED) == 0)
        {
            virtual_mm_unmap_range(g_scheduler_process->directory, iter->start, iter->end);
            dlist_remove(&iter->list);
        }
    }

    memset(g_scheduler_process->memory, 0x00, sizeof(struct process_mm));
    dlist_head_init(&g_scheduler_process->memory->list);
}
