#include <Gui/Application.hpp>

namespace Gui
{
    std::unique_ptr<Application> Application::m_instance = nullptr;

    Application::Application() : m_event_loop(std::make_shared<Core::EventLoop>()) {}

    Application &Application::instance()
    {
        if (m_instance == nullptr)
            m_instance = std::make_unique<Application>();
        return *m_instance;
    }

    int Application::run() { return m_event_loop->run(); }
} // namespace Gui
