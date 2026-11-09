
#include <gtest/gtest.h>

#include "config_parser/config.hpp"

using namespace config_parser;


// First GoogleTest test.
TEST(ConfigTest, AddSection)
{
    Config config;

    config.addSection("server");

    EXPECT_TRUE(config.hasSection("server"));
}


// Second GoogleTest test.
TEST(ConfigTest, AddAndGetValue)
{
    Config config;

    config.addSection("server");

    config.addValue(
        "server",
        "host",
        "localhost"
    );

    EXPECT_EQ(
        config.getValue("server", "host"),
        "localhost"
    );
}
