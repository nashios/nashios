#include <kernel/multiboot.h>
#include <kernel/stdio.h>

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

#define MULTIBOOT_CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))

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

enum MultibootFlag
{
    MULTIBOOT_MEMORY_FLAG,
    MULTIBOOT_BOOT_DEVICE_FLAG,
    MULTIBOOT_COMMAND_LINE_FLAG,
    MULTIBOOT_MODULES_FLAG,
    MULTIBOOT_SYMBOL_FLAG,
    MULTIBOOT_ELF_TABLE_FLAG,
    MULTIBOOT_MMAP_FLAG,
    MULTIBOOT_FRAMEBUFFER_FLAG = 12,
};

struct multiboot_info *s_multiboot_info = NULL;

void multiboot_init(uint32_t magic, uint32_t address)
{
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf("Multiboot: Invalid magic number = 0x%x\n", magic);
        return;
    }

    s_multiboot_info = (struct multiboot_info *)address;
    printf("Multiboot: Flags = 0x%x\n", s_multiboot_info->flags);

    if (MULTIBOOT_CHECK_FLAG(s_multiboot_info->flags, MULTIBOOT_MEMORY_FLAG))
        printf("Multiboot: Memory lower = %dKB, upper = %dKB\n", s_multiboot_info->mem_lower,
               s_multiboot_info->mem_upper);

    if (MULTIBOOT_CHECK_FLAG(s_multiboot_info->flags, MULTIBOOT_BOOT_DEVICE_FLAG))
        printf("Multiboot: Boot device = 0x%x\n", s_multiboot_info->boot_device);

    if (MULTIBOOT_CHECK_FLAG(s_multiboot_info->flags, MULTIBOOT_COMMAND_LINE_FLAG))
        printf("Multiboot: Command-line = %s\n", s_multiboot_info->cmdline);

    if (MULTIBOOT_CHECK_FLAG(s_multiboot_info->flags, MULTIBOOT_MODULES_FLAG))
    {
        printf("Multiboot: Modules count = %d, address = 0x%x\n", s_multiboot_info->mods_count,
               s_multiboot_info->mods_addr);

        struct multiboot_mod_list *module = (struct multiboot_mod_list *)s_multiboot_info->mods_addr;
        for (uint32_t i = 0; i < s_multiboot_info->mods_count; i++)
        {
            printf("Multiboot: * Module start = 0x%x, end = 0x%x, command-line = %s\n", module->mod_start,
                   module->mod_end, module->cmdline);
            module++;
        }
    }

    if (MULTIBOOT_CHECK_FLAG(s_multiboot_info->flags, MULTIBOOT_SYMBOL_FLAG) &&
        MULTIBOOT_CHECK_FLAG(s_multiboot_info->flags, MULTIBOOT_ELF_TABLE_FLAG))
    {
        printf("Multiboot: Both bits 4 and 5 are set\n");
        return;
    }

    if (MULTIBOOT_CHECK_FLAG(s_multiboot_info->flags, MULTIBOOT_SYMBOL_FLAG))
    {
        struct multiboot_aout_symbol_table *symbol = &s_multiboot_info->u.aout_sym;
        printf("Multiboot: Aout symbol table tabsize = 0x%x, strsize = 0x%x, address = 0x%x\n", symbol->tabsize,
               symbol->strsize, symbol->addr);
    }

    if (MULTIBOOT_CHECK_FLAG(s_multiboot_info->flags, MULTIBOOT_ELF_TABLE_FLAG))
    {
        struct multiboot_elf_section_header_table *section = &s_multiboot_info->u.elf_sec;
        printf("Multiboot: Elf section number = %d, size = 0x%x, address = 0x%x, shndx = %d\n", section->num,
               section->size, section->addr, section->shndx);
    }

    if (MULTIBOOT_CHECK_FLAG(s_multiboot_info->flags, MULTIBOOT_MMAP_FLAG))
    {
        printf("Multiboot: Mmap address = 0x%x, length = 0x%x\n", s_multiboot_info->mmap_addr,
               s_multiboot_info->mmap_length);

        for (struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry *)s_multiboot_info->mmap_addr;
             (unsigned long)mmap < s_multiboot_info->mmap_addr + s_multiboot_info->mmap_length;
             mmap = (struct multiboot_mmap_entry *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
        {
            printf("Multiboot: * Mmap size = 0x%x, address = 0x%x%08x, length = 0x%x%08x, type = %d\n",
                   (unsigned)mmap->size, (unsigned)(mmap->addr >> 32), (unsigned)(mmap->addr & 0xffffffff),
                   (unsigned)(mmap->len >> 32), (unsigned)(mmap->len & 0xffffffff), (unsigned)mmap->type);
        }
    }

    if (MULTIBOOT_CHECK_FLAG(s_multiboot_info->flags, MULTIBOOT_FRAMEBUFFER_FLAG))
    {
        printf("Multiboot: Framebuffer address 0x%x, type = %d\n", s_multiboot_info->framebuffer_addr,
               s_multiboot_info->framebuffer_type);

        printf("Multiboot: Framebuffer width = %d, height = %d, bpp = %d, pitch = %d\n",
               s_multiboot_info->framebuffer_width, s_multiboot_info->framebuffer_height,
               s_multiboot_info->framebuffer_bpp, s_multiboot_info->framebuffer_pitch);

        if (s_multiboot_info->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED)
        {
            printf("Multiboot: Framebuffer palette address = 0x%x, colors = %d\n",
                   s_multiboot_info->framebuffer_palette_addr, s_multiboot_info->framebuffer_palette_num_colors);
        }
        else if (s_multiboot_info->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB)
        {
            printf("Multiboot: * Framebuffer red position = %d, mask = %d\n",
                   s_multiboot_info->framebuffer_red_field_position, s_multiboot_info->framebuffer_red_mask_size);
            printf("Multiboot: * Framebuffer green position = %d, mask = %d\n",
                   s_multiboot_info->framebuffer_green_field_position, s_multiboot_info->framebuffer_green_mask_size);
            printf("Multiboot: * Framebuffer blue position = %d, mask = %d\n",
                   s_multiboot_info->framebuffer_blue_field_position, s_multiboot_info->framebuffer_blue_mask_size);
        }
    }

    printf("Multiboot: Initialized\n");
}