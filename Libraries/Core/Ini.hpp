#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Core
{
    class Ini
    {
    public:
        static std::unique_ptr<Ini> open(const std::string &path);

        std::string read_string(const std::string &section, const std::string &key,
                                const std::string &default_value = "") const;
        int read_int(const std::string &section, const std::string &key, int default_value = 0) const;
        bool read_bool(const std::string &section, const std::string &key, bool default_value = false) const;

        std::vector<std::string> sections() const;

    private:
        std::map<std::string, std::map<std::string, std::string>> m_sections;

        bool parse(const std::string &path);
        bool key_exists(const std::string &section, const std::string &key) const;
    };
} // namespace Core
