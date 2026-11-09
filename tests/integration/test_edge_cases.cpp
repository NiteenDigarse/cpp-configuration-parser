
#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <stdexcept>

#include "config_parser/config.hpp"

using namespace config_parser;

class ConfigEdgeCaseTest : public ::testing::Test
{
protected:
    const std::string filePath = "edge_case_test.ini";

    void writeFile(const std::string& content)
    {
        std::ofstream file(filePath, std::ios::binary);
        file << content;
    }

    void TearDown() override
    {
        std::remove(filePath.c_str());
    }
};

// 1. Blank and whitespace-only lines
TEST_F(ConfigEdgeCaseTest, IgnoresBlankAndWhitespaceOnlyLines)
{
    writeFile(
        "\n"
        "   \n"
        "[server]\n"
        "\t\n"
        "host = localhost\n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(config.getValue("server", "host"), "localhost");
}

// 2. Full-line comments
TEST_F(ConfigEdgeCaseTest, IgnoresFullLineComments)
{
    writeFile(
        "# Application settings\n"
        "   # Another comment\n"
        "[server]\n"
        "host = localhost\n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(config.getValue("server", "host"), "localhost");
}

// 3. Hash inside value must be preserved
TEST_F(ConfigEdgeCaseTest, PreservesHashInsideValue)
{
    writeFile(
        "[application]\n"
        "message = Hello # World\n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(
        config.getValue("application", "message"),
        "Hello # World"
    );
}

// 4. Multiple equals signs inside value
TEST_F(ConfigEdgeCaseTest, PreservesMultipleEqualsSigns)
{
    writeFile(
        "[database]\n"
        "connection = user=admin=password=secret\n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(
        config.getValue("database", "connection"),
        "user=admin=password=secret"
    );
}

// 5. Empty values are allowed
TEST_F(ConfigEdgeCaseTest, AcceptsEmptyValue)
{
    writeFile(
        "[server]\n"
        "password =\n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(config.getValue("server", "password"), "");
}

// 6. Leading and trailing whitespace is trimmed
TEST_F(ConfigEdgeCaseTest, TrimsWhitespaceAroundKeyAndValue)
{
    writeFile(
        "[server]\n"
        "   host   =   localhost   \n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(config.getValue("server", "host"), "localhost");
}

// 7. Repeated section declarations merge
TEST_F(ConfigEdgeCaseTest, MergesRepeatedSections)
{
    writeFile(
        "[database]\n"
        "host = localhost\n"
        "\n"
        "[database]\n"
        "port = 3306\n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(config.getValue("database", "host"), "localhost");
    EXPECT_EQ(config.getInt("database", "port"), 3306);
}

// 8. Duplicate key in the same section throws
TEST_F(ConfigEdgeCaseTest, RejectsDuplicateKey)
{
    writeFile(
        "[database]\n"
        "port = 3306\n"
        "port = 5432\n"
    );

    Config config;

    EXPECT_THROW(
        config.loadFromFile(filePath),
        std::runtime_error
    );
}

// 9. Same key in different sections is allowed
TEST_F(ConfigEdgeCaseTest, AllowsSameKeyInDifferentSections)
{
    writeFile(
        "[server]\n"
        "host = localhost\n"
        "\n"
        "[database]\n"
        "host = db.local\n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(config.getValue("server", "host"), "localhost");
    EXPECT_EQ(config.getValue("database", "host"), "db.local");
}

// 10. Special characters inside values
TEST_F(ConfigEdgeCaseTest, PreservesSpecialCharacters)
{
    writeFile(
        "[application]\n"
        "path = C:\\Program Files\\MyApp\n"
        "url = https://example.com?a=1&b=2\n"
        "symbols = !@$%^&*()_+-\n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(
        config.getValue("application", "path"),
        "C:\\Program Files\\MyApp"
    );

    EXPECT_EQ(
        config.getValue("application", "url"),
        "https://example.com?a=1&b=2"
    );

    EXPECT_EQ(
        config.getValue("application", "symbols"),
        "!@$%^&*()_+-"
    );
}

// 11. Large configuration file
TEST_F(ConfigEdgeCaseTest, LoadsLargeConfiguration)
{
    std::string content = "[large]\n";

    constexpr int numberOfKeys = 1000;

    for (int i = 0; i < numberOfKeys; ++i)
    {
        content += "key" + std::to_string(i);
        content += " = ";
        content += std::to_string(i);
        content += "\n";
    }

    writeFile(content);

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(config.getInt("large", "key0"), 0);
    EXPECT_EQ(config.getInt("large", "key500"), 500);
    EXPECT_EQ(config.getInt("large", "key999"), 999);
    EXPECT_TRUE(config.hasKey("large", "key500"));
}

// 12. Empty section name after whitespace
TEST_F(ConfigEdgeCaseTest, RejectsWhitespaceOnlySectionName)
{
    writeFile(
        "[   ]\n"
    );

    Config config;

    EXPECT_THROW(
        config.loadFromFile(filePath),
        std::runtime_error
    );
}
