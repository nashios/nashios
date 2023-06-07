#pragma once

void Kernel_main();

namespace Kernel
{
    class Init
    {
    public:
        static void main();

    private:
        static void stage2();
    };
} // namespace Kernel
