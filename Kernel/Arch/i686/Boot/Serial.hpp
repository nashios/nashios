#pragma once

#include <stddef.h>

namespace Kernel::Arch::i686::Boot
{
    class Serial
    {
    public:
        enum Port
        {
            invalid,
            com1 = 0x3F8,
            com2 = 0x2F8,
            com3 = 0x3E8,
            com4 = 0x2E8,
            com5 = 0x5F8,
            com6 = 0x4F8,
            com7 = 0x5E8,
            com8 = 0x4E8
        };

        static void init(Port port);
        static void write(const char *buffer, size_t length);

    private:
        static Port s_serial_port;

        static void write(char character);
    };
} // namespace Kernel::Arch::i686::Boot
