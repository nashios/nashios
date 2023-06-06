#pragma once

namespace Core
{
    class EventLoop
    {
    public:
        int run();

        void exit(int exit_code = 0);

    private:
        int m_exit_code{0};
        bool m_should_exit{false};
    };
} // namespace Core
