#pragma once

#include <fstream>

namespace Core
{
    class Debug
    {
    public:
        static void printl(const char *format, ...);

    private:
        static std::ofstream m_file;
    };
} // namespace Core
