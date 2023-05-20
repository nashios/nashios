#include <kernel/memory/heap.h>
#include <kernel/memory/virtual.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <kernel/string.h>

#define PAGE_TBL_ADDRESS 0xFFC00000
#define PAGE_DIR_ADDRESS 0xFFFFF000

#define PAGE_TBL_INDEX(address) (((address) >> 12) & 0x3FF)
#define PAGE_DIR_INDEX(address) (((address) >> 22) & 0x3FF)
#define PAGE_IS_ENABLED(address) (address & 0x01)
#define PAGE_IS_ALIGNED(address) ((address) == PAGE_ALIGN(address))

struct page_table
{
    uint32_t entries[1024];
};

enum directory_flags
{
    PAGE_DIR_PRESENT = 1,
    PAGE_DIR_WRITABLE = 2
};

struct page_directory *g_virtual_directory = NULL;

void page_enable(uint32_t address);
void flush_tbl(uint32_t address);

void virtual_mm_identity_map(struct page_directory *directory, uint32_t physical_address, uint32_t virtual_address)
{
    uint32_t physical_table = (uint32_t)physical_mm_allocate();
    struct page_table *table = (struct page_table *)PHYS_TO_VIRT(physical_table);
    memset(table, 0x00, sizeof(struct page_table));

    for (uint32_t i = 0, i_physical_address = physical_address, i_virtual_address = virtual_address; i < 1024;
         i++, i_physical_address += PAGE_SIZE, i_virtual_address += PAGE_SIZE)
    {
        uint32_t *page = &table->entries[PAGE_TBL_INDEX(i_virtual_address)];
        *page = i_physical_address | PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE;
        physical_mm_mark(i_physical_address);
    }

    uint32_t *entry = &directory->entries[PAGE_DIR_INDEX(virtual_address)];
    *entry = physical_table | PAGE_DIR_PRESENT | PAGE_DIR_WRITABLE;

    printf("Virtual MM: Identity mapped physical = 0x%08x, virtual = 0x%08x, directory = 0x%x\n", physical_address,
           virtual_address, &directory);
}

void virtual_mm_allocate_tbl(struct page_directory *directory, int index)
{
    if (PAGE_IS_ENABLED(directory->entries[index]))
        return;

    uint32_t physical = (uint32_t)physical_mm_allocate();
    directory->entries[index] = physical | PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE;
}

void virtual_mm_init()
{
    uint32_t physical_directory = (uint32_t)physical_mm_allocate();
    struct page_directory *directory = (struct page_directory *)PHYS_TO_VIRT(physical_directory);
    memset(directory, 0x00, sizeof(struct page_directory));

    virtual_mm_identity_map(directory, 0x00000000, 0xC0000000);

    for (int i = 769; i < 1024; i++)
        virtual_mm_allocate_tbl(directory, i);

    directory->entries[1023] = (physical_directory & PAGE_DIR_ADDRESS) | PAGE_DIR_PRESENT | PAGE_DIR_WRITABLE;

    g_virtual_directory = directory;
    page_enable(physical_directory);
    printf("Virtual MM: Page enable with page directory address = 0x%x\n", physical_directory);
    printf("Virtual MM: Initialized\n");
}

void virtual_mm_create_page_table(struct page_directory *directory, uint32_t virtual, uint32_t flags)
{
    if (PAGE_IS_ENABLED(directory->entries[PAGE_DIR_INDEX(virtual)]))
        return;

    uint32_t table_address = (uint32_t)physical_mm_allocate();
    directory->entries[PAGE_DIR_INDEX(virtual)] = table_address | flags;
    flush_tbl(virtual);

    memset((char *)PAGE_TBL_ADDRESS + PAGE_DIR_INDEX(virtual) * PAGE_SIZE, 0x00, sizeof(struct page_table));
}

void virtual_mm_map(struct page_directory *directory, uint32_t physical, uint32_t virtual, uint32_t flags)
{
    if (!PAGE_IS_ALIGNED(virtual))
        printf("Virtual MM: Virtual address = 0x%x is not page aligned\n", virtual);

    if (!PAGE_IS_ENABLED(directory->entries[PAGE_DIR_INDEX(virtual)]))
        virtual_mm_create_page_table(directory, virtual, flags);

    uint32_t *table = (uint32_t *)((char *)PAGE_TBL_ADDRESS + PAGE_DIR_INDEX(virtual) * PAGE_SIZE);
    table[PAGE_TBL_INDEX(virtual)] = physical | flags;
    flush_tbl(virtual);
}

uint32_t virtual_mm_get_physical(uint32_t virtual)
{
    uint32_t *table = (uint32_t *)((char *)PAGE_TBL_ADDRESS + PAGE_DIR_INDEX(virtual) * PAGE_SIZE);
    return table[PAGE_TBL_INDEX(virtual)];
}

struct page_directory *virtual_mm_create_address()
{
    char *aligned = heap_align(PAGE_SIZE);
    struct page_directory *directory = calloc(1, sizeof(struct page_directory));
    if (aligned)
        free(aligned);

    for (int i = 768; i < 1023; i++)
        directory->entries[i] = virtual_mm_get_physical(PAGE_TBL_ADDRESS + i * PAGE_SIZE);

    directory->entries[1023] = virtual_mm_get_physical((uint32_t)directory);
    return directory;
}