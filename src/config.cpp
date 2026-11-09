
#include "config_parser/config.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace config_parser
{

    namespace
    {

        // Removes leading and trailing whitespace.
        std::string trim(const std::string &text)
        {
            constexpr const char *whitespace = " \t\r\n";

            const std::size_t first =
                text.find_first_not_of(whitespace);

            if (first == std::string::npos)
            {
                return {};
            }

            const std::size_t last =
                text.find_last_not_of(whitespace);

            return text.substr(first, last - first + 1);
        }

        // Throws an error containing filename and line number.
        [[noreturn]] void throwParseError(
            const std::string &filePath,
            std::size_t lineNumber,
            const std::string &message)
        {
            throw std::runtime_error(
                filePath + ": Line " +
                std::to_string(lineNumber) + ": " + message);
        }

        // valid section name
        bool isValidSectionDeclaration(const std::string &line)
        {
            if (line.size() < 2)
            {
                return false;
            }

            if (line.front() != '[' || line.back() != ']')
            {
                return false;
            }

            const std::string sectionName =
                line.substr(1, line.size() - 2);

            if (sectionName.find('[') != std::string::npos ||
                sectionName.find(']') != std::string::npos)
            {
                return false;
            }

            return true;
        }

    } // anonymous namespace

    void Config::addSection(const std::string &sectionName)
    {
        sections.try_emplace(sectionName);
    }

    void Config::addValue(
        const std::string &sectionName,
        const std::string &keyName,
        const std::string &valueName)
    {
        sections[sectionName].insert_or_assign(
            keyName,
            valueName);
    }

    std::string Config::getValue(
        const std::string &sectionName,
        const std::string &keyName) const
    {
        const auto sectionIt = sections.find(sectionName);

        if (sectionIt == sections.end())
        {
            throw std::runtime_error(
                "Section not found: '" + sectionName + "'");
        }

        const auto keyIt =
            sectionIt->second.find(keyName);

        if (keyIt == sectionIt->second.end())
        {
            throw std::runtime_error(
                "Key '" + keyName +
                "' not found in section '" + sectionName + "'");
        }

        return keyIt->second;
    }

    int Config::getInt(
        const std::string &sectionName,
        const std::string &keyName) const
    {
        const std::string value =
            trim(getValue(sectionName, keyName));

        try
        {
            std::size_t position = 0;

            const int result = std::stoi(value, &position);

            if (position != value.size())
            {
                throw std::runtime_error(
                    "Invalid integer value for key '" +
                    keyName + "' in section '" + sectionName + "'");
            }

            return result;
        }
        catch (const std::invalid_argument &)
        {
            throw std::runtime_error(
                "Invalid integer value for key '" +
                keyName + "' in section '" + sectionName + "'");
        }
        catch (const std::out_of_range &)
        {
            throw std::runtime_error(
                "Integer value out of range for key '" +
                keyName + "' in section '" + sectionName + "'");
        }
    }

    bool Config::getBool(
        const std::string &sectionName,
        const std::string &keyName) const
    {
        std::string value =
            trim(getValue(sectionName, keyName));

        std::transform(
            value.begin(),
            value.end(),
            value.begin(),
            [](unsigned char ch)
            {
                return static_cast<char>(std::tolower(ch));
            });

        if (value == "true" || value == "1")
        {
            return true;
        }

        if (value == "false" || value == "0")
        {
            return false;
        }

        throw std::runtime_error(
            "Invalid boolean value for key '" +
            keyName + "' in section '" + sectionName + "'");
    }

    bool Config::hasSection(const std::string &sectionName) const
    {
        return sections.find(sectionName) != sections.end();
    }

    bool Config::hasKey(
        const std::string &sectionName,
        const std::string &keyName) const
    {
        const auto sectionIt = sections.find(sectionName);

        if (sectionIt == sections.end())
        {
            return false;
        }

        return sectionIt->second.find(keyName) !=
               sectionIt->second.end();
    }

    void Config::loadFromFile(const std::string &filePath)
    {
        // Step 1: Open file in binary mode.
        std::ifstream file(filePath, std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error(
                "Unable to open configuration file: " + filePath);
        }

        // Step 2: Parse into a temporary configuration.
        // Existing configuration remains unchanged until commit.
        Config temporaryConfig;

        std::string line;
        std::string currentSection;

        std::size_t lineNumber = 0;

        // Step 3: Read and parse every line.
        while (std::getline(file, line))
        {
            ++lineNumber;

            // Handle UTF-8 BOM only at the beginning of the file.
            if (lineNumber == 1 && line.size() >= 3)
            {
                if (
                    static_cast<unsigned char>(line[0]) == 0xEF &&
                    static_cast<unsigned char>(line[1]) == 0xBB &&
                    static_cast<unsigned char>(line[2]) == 0xBF)
                {
                    line.erase(0, 3);
                }
            }

            line = trim(line);

            // Ignore blank lines and full-line comments.
            if (line.empty() || line.front() == '#')
            {
                continue;
            }

            // Section parsing.
            if (line.front() == '[')
            {
                if (!isValidSectionDeclaration(line))
                {
                    throwParseError(
                        filePath,
                        lineNumber,
                        "Invalid section declaration.");
                }

                currentSection = trim(
                    line.substr(1, line.size() - 2));

                if (currentSection.empty())
                {
                    throwParseError(
                        filePath,
                        lineNumber,
                        "Empty section name.");
                }

                temporaryConfig.addSection(currentSection);

                continue;
            }

            // Key-value parsing.
            const std::size_t equalPosition = line.find('=');

            if (equalPosition == std::string::npos)
            {
                throwParseError(
                    filePath,
                    lineNumber,
                    "Invalid key-value declaration. Missing '='.");
            }

            if (currentSection.empty())
            {
                throwParseError(
                    filePath,
                    lineNumber,
                    "Key-value found before section.");
            }

            const std::string key = trim(
                line.substr(0, equalPosition));

            const std::string value = trim(
                line.substr(equalPosition + 1));

            if (key.empty())
            {
                throwParseError(
                    filePath,
                    lineNumber,
                    "Empty key.");
            }

            if (temporaryConfig.hasKey(currentSection, key))
            {
                throwParseError(
                    filePath,
                    lineNumber,
                    "Duplicate key '" + key +
                        "' in section '" + currentSection + "'.");
            }

            temporaryConfig.addValue(
                currentSection,
                key,
                value);
        }

        // Step 4: Detect file reading errors.
        if (file.bad())
        {
            throw std::runtime_error(
                "Unable to read configuration file: " + filePath);
        }

        // Step 5: Commit only after successful parsing.
        sections = std::move(temporaryConfig.sections);
    }

} // namespace config_parser
