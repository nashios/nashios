#pragma once

#include <Core/EventLoop.hpp>
#include <memory>

namespace Gui
{
    class Application
    {
    public:
        Application();

        static Application &instance();

        int run();

    private:
        static std::unique_ptr<Application> m_instance;

        std::shared_ptr<Core::EventLoop> m_event_loop;
    };
} // namespace Gui
