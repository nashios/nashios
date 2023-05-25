#include <kernel/api/posix/errno.h>
#include <kernel/drivers/ata.h>
#include <kernel/interrupts/irq.h>
#include <kernel/io.h>
#include <kernel/panic.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <kernel/string.h>

#define ATA0_IO_ADDR 0x1F0
#define ATA1_IO_ADDR 0x170

#define ATA_SR_BSY 0x80
#define ATA_SR_DRQ 0x08
#define ATA_SR_DF 0x20
#define ATA_SR_ERR 0x01

#define ATA_BUF_SIZE 256

enum ata_identify
{
    ATA_IDENTIFY_NOT_FOUND,
    ATA_IDENTIFY_FAILED,
    ATA_IDENTIFY_SUCCESS
};

enum ata_polling
{
    ATA_POLLING_FAILED,
    ATA_POLLING_SUCCESS,
};

static struct dlist_head s_ata_device_list = {};

void ata_io_wait(uint16_t io_base)
{
    io_inb(io_base + 7);
    io_inb(io_base + 7);
    io_inb(io_base + 7);
    io_inb(io_base + 7);
}

uint8_t ata_identify_polling(uint16_t io_base)
{
    uint8_t status;
    while (true)
    {
        status = io_inb(io_base + 7);
        if (status == 0)
            return ATA_IDENTIFY_NOT_FOUND;

        if (!(status & ATA_SR_BSY))
            break;
    }

    while (true)
    {
        status = io_inb(io_base + 7);
        if (status & ATA_SR_DRQ)
            break;

        if (status & ATA_SR_ERR || status & ATA_SR_DF)
            return ATA_IDENTIFY_FAILED;
    }

    return ATA_IDENTIFY_SUCCESS;
}

uint8_t ata_identify(uint16_t io_base, bool master)
{
    io_outb(io_base + 6, master ? 0xA0 : 0xB0);
    ata_io_wait(io_base);

    io_outb(io_base + 2, 0x00);
    io_outb(io_base + 3, 0x00);
    io_outb(io_base + 4, 0x00);
    io_outb(io_base + 5, 0x00);
    io_outb(io_base + 7, 0xEC);

    uint8_t status = ata_identify_polling(io_base);
    if (status != ATA_IDENTIFY_SUCCESS)
        return status;

    if (!(io_inb(io_base + 7) & ATA_SR_ERR))
    {
        uint16_t buffer[ATA_BUF_SIZE] = {};
        io_insw(io_base, buffer, ATA_BUF_SIZE);

        return ATA_IDENTIFY_SUCCESS;
    }

    return ATA_IDENTIFY_FAILED;
}

bool ata_detect(const char *name, uint16_t io_base, bool master)
{
    if (ata_identify(io_base, master) != ATA_IDENTIFY_SUCCESS)
        return false;

    struct ata_device *device = (struct ata_device *)calloc(1, sizeof(struct ata_device));
    device->name = strdup(name);
    device->io_base = io_base;
    device->master = master;
    dlist_add_tail(&device->list, &s_ata_device_list);

    printf("ATA: Added device name = %s, io_base = 0x%x\n", name, io_base);
    return true;
}

uint8_t ata_polling(uint16_t io_base)
{
    uint8_t status;
    while (true)
    {
        status = io_inb(io_base + 7);
        if (!(status & ATA_SR_BSY) || (status & ATA_SR_DRQ))
            return ATA_POLLING_SUCCESS;

        if ((status & ATA_SR_ERR) || (status & ATA_SR_DF))
            return ATA_POLLING_FAILED;
    }
}

int ata_read(struct ata_device *device, uint32_t lba, uint8_t sectors, uint16_t *buffer)
{
    io_outb(device->io_base + 6, (device->master ? 0xE0 : 0xF0) | ((lba >> 24) & 0x0F));
    ata_io_wait(device->io_base);

    io_outb(device->io_base + 1, 0x00);
    io_outb(device->io_base + 2, sectors);
    io_outb(device->io_base + 3, (uint8_t)lba);
    io_outb(device->io_base + 4, (uint8_t)(lba >> 8));
    io_outb(device->io_base + 5, (uint8_t)(lba >> 16));
    io_outb(device->io_base + 7, 0x20);

    if (ata_polling(device->io_base) == ATA_POLLING_FAILED)
        return -ENXIO;

    for (int i = 0; i < sectors; ++i)
    {
        io_insw(device->io_base, buffer + i * ATA_BUF_SIZE, ATA_BUF_SIZE);
        ata_io_wait(device->io_base);

        if (ata_polling(device->io_base) == ATA_POLLING_FAILED)
            return -ENXIO;
    }
    return 0;
}

struct ata_device *ata_find_device(const char *name)
{
    struct ata_device *device;
    dlist_for_each_entry(device, &s_ata_device_list, list)
    {
        if (strcmp(device->name, name) == 0)
            return device;
    }
    return NULL;
}

bool ata_handler(struct registers *registers)
{
    pic_send_eoi(registers->number);
    return ITR_CONTINUE;
}

void ata_init()
{
    dlist_head_init(&s_ata_device_list);

    irq_set_handler(14, ata_handler);
    irq_set_handler(15, ata_handler);

    ata_detect("/dev/hda", ATA0_IO_ADDR, true);
    ata_detect("/dev/hdb", ATA0_IO_ADDR, false);
    ata_detect("/dev/hdc", ATA1_IO_ADDR, true);
    ata_detect("/dev/hdd", ATA1_IO_ADDR, false);

    printf("ATA: Initialized\n");
}
