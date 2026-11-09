
#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <stdexcept>

#include "config_parser/config.hpp"

using namespace config_parser;

class ConfigFileLoadingTest : public ::testing::Test
{
protected:
    const std::string filePath = "integration_test.ini";

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

// 1. Normal file loading
TEST_F(ConfigFileLoadingTest, LoadsNormalConfiguration)
{
    writeFile(
        "[server]\n"
        "host = localhost\n"
        "port = 8080\n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(config.getValue("server", "host"), "localhost");
    EXPECT_EQ(config.getInt("server", "port"), 8080);
}

// 2. Multiple sections and keys
TEST_F(ConfigFileLoadingTest, LoadsMultipleSections)
{
    writeFile(
        "[server]\n"
        "host = localhost\n"
        "\n"
        "[database]\n"
        "host = db.local\n"
        "port = 3306\n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_TRUE(config.hasSection("server"));
    EXPECT_TRUE(config.hasSection("database"));

    EXPECT_EQ(config.getValue("server", "host"), "localhost");
    EXPECT_EQ(config.getValue("database", "host"), "db.local");
    EXPECT_EQ(config.getInt("database", "port"), 3306);
}

// 3. Typed getters from loaded file
TEST_F(ConfigFileLoadingTest, LoadsTypedValues)
{
    writeFile(
        "[application]\n"
        "port = 9000\n"
        "debug = true\n"
        "maintenance = false\n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(config.getInt("application", "port"), 9000);
    EXPECT_TRUE(config.getBool("application", "debug"));
    EXPECT_FALSE(config.getBool("application", "maintenance"));
}

// 4. Empty file
TEST_F(ConfigFileLoadingTest, LoadsEmptyFile)
{
    writeFile("");

    Config config;
    EXPECT_NO_THROW(config.loadFromFile(filePath));
}

// 5. BOM and CRLF handling
TEST_F(ConfigFileLoadingTest, LoadsBOMAndCRLFFile)
{
    const std::string bom = "\xEF\xBB\xBF";

    writeFile(
        bom +
        "[server]\r\n"
        "host = localhost\r\n"
        "port = 8080\r\n"
    );

    Config config;
    config.loadFromFile(filePath);

    EXPECT_EQ(config.getValue("server", "host"), "localhost");
    EXPECT_EQ(config.getInt("server", "port"), 8080);
}

// 6. Failed loading must preserve old configuration
TEST_F(ConfigFileLoadingTest, FailedLoadingPreservesOldConfiguration)
{
    Config config;

    config.addSection("server");
    config.addValue("server", "host", "old-server");
    config.addValue("server", "port", "8080");

    writeFile(
        "[server]\n"
        "host = new-server\n"
        "port = 9000\n"
        "port = 10000\n"
    );

    EXPECT_THROW(config.loadFromFile(filePath), std::runtime_error);

    EXPECT_EQ(config.getValue("server", "host"), "old-server");
    EXPECT_EQ(config.getInt("server", "port"), 8080);
}

// 7. Successful loading replaces old configuration
TEST_F(ConfigFileLoadingTest, SuccessfulLoadingReplacesOldConfiguration)
{
    Config config;

    config.addSection("old");
    config.addValue("old", "key", "old-value");

    writeFile(
        "[new]\n"
        "key = new-value\n"
    );

    config.loadFromFile(filePath);

    EXPECT_FALSE(config.hasSection("old"));
    EXPECT_TRUE(config.hasSection("new"));
    EXPECT_EQ(config.getValue("new", "key"), "new-value");
}

// 8. Missing file
TEST_F(ConfigFileLoadingTest, MissingFileThrowsException)
{
    Config config;

    EXPECT_THROW(
        config.loadFromFile("file_that_does_not_exist.ini"),
        std::runtime_error
    );
}
