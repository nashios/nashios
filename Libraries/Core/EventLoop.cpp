#include <Core/EventLoop.hpp>
#include <cstdlib>

namespace Core
{
    int EventLoop::run()
    {
        while (!m_should_exit)
            ;
        return m_exit_code;
    }

    void EventLoop::exit(int exit_code)
    {
        m_exit_code = exit_code;
        m_should_exit = true;
    }
} // namespace Core
