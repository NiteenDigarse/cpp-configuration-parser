
#include <gtest/gtest.h>

#include "config_parser/config.hpp"

using namespace config_parser;


// Test 1: Section should be added successfully.
TEST(ConfigSectionTest, AddSection)
{
    Config config;

    config.addSection("server");

    EXPECT_TRUE(config.hasSection("server"));
}


// Test 2: Existing section should be found.
TEST(ConfigSectionTest, HasExistingSection)
{
    Config config;

    config.addSection("database");

    EXPECT_TRUE(config.hasSection("database"));
}


// Test 3: Missing section should return false.
TEST(ConfigSectionTest, HasMissingSection)
{
    Config config;

    EXPECT_FALSE(config.hasSection("unknown"));
}


// Test 4: Adding duplicate section should not crash.
TEST(ConfigSectionTest, AddDuplicateSection)
{
    Config config;

    config.addSection("server");
    config.addSection("server");

    EXPECT_TRUE(config.hasSection("server"));
}


// Test 5: Accessing missing section should throw an exception.
TEST(ConfigSectionTest, MissingSectionThrowsException)
{
    Config config;

    EXPECT_THROW(
        config.getValue("unknown", "host"),
        std::runtime_error
    );
}
