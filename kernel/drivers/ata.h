/**
 * @file kernel/drivers/ata.h
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
#pragma once

#include <kernel/dlist.h>
#include <stdint.h>

#define ATA_MASTER 0xA0
#define ATA_IO_ADDR 0x1F0

struct ata_device
{
    const char *name;
    uint16_t io_addr;
    struct dlist_head list;
};

enum ata_reg_status
{
    ATA_REG_ERR = 0x01,
    ATA_REG_IDX,
    ATA_REG_CORR,
    ATA_REG_DRQ = 0x08,
    ATA_REG_SRV,
    ATA_REG_DF = 0x20,
    ATA_REG_RDY,
    ATA_REG_BSY = 0x80
};

enum ata_identify_status
{
    ATA_ERROR,
    ATA_SUCCESS,
    ATA_NOT_FOUND
};

void ata_init();
struct ata_device *ata_get_device(const char *name);
int8_t ata_read(struct ata_device *device, uint16_t *buffer, uint32_t lba, uint8_t sectors);
