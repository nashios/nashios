#include <Core/Debug.hpp>
#include <System/Service.hpp>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace System
{
    std::map<pid_t, std::shared_ptr<Service>> Service::m_services;

    Service::Service(const std::unique_ptr<Core::Ini> &ini, const std::string &section)
    {
        m_path = ini->read_string(section, "Path");
    }

    void Service::start()
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            Core::Debug::printl("Failed to fork(%s), result = %d", m_path.c_str(), pid);
            exit(pid);
        }
        else if (pid == 0)
        {
            std::vector<char *> args;
            args.emplace_back(const_cast<char *>(m_path.c_str()));
            args.emplace_back(nullptr);

            std::vector<char *> env;
            env.emplace_back(nullptr);

            auto result = execve(m_path.c_str(), args.data(), env.data());
            if (result < 0)
            {
                Core::Debug::printl("Failed to execve(%s), result = %d", m_path.c_str(), result);
                exit(result);
            }
        }
        else
        {
            m_pid = pid;
            m_services.emplace(pid, shared_from_this());
        }
    }
} // namespace System
