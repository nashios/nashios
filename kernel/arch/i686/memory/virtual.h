#pragma once

#include <kernel/api/posix/limits.h>
#include <kernel/arch/i686/memory/physical.h>

#define PHYS_TO_VIRT(address) (address + KERNEL_HIGHER_HALF)
#define VIRT_TO_PHYS(address) (address - KERNEL_HIGHER_HALF)

#define PAGE_MASK (~(PAGE_SIZE - 1))
#define PAGE_ALIGN(address) (((address) + PAGE_SIZE - 1) & PAGE_MASK)

enum table_flags
{
    PAGE_TBL_PRESENT = 1,
    PAGE_TBL_WRITABLE = 2,
    PAGE_TBL_USER = 4
};

struct page_directory
{
    uint32_t entries[1024];
};

extern struct page_directory *g_virtual_directory;

void virtual_mm_init();
void virtual_mm_map(struct page_directory *directory, uint32_t physical, uint32_t virtual, uint32_t flags);
void virtual_mm_unmap(struct page_directory *directory, uint32_t virtual);
void virtual_mm_unmap_range(struct page_directory *directory, uint32_t start, uint32_t end);
uint32_t virtual_mm_get_physical(uint32_t virtual);
struct page_directory *virtual_mm_create_address();
struct page_directory *virtual_mm_fork(struct page_directory *old_directory);
