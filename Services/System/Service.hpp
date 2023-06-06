#pragma once

#include <Core/Ini.hpp>
#include <unistd.h>

namespace System
{
    class Service : public std::enable_shared_from_this<Service>
    {
    public:
        Service(const std::unique_ptr<Core::Ini> &ini, const std::string &section);

        void start();

    private:
        static std::map<pid_t, std::shared_ptr<Service>> m_services;

        std::string m_path;
        pid_t m_pid{-1};
    };
} // namespace System
