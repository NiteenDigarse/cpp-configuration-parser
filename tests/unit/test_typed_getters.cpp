
#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "config_parser/config.hpp"

using namespace config_parser;


// Test 1: Valid integer conversion.
TEST(ConfigTypedGetterTest, ValidInteger)
{
    Config config;

    config.addValue("server", "port", "8080");

    EXPECT_EQ(config.getInt("server", "port"), 8080);
}


// Test 2: Invalid integer conversion.
TEST(ConfigTypedGetterTest, InvalidInteger)
{
    Config config;

    config.addValue("server", "port", "abc");

    EXPECT_THROW(
        config.getInt("server", "port"),
        std::runtime_error
    );
}


// Test 3: Partial integer should be rejected.
TEST(ConfigTypedGetterTest, PartialInteger)
{
    Config config;

    config.addValue("server", "port", "8080abc");

    EXPECT_THROW(
        config.getInt("server", "port"),
        std::runtime_error
    );
}


// Test 4: Integer overflow should be rejected.
TEST(ConfigTypedGetterTest, IntegerOverflow)
{
    Config config;

    config.addValue(
        "server",
        "port",
        "999999999999999999999999999"
    );

    EXPECT_THROW(
        config.getInt("server", "port"),
        std::runtime_error
    );
}


// Test 5: Negative integer should be supported.
TEST(ConfigTypedGetterTest, NegativeInteger)
{
    Config config;

    config.addValue("server", "offset", "-100");

    EXPECT_EQ(config.getInt("server", "offset"), -100);
}


// Test 6: Boolean true should be supported.
TEST(ConfigTypedGetterTest, BooleanTrue)
{
    Config config;

    config.addValue("server", "enabled", "true");

    EXPECT_TRUE(config.getBool("server", "enabled"));
}


// Test 7: Boolean false should be supported.
TEST(ConfigTypedGetterTest, BooleanFalse)
{
    Config config;

    config.addValue("server", "enabled", "false");

    EXPECT_FALSE(config.getBool("server", "enabled"));
}


// Test 8: Uppercase boolean should be supported.
TEST(ConfigTypedGetterTest, UppercaseBoolean)
{
    Config config;

    config.addValue("server", "enabled", "TRUE");

    EXPECT_TRUE(config.getBool("server", "enabled"));
}


// Test 9: Numeric boolean should be supported.
TEST(ConfigTypedGetterTest, NumericBoolean)
{
    Config config;

    config.addValue("server", "enabled", "1");
    config.addValue("server", "disabled", "0");

    EXPECT_TRUE(config.getBool("server", "enabled"));
    EXPECT_FALSE(config.getBool("server", "disabled"));
}


// Test 10: Invalid boolean should throw exception.
TEST(ConfigTypedGetterTest, InvalidBoolean)
{
    Config config;

    config.addValue("server", "enabled", "yes");

    EXPECT_THROW(
        config.getBool("server", "enabled"),
        std::runtime_error
    );
}


// Test 11: Missing section should throw exception.
TEST(ConfigTypedGetterTest, MissingSection)
{
    Config config;

    EXPECT_THROW(
        config.getInt("unknown", "port"),
        std::runtime_error
    );
}


// Test 12: Missing key should throw exception.
TEST(ConfigTypedGetterTest, MissingKey)
{
    Config config;

    config.addSection("server");

    EXPECT_THROW(
        config.getInt("server", "unknown"),
        std::runtime_error
    );
}
