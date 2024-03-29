#include <kernel/arch/x86/boot/serial.h>
#include <kernel/arch/x86/cpu/io.h>
#include <st/debug.h>

static enum serial_port s_serial_port = SERIAL_INVALID;

void serial_init(enum serial_port port)
{
    if (s_serial_port != SERIAL_INVALID)
        return;

    s_serial_port = port;

    io_outb(s_serial_port + 1, 0x00);
    io_outb(s_serial_port + 3, 0x80);
    io_outb(s_serial_port + 0, 0x03);
    io_outb(s_serial_port + 1, 0x00);
    io_outb(s_serial_port + 3, 0x03);
    io_outb(s_serial_port + 2, 0xC7);
    io_outb(s_serial_port + 4, 0x0B);
    io_outb(s_serial_port + 4, 0x1E);
    io_outb(s_serial_port + 0, 0xAE);

    if (io_inb(s_serial_port + 0) != 0xAE)
        return;

    io_outb(s_serial_port + 4, 0x0F);
    dbgln("Serial: Initialized");
}

void serial_write_char(char character)
{
    while ((io_inb(s_serial_port + 5) & 0x20) == 0)
        ;

    io_outb(s_serial_port, character);
}

void serial_write(const char *buffer, size_t length)
{
    if (s_serial_port == SERIAL_INVALID)
        return;

    for (size_t i = 0; i < length; i++)
        serial_write_char(buffer[i]);
}

void early_printf(const char *buffer, size_t length) { serial_write(buffer, length); }
