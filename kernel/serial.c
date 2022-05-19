/**
 * @file kernel/serial.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief 16550 UART serial communication
 * @version 0.1
 * @date 2022-05-19
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
#include <kernel/serial.h>
#include <kernel/io.h>

void serial_init()
{
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x80);
    outb(SERIAL_PORT + 0, 0x03);
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x03);
    outb(SERIAL_PORT + 2, 0xC7);
    outb(SERIAL_PORT + 4, 0x0B);
    outb(SERIAL_PORT + 4, 0x1E);
    outb(SERIAL_PORT + 0, 0xAE);

    if (inb(SERIAL_PORT + 0) != 0xAE)
        return;

    outb(SERIAL_PORT + 4, 0x0F);
}

void serial_write_char(char ch)
{
    while ((inb(SERIAL_PORT + 5) & 0x20) == 0)
        ;

    outb(SERIAL_PORT, ch);
}

void serial_write(const char *buffer, size_t length)
{
    for (size_t i = 0; i < length; i++)
        serial_write_char(buffer[i]);
}
