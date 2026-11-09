
#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "config_parser/config.hpp"

using namespace config_parser;


// 1. addValue creates a missing section automatically.
TEST(ConfigApiContractTest, AddValueCreatesMissingSection)
{
    Config config;

    config.addValue("database", "port", "3306");

    EXPECT_TRUE(config.hasSection("database"));
    EXPECT_TRUE(config.hasKey("database", "port"));

    EXPECT_EQ(
        config.getValue("database", "port"),
        "3306"
    );
}


// 2. addValue updates an existing value.
TEST(ConfigApiContractTest, AddValueUpdatesExistingKey)
{
    Config config;

    config.addValue("server", "host", "localhost");
    config.addValue("server", "host", "production");

    EXPECT_EQ(
        config.getValue("server", "host"),
        "production"
    );
}


// 3. getInt accepts surrounding whitespace.
TEST(ConfigApiContractTest, GetIntAcceptsSurroundingWhitespace)
{
    Config config;

    config.addValue("server", "port", "  8080  ");

    EXPECT_EQ(config.getInt("server", "port"), 8080);
}


// 4. getBool accepts surrounding whitespace.
TEST(ConfigApiContractTest, GetBoolAcceptsSurroundingWhitespace)
{
    Config config;

    config.addValue("application", "debug", "  TRUE  ");

    EXPECT_TRUE(
        config.getBool("application", "debug")
    );
}


// 5. getInt rejects trailing non-whitespace characters.
TEST(ConfigApiContractTest, GetIntRejectsTrailingCharacters)
{
    Config config;

    config.addValue("server", "port", "8080abc");

    EXPECT_THROW(
        config.getInt("server", "port"),
        std::runtime_error
    );
}
