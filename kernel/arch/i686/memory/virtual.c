#include <kernel/arch/i686/memory/virtual.h>
#include <st/debug.h>
#include <kernel/lib/stdlib.h>
#include <kernel/lib/string.h>
#include <kernel/memory/heap.h>
#include <st/assert.h>

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
    PAGE_DIR_WRITABLE = 2,
    PAGE_DIR_USER = 4
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

    dbgln("Virtual MM: Identity mapped physical = 0x%08x, virtual = 0x%08x, directory = 0x%x", physical_address,
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
    dbgln("Virtual MM: Page enable with page directory address = 0x%x", physical_directory);
    dbgln("Virtual MM: Initialized");
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
        dbgln("Virtual MM: Virtual address = 0x%x is not page aligned", virtual);

    if (!PAGE_IS_ENABLED(directory->entries[PAGE_DIR_INDEX(virtual)]))
        virtual_mm_create_page_table(directory, virtual, flags);

    uint32_t *table = (uint32_t *)((char *)PAGE_TBL_ADDRESS + PAGE_DIR_INDEX(virtual) * PAGE_SIZE);
    table[PAGE_TBL_INDEX(virtual)] = physical | flags;
    flush_tbl(virtual);
}

void virtual_mm_unmap(struct page_directory *directory, uint32_t virtual)
{
    if (!PAGE_IS_ALIGNED(virtual))
        dbgln("Virtual MM: 2 Virtual address = 0x%x is not page aligned", virtual);

    if (!PAGE_IS_ENABLED(directory->entries[PAGE_DIR_INDEX(virtual)]))
        return;

    struct page_table *table = (struct page_table *)(PAGE_TBL_ADDRESS + PAGE_DIR_INDEX(virtual) * PAGE_SIZE);
    uint32_t index = PAGE_TBL_INDEX(virtual);
    if (!PAGE_IS_ENABLED(table->entries[index]))
        return;

    table->entries[index] = 0;
    flush_tbl(virtual);
}

void virtual_mm_unmap_range(struct page_directory *directory, uint32_t start, uint32_t end)
{
    assert(PAGE_IS_ALIGNED(start));
    assert(PAGE_IS_ALIGNED(end));

    for (uint32_t address = start; address < end; address += PAGE_SIZE)
        virtual_mm_unmap(directory, address);
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

    for (int i = 768; i < 1024; i++)
        directory->entries[i] = virtual_mm_get_physical(PAGE_TBL_ADDRESS + i * PAGE_SIZE);

    directory->entries[1023] = virtual_mm_get_physical((uint32_t)directory);
    return directory;
}

struct page_directory *virtual_mm_fork(struct page_directory *directory)
{
    struct page_directory *forked_directory = virtual_mm_create_address();
    char *aligned = heap_align(PAGE_SIZE);
    uint32_t heap = (uint32_t)heap_sbrk(0);

    for (uint32_t i_directory = 0; i_directory < 768; i_directory++)
    {
        if (!PAGE_IS_ENABLED(directory->entries[i_directory]))
            continue;

        struct page_table *forked_table = (struct page_table *)heap;
        uint32_t forked_table_physical = (uint32_t)physical_mm_allocate();
        virtual_mm_map(directory, forked_table_physical, (uint32_t)forked_table,
                       PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);
        memset(forked_table, 0x00, sizeof(struct page_table));

        heap += sizeof(struct page_table);
        struct page_table *new_table = (struct page_table *)(PAGE_TBL_ADDRESS + i_directory * PAGE_SIZE);
        for (uint32_t i_table = 0; i_table < 1024; i_table++)
        {
            if (!PAGE_IS_ENABLED(new_table->entries[i_table]))
                continue;

            char *table_entry = (char *)heap;
            char *forked_entry = table_entry + PAGE_SIZE;
            heap = (uint32_t)(forked_entry + PAGE_SIZE);
            uint32_t forked_entry_physical = (uint32_t)physical_mm_allocate();

            virtual_mm_map(directory, new_table->entries[i_table], (uint32_t)table_entry,
                           PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);
            virtual_mm_map(directory, forked_entry_physical, (uint32_t)forked_entry,
                           PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);
            memcpy(forked_entry, table_entry, PAGE_SIZE);

            virtual_mm_unmap(directory, (uint32_t)table_entry);
            virtual_mm_unmap(directory, (uint32_t)forked_entry);

            forked_table->entries[i_table] =
                forked_entry_physical | PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER;
        }

        virtual_mm_unmap(directory, (uint32_t)forked_table);
        forked_directory->entries[i_directory] =
            forked_table_physical | PAGE_DIR_PRESENT | PAGE_DIR_WRITABLE | PAGE_DIR_USER;
    }

    if (aligned)
        free(aligned);

    return forked_directory;
}
