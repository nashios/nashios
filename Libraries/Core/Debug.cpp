#include <Core/Debug.hpp>
#include <cstdarg>
#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <iterator>
#include <unistd.h>
#include <vector>

namespace Core
{
    std::ofstream Debug::m_file = {};

    void Debug::printl(const char *format, ...)
    {
        if (!m_file.is_open())
        {
            m_file.open("/dev/debug", std::ios::out);
            if (!m_file.is_open())
                return;
        }

        auto output_format = "%s(%d:%d): %s\n";
        auto size = std::snprintf(nullptr, 0, output_format, program_invocation_short_name, getpid(), gettid(), format);
        if (size < 0)
            return;

        std::vector<char> output_buffer(size + 1);
        size = std::snprintf(output_buffer.data(), size, output_format, program_invocation_short_name, getpid(),
                             gettid(), format);
        if (size < 0)
            return;

        va_list args1;
        va_start(args1, format);
        va_list args2;
        va_copy(args2, args1);

        size = std::vsnprintf(nullptr, 0, output_buffer.data(), args1);
        if (size < 0)
            return;
        va_end(args1);

        std::vector<char> format_buffer(size + 1);
        size = std::vsnprintf(format_buffer.data(), format_buffer.size(), output_buffer.data(), args2);
        if (size < 0)
            return;
        va_end(args2);

        std::ostream_iterator<char> iterator(m_file);
        std::copy(format_buffer.begin(), format_buffer.end(), iterator);
        m_file.flush();
    }
} // namespace Core
