#include <Kernel/Arch/i686/Boot/Serial.hpp>

extern "C"
{
#include <Kernel/Arch/i686/cpu/io.h>
#include <Kernel/lib/stdio.h>
}

namespace Kernel::Arch::i686::Boot
{
    Serial::Port Serial::s_serial_port = Port::invalid;

    void Serial::init(Port port)
    {
        if (s_serial_port != Port::invalid)
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
        printf("Serial: Initialized\n");
    }

    void Serial::write(char character)
    {
        while ((io_inb(s_serial_port + 5) & 0x20) == 0)
            ;

        io_outb(s_serial_port, character);
    }

    void Serial::write(const char *buffer, size_t length)
    {
        if (s_serial_port == Port::invalid)
            return;

        for (size_t i = 0; i < length; i++)
            write(buffer[i]);
    }
} // namespace Kernel::Arch::i686::Boot

extern "C" void early_printf(const char *buffer, size_t length)
{
    Kernel::Arch::i686::Boot::Serial::write(buffer, length);
}
