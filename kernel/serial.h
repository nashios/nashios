#pragma once

#include <stddef.h>

enum SerialPort
{
    SERIAL_INVALID,
    SERIAL_COM1 = 0x3F8,
    SERIAL_COM2 = 0x2F8,
    SERIAL_COM3 = 0x3E8,
    SERIAL_COM4 = 0x2E8,
    SERIAL_COM5 = 0x5F8,
    SERIAL_COM6 = 0x4F8,
    SERIAL_COM7 = 0x5E8,
    SERIAL_COM8 = 0x4E8
};

void serial_init(enum SerialPort port);
void serial_write(const char *buffer, size_t length);
