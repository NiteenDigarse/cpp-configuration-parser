
#include <gtest/gtest.h>

#include <fstream>
#include <stdexcept>
#include <string>

#include "config_parser/config.hpp"

using namespace config_parser;


// Creates a temporary configuration file.
class ConfigValidationTest : public ::testing::Test
{
protected:

    const std::string filePath = "validation_test.ini";

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


// Test 1: Empty section name.
TEST_F(ConfigValidationTest, EmptySectionName)
{
    writeFile("[]\n");

    Config config;

    try
    {
        config.loadFromFile(filePath);
        FAIL() << "Expected an exception";
    }
    catch (const std::runtime_error& error)
    {
        EXPECT_NE(
            std::string(error.what()).find("Empty section name"),
            std::string::npos
        );
    }
}


// Test 2: Missing closing bracket.
TEST_F(ConfigValidationTest, MissingClosingBracket)
{
    writeFile("[server\n");

    Config config;

    EXPECT_THROW(
        config.loadFromFile(filePath),
        std::runtime_error
    );
}


// Test 3: Missing equals sign.
TEST_F(ConfigValidationTest, MissingEquals)
{
    writeFile("[server]\nhost localhost\n");

    Config config;

    EXPECT_THROW(
        config.loadFromFile(filePath),
        std::runtime_error
    );
}


// Test 4: Empty key.
TEST_F(ConfigValidationTest, EmptyKey)
{
    writeFile("[server]\n= localhost\n");

    Config config;

    EXPECT_THROW(
        config.loadFromFile(filePath),
        std::runtime_error
    );
}


// Test 5: Duplicate key.
TEST_F(ConfigValidationTest, DuplicateKey)
{
    writeFile(
        "[server]\n"
        "host = localhost\n"
        "host = 127.0.0.1\n"
    );

    Config config;

    EXPECT_THROW(
        config.loadFromFile(filePath),
        std::runtime_error
    );
}


// Test 6: Key-value before section.
TEST_F(ConfigValidationTest, KeyBeforeSection)
{
    writeFile("host = localhost\n");

    Config config;

    EXPECT_THROW(
        config.loadFromFile(filePath),
        std::runtime_error
    );
}


// Test 7: Unexpected characters after section.
TEST_F(ConfigValidationTest, UnexpectedCharactersAfterSection)
{
    writeFile("[server]extra\n");

    Config config;

    EXPECT_THROW(
        config.loadFromFile(filePath),
        std::runtime_error
    );
}


// Test 8: Error contains filename and line number.
TEST_F(ConfigValidationTest, ErrorContainsFileAndLine)
{
    writeFile(
        "[server]\n"
        "host = localhost\n"
        "port = 8080\n"
        "port = 9090\n"
    );

    Config config;

    try
    {
        config.loadFromFile(filePath);
        FAIL() << "Expected an exception";
    }
    catch (const std::runtime_error& error)
    {
        std::string message = error.what();

        EXPECT_NE(
            message.find(filePath),
            std::string::npos
        );

        EXPECT_NE(
            message.find("Line 4"),
            std::string::npos
        );
    }
}
