#include <kernel/api/posix/sys/limits.h>
#include <kernel/memory/virtual.h>
#include <kernel/stdio.h>
#include <kernel/string.h>

#define PAGE_TBL_ADDRESS 0xFFC00000
#define PAGE_DIR_ADDRESS 0xFFFFF000
#define PAGE_MASK ~(PAGE_SIZE - 1)

#define PAGE_TBL_INDEX(address) ((address >> 12) & 0x3FF)
#define PAGE_DIR_INDEX(address) ((address >> 22) & 0x3FF)
#define PAGE_ALIGN(address) ((address + PAGE_SIZE - 1) & PAGE_MASK)
#define PAGE_IS_ENABLED(address) (address & 0x01)
#define PAGE_IS_ALIGNED(address) (address == PAGE_ALIGN(address))

struct page_table
{
    uint32_t entries[1024];
};

struct page_directory
{
    uint32_t entries[1024];
};

enum table_flags
{
    PAGE_TBL_PRESENT = 1,
    PAGE_TBL_WRITABLE = 2
};

enum directory_flags
{
    PAGE_DIR_PRESENT = 1,
    PAGE_DIR_WRITABLE = 2
};

static struct page_directory *s_virtual_directory = NULL;

void page_enable(uint32_t address);
void flush_tbl(uint32_t address);

void virtual_mm_identity_map(uint32_t physical_address, uint32_t virtual_address)
{
    uint32_t physical_table = (uint32_t)physical_mm_allocate();
    struct page_table *table = (struct page_table *)PHYS_TO_VIRT(physical_table);
    memset((void *)table, 0x00, sizeof(struct page_table));

    for (uint32_t i = 0, i_physical_address = physical_address, i_virtual_address = virtual_address; i < 1024;
         i++, i_physical_address += 4096, i_virtual_address += 4096)
    {
        uint32_t *page = &table->entries[PAGE_TBL_INDEX(i_virtual_address)];
        *page = i_physical_address | PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE;
    }

    uint32_t *entry = &s_virtual_directory->entries[PAGE_DIR_INDEX(virtual_address)];
    *entry = physical_table | PAGE_DIR_PRESENT | PAGE_DIR_WRITABLE;

    printf("Virtual MM: Identity mapped physical = 0x%08x, virtual = 0x%x, directory = 0x%x\n", physical_address,
           virtual_address, &s_virtual_directory);
}

void virtual_mm_init()
{
    uint32_t physical_directory = (uint32_t)physical_mm_allocate();
    struct page_directory *directory = (struct page_directory *)PHYS_TO_VIRT(physical_directory);
    s_virtual_directory = directory;
    virtual_mm_identity_map(0x00000000, 0xC0000000);

    directory->entries[1023] = (physical_directory & 0xFFFFF000) | PAGE_DIR_PRESENT | PAGE_DIR_WRITABLE;

    page_enable(physical_directory);
    printf("Virtual MM: Page enable with page directory address = 0x%x\n", physical_directory);
    printf("Virtual MM: Initialized\n");
}

void virtual_mm_create_page_table(uint32_t virtual, uint32_t flags)
{
    if (PAGE_IS_ENABLED(s_virtual_directory->entries[PAGE_DIR_INDEX(virtual)]))
        return;

    uint32_t table_address = (uint32_t)physical_mm_allocate();
    s_virtual_directory->entries[PAGE_DIR_INDEX(virtual)] = table_address | flags;
    flush_tbl(virtual);

    memset((char *)PAGE_TBL_ADDRESS + PAGE_DIR_INDEX(virtual) * PAGE_SIZE, 0x00, sizeof(struct page_table));
}

void virtual_mm_map(uint32_t physical, uint32_t virtual, uint32_t flags)
{
    if (!PAGE_IS_ALIGNED(virtual))
        printf("Virtual MM: Virtual address = 0x%x is not page aligned\n", virtual);

    if (!PAGE_IS_ENABLED(s_virtual_directory->entries[PAGE_DIR_INDEX(virtual)]))
        virtual_mm_create_page_table(virtual, flags);

    char *table = ((char *)PAGE_TBL_ADDRESS + PAGE_DIR_INDEX(virtual) * PAGE_SIZE);
    table[PAGE_TBL_INDEX(virtual)] = physical | flags;
    flush_tbl(virtual);
}
