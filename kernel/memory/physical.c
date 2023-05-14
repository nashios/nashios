#include <kernel/api/posix/sys/limits.h>
#include <kernel/bitmap.h>
#include <kernel/math.h>
#include <kernel/memory/physical.h>
#include <kernel/memory/virtual.h>
#include <kernel/multiboot.h>
#include <kernel/stdio.h>
#include <kernel/string.h>

extern void *_kernel_boot;
extern void *_kernel_start;
extern void *_kernel_end;
extern void *_kernel_size;

#define KERNEL_BOOT ((uint32_t)&_kernel_boot)
#define KERNEL_START ((uint32_t)&_kernel_start)
#define KERNEL_END ((uint32_t)&_kernel_end)
#define KERNEL_SIZE ((uint32_t)&_kernel_size)

#define PHYSICAL_FRAMES_PER_BYTE 8

static uint32_t *s_physical_bitmap = NULL;
static uint32_t s_physical_used = 0;
static uint32_t s_physical_max = 0;

void physical_mm_set_region(uint32_t address, uint32_t length)
{
    uint32_t frame = address / PAGE_SIZE;
    uint32_t frames = length / PAGE_SIZE;

    for (uint32_t i = 0; i < frames; i++)
    {
        bitmap_unset(s_physical_bitmap, frame + i);
        s_physical_used--;
    }

    bitmap_set(s_physical_bitmap, 0);

    printf("Physical MM: Set region address = 0x%08x, size = 0x%08x\n", address, length);
}

void physical_mm_unset_region(uint32_t address, uint32_t length)
{
    uint32_t frame = address / PAGE_SIZE;
    uint32_t frames = length / PAGE_SIZE;

    for (uint32_t i = 0; i < frames; i++)
    {
        bitmap_set(s_physical_bitmap, frame + i);
        s_physical_used++;
    }

    printf("Physical MM: Unset region address = 0x%08x, size = 0x%08x\n", address, length);
}

void physical_mm_init()
{
    uint32_t memory_size = g_multiboot_info->mem_lower + g_multiboot_info->mem_upper;
    uint32_t bitmap_frames = DIV_ROUND_UP(memory_size * 1024, PAGE_SIZE);
    uint32_t bitmap_size = DIV_ROUND_UP(bitmap_frames, PHYSICAL_FRAMES_PER_BYTE);

    s_physical_used = bitmap_frames;
    s_physical_max = bitmap_frames;

    s_physical_bitmap = (uint32_t *)KERNEL_END;
    memset(s_physical_bitmap, 0x00, bitmap_size);

    for (struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry *)PHYS_TO_VIRT(g_multiboot_info->mmap_addr);
         (unsigned long)mmap < PHYS_TO_VIRT(g_multiboot_info->mmap_addr) + g_multiboot_info->mmap_length;
         mmap = (struct multiboot_mmap_entry *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
    {
        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE)
            continue;

        if ((mmap->addr + mmap->len) > 0xffffffff)
            continue;

        uint32_t address = mmap->addr;
        uint32_t length = mmap->len;
        uint32_t diff = address % PAGE_SIZE;
        if (diff != 0)
        {
            printf("Physical MM: Unaligned region address = 0x%x of 0x%x\n", address, diff);
            diff = PAGE_SIZE - diff;
            address += diff;
            length -= diff;
        }

        if ((length % PAGE_SIZE) != 0)
        {
            printf("Physical MM: Unaligned region length = 0x%x of 0x%x\n", length, length % PAGE_SIZE);
            length -= length % PAGE_SIZE;
        }

        if (length < PAGE_SIZE)
        {
            printf("Physical MM: Too small region, wanted >= 0x%x bytes, got = 0x%x\n", PAGE_SIZE, length);
            continue;
        }

        physical_mm_set_region(address, length);
    }

    physical_mm_unset_region(0x0, KERNEL_BOOT);
    physical_mm_unset_region(KERNEL_BOOT, KERNEL_SIZE + bitmap_size);

    printf("Physical MM: Kernel start = 0x%x, end = 0x%x, size = 0x%x\n", KERNEL_START, KERNEL_END, KERNEL_SIZE);
    printf("Physical MM: Bitmap address = 0x%x, size = 0x%x, used frames = %d, max = %d\n", s_physical_bitmap,
           bitmap_size, s_physical_used, s_physical_max);
    printf("Physical MM: Initialized\n");
}

void *physical_mm_allocate()
{
    if (s_physical_max <= s_physical_used)
        return 0;

    uint32_t frame = bitmap_first_free(s_physical_bitmap, s_physical_max);
    if ((int)frame == -1)
        return 0;

    bitmap_set(s_physical_bitmap, frame);
    s_physical_used++;

    uint32_t address = frame * PAGE_SIZE;
    return (void *)address;
}

void *physical_mm_allocate_size(size_t size)
{
    if (s_physical_max <= s_physical_used)
        return 0;

    if (s_physical_max - s_physical_used < size)
        return 0;

    uint32_t frame = bitmap_first_free_size(s_physical_bitmap, s_physical_max, size);
    if ((int)frame == -1)
        return 0;

    for (uint32_t i = 0; i < size; i++)
    {
        bitmap_set(s_physical_bitmap, frame + i);
        s_physical_used++;
    }

    uint32_t address = frame * PAGE_SIZE;
    return (void *)address;
}
