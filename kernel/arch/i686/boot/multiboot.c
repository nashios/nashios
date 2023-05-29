#include <kernel/arch/i686/boot/multiboot.h>
#include <kernel/arch/i686/memory/virtual.h>
#include <kernel/lib/stdio.h>

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

struct multiboot_color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

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

struct multiboot_info *g_multiboot_info = NULL;

void multiboot_init(uint32_t magic, uint32_t address)
{
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf("Multiboot: Invalid magic number = 0x%x\n", magic);
        return;
    }

    g_multiboot_info = (struct multiboot_info *)address;
    printf("Multiboot: Flags = 0x%x\n", g_multiboot_info->flags);

    if (MULTIBOOT_CHECK_FLAG(g_multiboot_info->flags, MULTIBOOT_MEMORY_FLAG))
        printf("Multiboot: Memory lower = %dKB, upper = %dKB\n", g_multiboot_info->mem_lower,
               g_multiboot_info->mem_upper);

    if (MULTIBOOT_CHECK_FLAG(g_multiboot_info->flags, MULTIBOOT_BOOT_DEVICE_FLAG))
        printf("Multiboot: Boot device = 0x%x\n", g_multiboot_info->boot_device);

    if (MULTIBOOT_CHECK_FLAG(g_multiboot_info->flags, MULTIBOOT_COMMAND_LINE_FLAG))
        printf("Multiboot: Command-line = %s\n", PHYS_TO_VIRT(g_multiboot_info->cmdline));

    if (MULTIBOOT_CHECK_FLAG(g_multiboot_info->flags, MULTIBOOT_MODULES_FLAG))
    {
        printf("Multiboot: Modules count = %d, address = 0x%x\n", g_multiboot_info->mods_count,
               g_multiboot_info->mods_addr);

        struct multiboot_mod_list *module = (struct multiboot_mod_list *)g_multiboot_info->mods_addr;
        for (uint32_t i = 0; i < g_multiboot_info->mods_count; i++)
        {
            printf("Multiboot: * Module start = 0x%x, end = 0x%x, command-line = %s\n", module->mod_start,
                   module->mod_end, module->cmdline);
            module++;
        }
    }

    if (MULTIBOOT_CHECK_FLAG(g_multiboot_info->flags, MULTIBOOT_SYMBOL_FLAG) &&
        MULTIBOOT_CHECK_FLAG(g_multiboot_info->flags, MULTIBOOT_ELF_TABLE_FLAG))
    {
        printf("Multiboot: Both bits 4 and 5 are set\n");
        return;
    }

    if (MULTIBOOT_CHECK_FLAG(g_multiboot_info->flags, MULTIBOOT_SYMBOL_FLAG))
    {
        struct multiboot_aout_symbol_table *symbol = &g_multiboot_info->u.aout_sym;
        printf("Multiboot: Aout symbol table tabsize = 0x%x, strsize = 0x%x, address = 0x%x\n", symbol->tabsize,
               symbol->strsize, symbol->addr);
    }

    if (MULTIBOOT_CHECK_FLAG(g_multiboot_info->flags, MULTIBOOT_ELF_TABLE_FLAG))
    {
        struct multiboot_elf_section_header_table *section = &g_multiboot_info->u.elf_sec;
        printf("Multiboot: Elf section number = %d, size = 0x%x, address = 0x%x, shndx = %d\n", section->num,
               section->size, section->addr, section->shndx);
    }

    if (MULTIBOOT_CHECK_FLAG(g_multiboot_info->flags, MULTIBOOT_MMAP_FLAG))
    {
        printf("Multiboot: Mmap address = 0x%x, length = 0x%x\n", g_multiboot_info->mmap_addr,
               g_multiboot_info->mmap_length);

        for (struct multiboot_mmap_entry *mmap =
                 (struct multiboot_mmap_entry *)PHYS_TO_VIRT(g_multiboot_info->mmap_addr);
             (unsigned long)mmap < PHYS_TO_VIRT(g_multiboot_info->mmap_addr) + g_multiboot_info->mmap_length;
             mmap = (struct multiboot_mmap_entry *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
        {
            printf("Multiboot: * Mmap size = 0x%x, address = 0x%x%08x, length = 0x%x%08x, type = %d\n",
                   (unsigned)mmap->size, (unsigned)(mmap->addr >> 32), (unsigned)(mmap->addr & 0xffffffff),
                   (unsigned)(mmap->len >> 32), (unsigned)(mmap->len & 0xffffffff), (unsigned)mmap->type);
        }
    }

    if (MULTIBOOT_CHECK_FLAG(g_multiboot_info->flags, MULTIBOOT_FRAMEBUFFER_FLAG))
    {
        printf("Multiboot: Framebuffer address 0x%x, type = %d\n", g_multiboot_info->framebuffer_addr,
               g_multiboot_info->framebuffer_type);

        printf("Multiboot: Framebuffer width = %d, height = %d, bpp = %d, pitch = %d\n",
               g_multiboot_info->framebuffer_width, g_multiboot_info->framebuffer_height,
               g_multiboot_info->framebuffer_bpp, g_multiboot_info->framebuffer_pitch);

        if (g_multiboot_info->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED)
        {
            printf("Multiboot: Framebuffer palette address = 0x%x, colors = %d\n",
                   g_multiboot_info->framebuffer_palette_addr, g_multiboot_info->framebuffer_palette_num_colors);
        }
        else if (g_multiboot_info->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB)
        {
            printf("Multiboot: * Framebuffer red position = %d, mask = %d\n",
                   g_multiboot_info->framebuffer_red_field_position, g_multiboot_info->framebuffer_red_mask_size);
            printf("Multiboot: * Framebuffer green position = %d, mask = %d\n",
                   g_multiboot_info->framebuffer_green_field_position, g_multiboot_info->framebuffer_green_mask_size);
            printf("Multiboot: * Framebuffer blue position = %d, mask = %d\n",
                   g_multiboot_info->framebuffer_blue_field_position, g_multiboot_info->framebuffer_blue_mask_size);
        }
    }

    printf("Multiboot: Initialized\n");
}
