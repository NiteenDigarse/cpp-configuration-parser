
#pragma once

#include <string>
#include <unordered_map>

namespace config_parser
{

class Config
{
private:

    using Section = std::unordered_map<std::string, std::string>;

    std::unordered_map<std::string, Section> sections;

public:

    Config() = default;

    // Creates a section.
    // Repeated section declarations do not remove existing values.
    void addSection(const std::string& sectionName);

    // Adds or updates a key.
    // If the section does not exist, it is created automatically.
    void addValue(
        const std::string& sectionName,
        const std::string& keyName,
        const std::string& valueName
    );

    // Returns the value of a key.
    // Throws std::runtime_error if the section or key is missing.
    std::string getValue(
        const std::string& sectionName,
        const std::string& keyName
    ) const;

    // Converts a stored value into an integer.
    // Surrounding whitespace is ignored.
    // Throws std::runtime_error for invalid or out-of-range values.
    int getInt(
        const std::string& sectionName,
        const std::string& keyName
    ) const;

    // Accepts true, false, 1 and 0 (case-insensitive).
    // Surrounding whitespace is ignored.
    bool getBool(
        const std::string& sectionName,
        const std::string& keyName
    ) const;

    // Checks whether a section exists.
    bool hasSection(const std::string& sectionName) const;

    // Checks whether a key exists inside a section.
    bool hasKey(
        const std::string& sectionName,
        const std::string& keyName
    ) const;

    // Loads configuration from an INI-style file.
    // Loading is transactional: the existing configuration is replaced
    // only when the entire file is parsed successfully.
    void loadFromFile(const std::string& filePath);
};

} // namespace config_parser
