#include <kernel/api/framebuffer.h>
#include <kernel/api/posix/errno.h>
#include <kernel/arch/i686/boot/multiboot.h>
#include <kernel/arch/i686/cpu/io.h>
#include <kernel/arch/i686/memory/virtual.h>
#include <kernel/drivers/fb.h>
#include <kernel/drivers/pci.h>
#include <kernel/filesystem/chardev.h>
#include <kernel/filesystem/devfs.h>
#include <kernel/lib/stdio.h>
#include <kernel/memory/mmap.h>
#include <st/math.h>

#define FB_INDEX 0x01CE
#define FB_DATA 0x01CF

#define FB_XRES_REG 0x1
#define FB_YRES_REG 0x2
#define FB_BPP_REG 0x3
#define FB_ENABLE_REG 0x4
#define FB_BANK_REG 0x5
#define FB_VWIDTH_REG 0x6
#define FB_VHEIGHT_REG 0x7

#define FB_ENABLED_REG 0x01
#define FB_LINEAR_FB_REG 0x40

struct fb_display
{
    uint32_t address;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
};

static struct fb_display s_fb_display = {};

void fb_write_field(uint16_t index, uint16_t data)
{
    io_outw(FB_INDEX, index);
    io_outw(FB_DATA, data);
}

uint16_t fb_read_field(uint16_t index)
{
    io_outw(FB_INDEX, index);
    return io_inw(FB_DATA);
}

int fb_init_vboxvga(struct pci_device *device, uint16_t width, uint16_t height)
{
    fb_write_field(FB_ENABLE_REG, 0);
    fb_write_field(FB_XRES_REG, width);
    fb_write_field(FB_YRES_REG, height);
    fb_write_field(FB_VWIDTH_REG, width);
    fb_write_field(FB_VHEIGHT_REG, height * 2);
    fb_write_field(FB_BPP_REG, 32);
    fb_write_field(FB_ENABLE_REG, FB_ENABLED_REG | FB_LINEAR_FB_REG);
    fb_write_field(FB_BANK_REG, 0);
    if (width != fb_read_field(FB_XRES_REG) || height != fb_read_field(FB_YRES_REG))
        return -EINVAL;

    s_fb_display.address = pci_read_bar(device, PCI_BAR0);
    s_fb_display.width = width;
    s_fb_display.height = height;
    s_fb_display.pitch = width * sizeof(uint32_t);

    return 0;
}

void fb_init_multiboot()
{
    s_fb_display.address = g_multiboot_info->framebuffer_addr;
    s_fb_display.width = g_multiboot_info->framebuffer_width;
    s_fb_display.height = g_multiboot_info->framebuffer_height;
    s_fb_display.pitch = g_multiboot_info->framebuffer_pitch;
}

int fb_vscreeninfo(void *arg)
{
    struct fb_var_screeninfo *vsinfo = (struct fb_var_screeninfo *)arg;
    if (!vsinfo)
        return -EINVAL;

    vsinfo->xres = s_fb_display.width;
    vsinfo->yres = s_fb_display.height;
    vsinfo->bits_per_pixel = sizeof(uint32_t);

    return 0;
}

int fb_open(struct vfs_inode *, struct vfs_file *) { return 0; }

int fb_ioctl(struct vfs_inode *, struct vfs_file *, unsigned int cmd, void *arg)
{
    switch (cmd)
    {
    case FBIOGET_VSCREENINFO:
        return fb_vscreeninfo(arg);
        break;
    }
    return 0;
}

int fb_mmap(struct vfs_file *, struct process_vm *area)
{
    uint32_t screen_size = s_fb_display.height * s_fb_display.pitch;
    uint32_t blocks = DIV_ROUND_UP(screen_size, PAGE_SIZE);
    for (uint32_t i = 0; i < blocks; i++)
    {
        virtual_mm_map(g_scheduler_process->directory, s_fb_display.address + i * PAGE_SIZE,
                       area->start + i * PAGE_SIZE, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);
    }

    return 0;
}

static struct vfs_file_op s_fb_op = {.open = fb_open, .ioctl = fb_ioctl, .mmap = fb_mmap};

static struct chardev s_fb_chardev = {.dev = MKDEV(29, 0), .major = 29, .baseminor = 0, .minorct = 0, .op = &s_fb_op};

void fb_init()
{
    if (g_multiboot_info->framebuffer_addr != 0x00 &&
        g_multiboot_info->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB)
    {
        fb_init_multiboot();
        printf("Fb: Using a preset resolution from multiboot without knowing the pci device\n");
    }
    else if (!pci_is_disabled())
    {
        struct pci_device *device;
        dlist_for_each_entry(device, &g_pci_device_list, list)
        {
            bool vga_display_compatible = device->class_code == 0x03 && device->subclass_code == 0x00;
            bool vga_general_pci_compatible = device->class_code == 0x00 && device->subclass_code == 0x01;
            if (!(vga_display_compatible || vga_general_pci_compatible))
                continue;

            if (device->vendor_id != 0x1234 && device->device_id != 0x1111)
                continue;

            fb_init_vboxvga(device, 1024, 768);
        }
    }
    else
    {
        printf("Fb: Failed to find graphic adapter\n");
        return;
    }

    chardev_set(&s_fb_chardev);
    virtual_fs_mknod("/dev/fb0", S_IFCHR, s_fb_chardev.dev);

    uint32_t screen_size = s_fb_display.height * s_fb_display.pitch;
    uint32_t blocks = DIV_ROUND_UP(screen_size, PAGE_SIZE);
    for (uint32_t i = 0; i < blocks; i++)
    {
        virtual_mm_map(g_scheduler_process->directory, s_fb_display.address + i * PAGE_SIZE,
                       s_fb_display.address + i * PAGE_SIZE, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE);
    }

    printf("Fb: Address = 0x%x, width = %d, height = %d, pitch = %d\n", s_fb_display.address, s_fb_display.width,
           s_fb_display.height, s_fb_display.pitch);
    printf("Fb: Initialized\n");
}
