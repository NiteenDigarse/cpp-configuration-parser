
#include "config_parser/config.hpp"

#include <iostream>
#include <fstream>
#include <string>

using namespace config_parser;

int main()
{
    Config config;

    // Test 1: Create and load original configuration.
    {
        std::ofstream file("old.ini", std::ios::binary);

        file << "[server]\r\n";
        file << "host = old-server\r\n";
        file << "port = 8080\r\n";
    }

    try
    {
        config.loadFromFile("old.ini");

        std::cout << "--- Original Configuration ---\n";
        std::cout << "Host: "
                  << config.getValue("server", "host") << '\n';

        std::cout << "Port: "
                  << config.getInt("server", "port") << "\n\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << '\n';
        return 1;
    }


    // Test 2: Create malformed configuration.
    {
        std::ofstream file("broken.ini");

        file << "[server]\n";
        file << "host = new-server\n";
        file << "port = 9090\n";
        file << "port = 7070\n"; // Duplicate key
    }

    std::cout << "--- Transactional Loading Test ---\n";

    try
    {
        config.loadFromFile("broken.ini");
    }
    catch (const std::exception& e)
    {
        std::cout << "Expected Error: " << e.what() << '\n';
    }

    // Old configuration must still exist.
    std::cout << "Host after failed load: "
              << config.getValue("server", "host") << '\n';

    std::cout << "Port after failed load: "
              << config.getInt("server", "port") << "\n\n";


    // Test 3: Create a UTF-8 BOM + CRLF file.
    {
        std::ofstream file("bom.ini", std::ios::binary);

        // UTF-8 BOM bytes.
        file.put(static_cast<char>(0xEF));
        file.put(static_cast<char>(0xBB));
        file.put(static_cast<char>(0xBF));

        // Configuration with Windows CRLF endings.
        file << "[database]\r\n";
        file << "host = localhost\r\n";
        file << "port = 3306\r\n";
    }

    std::cout << "--- BOM and CRLF Test ---\n";

    try
    {
        config.loadFromFile("bom.ini");

        std::cout << "Database Host: "
                  << config.getValue("database", "host") << '\n';

        std::cout << "Database Port: "
                  << config.getInt("database", "port") << '\n';

        std::cout << "BOM and CRLF handled successfully.\n\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << '\n';
    }


    // Test 4: File open error.
    std::cout << "--- File Open Error Test ---\n";

    try
    {
        config.loadFromFile("missing.ini");
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }

    return 0;
}
