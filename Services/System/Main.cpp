#include <Core/Debug.hpp>
#include <System/Service.hpp>

int main()
{
    auto ini = Core::Ini::open("/etc/SystemService.ini");
    if (!ini)
        return EXIT_FAILURE;

    std::vector<std::shared_ptr<System::Service>> services;
    for (const auto &section : ini->sections())
    {
        auto service = std::make_shared<System::Service>(ini, section);
        services.emplace_back(service);
    }

    Core::Debug::printl("Starting services, count = %d", services.size());
    for (const auto &service : services)
        service->start();

    while (true)
        ;
}
