#include <Core/Ini.hpp>
#include <fstream>
#include <sstream>

namespace Core
{
    std::unique_ptr<Ini> Ini::open(const std::string &path)
    {
        auto ini = std::make_unique<Ini>();
        if (!ini->parse(path))
        {
            ini.reset();
            return {};
        }
        return ini;
    }

    std::string Ini::read_string(const std::string &section, const std::string &key,
                                 const std::string &default_value) const
    {
        if (!key_exists(section, key))
            return default_value;

        return m_sections.find(section)->second.find(key)->second;
    }

    int Ini::read_int(const std::string &section, const std::string &key, int default_value) const
    {
        auto value = read_string(section, key);
        if (value.empty())
            return default_value;
        return std::stoi(value);
    }

    bool Ini::read_bool(const std::string &section, const std::string &key, bool default_value) const
    {
        auto value = read_string(section, key, default_value ? "true" : "false");
        return value == "true";
    }

    std::vector<std::string> Ini::sections() const
    {
        std::vector<std::string> sections;
        for (const auto &section : m_sections)
            sections.push_back(section.first);
        return sections;
    }

    bool Ini::parse(const std::string &path)
    {
        m_sections.clear();

        std::ifstream file;
        file.open(path);
        if (!file.is_open())
            return false;

        std::map<std::string, std::string> *current_section = nullptr;
        std::string line;
        while (std::getline(file, line))
        {
            std::size_t i = 0;
            while (i < line.length() && (line[i] == ' ' || line[i] == '\t' || line[i] == '\n'))
                i++;

            if (i >= line.length())
                continue;

            switch (line[i])
            {
            case '#':
            case ';':
                continue;
            case '[': {
                std::stringstream section;
                i++;
                while (i < line.length() && (line[i] != ']'))
                {
                    section << line[i];
                    i++;
                }

                auto pair = std::make_pair(section.str(), std::map<std::string, std::string>());
                current_section = &m_sections.emplace(pair).first->second;
                break;
            }
            default: {
                std::stringstream key;
                while (i < line.length() && (line[i] != '='))
                {
                    key << line[i];
                    i++;
                }
                i++;

                std::stringstream value;
                while (i < line.length() && (line[i] != '\n'))
                {
                    value << line[i];
                    ++i;
                }

                if (!current_section)
                {
                    auto pair = std::make_pair("", std::map<std::string, std::string>());
                    current_section = &m_sections.emplace(pair).first->second;
                }

                auto pair = std::make_pair(key.str(), value.str());
                current_section->emplace(pair);
            }
            }
        }

        return true;
    }

    bool Ini::key_exists(const std::string &section, const std::string &key) const
    {
        if (!m_sections.contains(section))
            return false;
        return m_sections.find(section)->second.contains(key);
    }
} // namespace Core
