/**
 * @file kernel/drivers/ata.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Advanced Technology Attachment
 * @version 0.1
 * @date 2022-06-07
 *
 * @copyright Copyright (C) 2022 Saullo Bretas Silva
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include <kernel/drivers/ata.h>
#include <kernel/system/sys.h>
#include <kernel/stdlib.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/io.h>

static struct dlist_head ata_list;

struct ata_device *ata_get_device(const char *name)
{
    struct ata_device *device;
    dlist_foreach_entry(device, &ata_list, list)
    {
        if (strcmp(device->name, name) == 0)
            return device;
    }
    return NULL;
}

void ata_delay(uint16_t io_addr)
{
    inb(io_addr + 7);
    inb(io_addr + 7);
    inb(io_addr + 7);
    inb(io_addr + 7);
}

uint8_t ata_identify_polling(uint16_t io_addr)
{
    while (true)
    {
        uint8_t status = inb(io_addr + 7);
        if (status == 0)
            return ATA_NOT_FOUND;
        if (!(status & ATA_REG_BSY))
            break;
        if (status & ATA_REG_DRQ)
            break;
        if (status & ATA_REG_ERR || status & ATA_REG_DF)
            return ATA_ERROR;
    }
    return ATA_SUCCESS;
}

uint8_t ata_identify(uint16_t io_addr)
{
    outb(io_addr + 6, ATA_MASTER);
    ata_delay(io_addr);

    outb(io_addr + 2, 0);
    outb(io_addr + 3, 0);
    outb(io_addr + 4, 0);
    outb(io_addr + 5, 0);
    outb(io_addr + 7, 0xEC);

    uint8_t status = ata_identify_polling(io_addr);
    if (status != ATA_SUCCESS)
        return status;

    if (!(inb(io_addr + 7) & ATA_REG_ERR))
    {
        uint16_t buffer[256];
        insw(io_addr, buffer, 256);
        return ATA_SUCCESS;
    }
    return ATA_ERROR;
}

struct ata_device *ata_detect(const char *name, uint16_t io_addr)
{
    if (ata_identify(io_addr) == ATA_SUCCESS)
    {
        struct ata_device *device = calloc(1, sizeof(struct ata_device));
        device->io_addr = io_addr;
        device->name = strdup(name);

        dlist_add_tail(&device->list, &ata_list);

        printf("ATA: Detected device name = %s, io = 0x%x\n", device->name, io_addr);
        return device;
    }
    return NULL;
}

void ata_init()
{
    dlist_head_init(&ata_list);

    if (ata_detect("/dev/hda", ATA_IO_ADDR) == NULL)
        PANIC("ATA: Failed to identify /dev/hda", NULL);

    printf("ATA: Initialized\n");
}
