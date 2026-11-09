
#include <gtest/gtest.h>

#include "config_parser/config.hpp"

using namespace config_parser;


// Test 1: Add and retrieve a value.
TEST(ConfigValueTest, AddAndGetValue)
{
    Config config;

    config.addSection("server");
    config.addValue("server", "host", "localhost");

    EXPECT_EQ(config.getValue("server", "host"), "localhost");
}


// Test 2: Same key in different sections.
TEST(ConfigValueTest, SameKeyInDifferentSections)
{
    Config config;

    config.addValue("server", "host", "localhost");
    config.addValue("database", "host", "127.0.0.1");

    EXPECT_EQ(config.getValue("server", "host"), "localhost");
    EXPECT_EQ(config.getValue("database", "host"), "127.0.0.1");
}


// Test 3: Missing key should throw an exception.
TEST(ConfigValueTest, MissingKeyThrowsException)
{
    Config config;

    config.addSection("server");

    EXPECT_THROW(
        config.getValue("server", "unknown"),
        std::runtime_error
    );
}


// Test 4: Empty value should be supported.
TEST(ConfigValueTest, EmptyValue)
{
    Config config;

    config.addValue("server", "host", "");

    EXPECT_TRUE(config.hasKey("server", "host"));
    EXPECT_EQ(config.getValue("server", "host"), "");
}


// Test 5: addValue should update an existing key.
TEST(ConfigValueTest, UpdateExistingValue)
{
    Config config;

    config.addValue("server", "port", "8080");
    config.addValue("server", "port", "9090");

    EXPECT_EQ(config.getValue("server", "port"), "9090");
}


// Test 6: Multiple values in one section.
TEST(ConfigValueTest, MultipleValuesInSection)
{
    Config config;

    config.addValue("server", "host", "localhost");
    config.addValue("server", "port", "8080");
    config.addValue("server", "enabled", "true");

    EXPECT_EQ(config.getValue("server", "host"), "localhost");
    EXPECT_EQ(config.getValue("server", "port"), "8080");
    EXPECT_EQ(config.getValue("server", "enabled"), "true");
}
