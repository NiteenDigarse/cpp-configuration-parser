C++ Configuration Parser
A lightweight, reusable configuration parser library written in
C++17. It reads INI-style configuration files, stores values by
section and key, provides typed getters, validates malformed input, and
preserves the existing configuration if loading a new file fails.
Features
- Section-based configuration storage.
- Key-value insertion and retrieval.
- Automatic section creation when adding a value.
- Existing key update through addValue().
- String, integer, and Boolean getters.
- Case-insensitive Boolean parsing.
- Whitespace trimming around section names, keys, and values.
- Blank-line and full-line # comment handling.
- Empty values and values containing additional = characters.
- Repeated section declarations are merged.
- Duplicate keys in a file are rejected.
- UTF-8 BOM and Windows CRLF input handling.
- Parsing errors include the file path and line number.
- Transactional file loading: failed parsing does not replace the
  current configuration.
- CMake-based build.
- GoogleTest-based automated tests.
- Default and Release benchmark executable.
Project Structure
cpp-Configuration-parser/
|
+-- include/
|   +-- config_parser/
|       +-- config.hpp
|
+-- src/
|   +-- config.cpp
|
+-- examples/
|   +-- main.cpp
|
+-- tests/
|   +-- unit/
|   +-- integration/
|   +-- CMakeLists.txt
|
+-- benchmarks/
|   +-- config_benchmark.cpp
|
+-- docs/
|   +-- design/
|   |   +-- DESIGN.md
|   +-- testing/
|   |   +-- TESTING.md
|   +-- benchmark/
|       +-- BENCHMARK_NOTES.md
|       +-- results/
|           +-- BENCHMARK_RESULTS.md
|           +-- benchmark_default.csv
|           +-- benchmark_release.csv
|
+-- CMakeLists.txt
+-- README.md
+-- .gitignore
The documentation and CSV files shown above are the intended repository
layout. Keep the generated CSV reports in the repository only after
verifying and preserving the actual files.
Requirements
- C++17-compatible compiler.
- CMake 3.20 or newer.
- Git (optional, for cloning and version control).
- Internet access during the first test-enabled CMake configure if
  GoogleTest must be downloaded by CMake FetchContent.
The project has been built using GCC with the MinGW Makefiles generator
on Windows. The CMake configuration also contains compiler-warning
options for MSVC and GNU/Clang compilers.
Build
Run the following commands from the project root.
1. Configure
cmake -S . -B build -G "MinGW Makefiles"
The project options default to: - Tests: ON - Examples: ON - Benchmarks:
OFF

To enable the benchmark target:
cmake -S . -B build -G "MinGW Makefiles" -DCONFIG_PARSER_BUILD_BENCHMARKS=ON
2. Build
cmake --build build
3. Run tests
ctest --test-dir build --output-on-failure
The test-enabled configuration uses CMake FetchContent to obtain
GoogleTest when it is not already available.
Quick Start
The following example illustrates the public API. It assumes the project
has been built and the config_parser library is linked to the example
or application target.
#include "config_parser/config.hpp"

#include <iostream>
#include <exception>

int main()
{
    try
    {
        config_parser::Config config;

        config.addSection("server");
        config.addValue("server", "host", "localhost");
        config.addValue("server", "port", "8080");
        config.addValue("server", "enabled", "true");

        const std::string host =
            config.getValue("server", "host");

        const int port =
            config.getInt("server", "port");

        const bool enabled =
            config.getBool("server", "enabled");

        std::cout << "Host: " << host << '\n';
        std::cout << "Port: " << port << '\n';
        std::cout << "Enabled: "
                  << (enabled ? "true" : "false") << '\n';
    }
    catch (const std::exception& error)
    {
        std::cerr << "Configuration error: "
                  << error.what() << '\n';
        return 1;
    }

    return 0;
}
Configuration File Format
The parser supports an INI-style section and key-value format.

Example config.ini:
# Server configuration
[server]
host = localhost
port = 8080
enabled = true

[database]
name = application_db
connection = user=admin;mode=fast
optional_value =
Load the file:
#include "config_parser/config.hpp"

int main()
{
    config_parser::Config config;
    config.loadFromFile("config.ini");

    const std::string host =
        config.getValue("server", "host");

    const int port =
        config.getInt("server", "port");

    const bool enabled =
        config.getBool("server", "enabled");
}
Public API
  Method                                        Description
  addSection(sectionName)                     Adds a section if it does not
already exist.

  addValue(sectionName, keyName, valueName)   Inserts or updates a value. Creates
the section if it is missing.

  getValue(sectionName, keyName)              Returns a value as std::string.
Throws if the section or key is
missing.

  getInt(sectionName, keyName)                Converts a value to int; rejects
invalid, partially numeric, and
out-of-range values.

  getBool(sectionName, keyName)               Converts true, false, 1, or
0 to bool; text matching is
case-insensitive.

  hasSection(sectionName)                     Checks whether a section exists.

  hasKey(sectionName, keyName)                Checks whether a key exists in a
section.
loadFromFile(filePath)                      Loads a file and replaces the
current configuration only after
successful parsing.
Typed getter behavior
getInt() accepts surrounding whitespace and validates that the
complete value was converted.

getBool() accepts: - true, TRUE, and other letter-case
variations. - false, FALSE, and other letter-case variations. - 1
and 0.

Other Boolean strings cause an exception.
Parsing Rules
  Input condition                                          Current behavior
  Blank or whitespace-only line                            Ignored
Full-line comment beginning with #                     Ignored
# inside a value                                       Preserved
Additional = characters in a value                     Preserved
Empty value                                              Allowed
Repeated section declaration                             Merged with the existing section
Duplicate key in the same section while loading a file   Rejected
Same key name in different sections                      Allowed
Key-value line before a section                          Rejected
Missing =                                              Rejected
Empty key                                                Rejected
Empty or malformed section declaration                   Rejected
UTF-8 BOM at beginning of file                           Handled
CRLF line endings                                        Handled

Inline comments, quoted-string escaping, nested sections, arrays, and a
formal guarantee of compatibility with every INI dialect are not
currently implemented.
Error Handling
The parser throws std::runtime_error for invalid operations and
malformed input, including: - Unable to open or read a file. - Missing
section or key. - Invalid integer or Boolean value. - Malformed section
declaration. - Missing =. - Key-value declaration before a section. -
Empty key. - Duplicate key in a section.

File parsing errors include the file path and line number to help locate
the problem.
Transactional File Loading
loadFromFile() parses into a temporary Config object. The new data
is moved into the current object only after the entire file has been
parsed successfully.

This means: - If parsing fails, the existing configuration remains
unchanged. - If parsing succeeds, the new configuration replaces the
existing configuration.

This behavior is covered by integration tests.
Testing
The project uses: - GoogleTest for test cases. - CTest for test
discovery and execution through CMake.

Run the complete test suite:
ctest --test-dir build --output-on-failure
Verified test result
The latest recorded test run reported:

  Metric                   Result
  Total tests                  58
Passed                       58
Failed                        0
Execution time     0.85 seconds

These results describe the recorded run. Re-run the test command after
changing the code to verify the current checkout.

See [`docs/testing/TESTING.md`](docs/testing/TESTING.md) for test
organization, strategy, and details.
Benchmarking
Enable the benchmark target during configuration:
cmake -S . -B build -G "MinGW Makefiles" -DCONFIG_PARSER_BUILD_BENCHMARKS=ON
cmake --build build
Run the Default benchmark:
./build/config_parser_benchmark.exe
Configure and build a Release benchmark:
cmake -S . -B build-release -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCONFIG_PARSER_BUILD_BENCHMARKS=ON -DCONFIG_PARSER_BUILD_TESTS=OFF -DCONFIG_PARSER_BUILD_EXAMPLES=OFF
cmake --build build-release
Run it:
./build-release/config_parser_benchmark.exe
The benchmark measures configuration loading and 100,000 lookup
operations per measurement run for workloads containing 100, 1,000,
10,000, and 100,000 entries. It performs 2 warm-up runs and 7
measurement runs.
Recorded average results
     Entries        Default        Release Default lookup Release lookup
               loading (ms)   loading (ms)           (ms)           (ms)

         100       0.233671         0.0815        38.2755         2.1735

       1,000        1.67691       0.366643        37.8466         3.0157

      10,000        16.0342        2.73677        40.7188        3.72656

     100,000        187.715        36.3182        57.2508        10.4238

The Release build recorded lower average loading and lookup times in
these runs. Benchmark results depend on the machine, compiler, build
settings, and system load; they are not universal performance
guarantees.

The benchmark executable writes benchmark_results.csv in its working
directory. Preserve the Default and Release reports separately so a
later run does not overwrite an earlier report.

See: -
[`docs/benchmark/BENCHMARK_NOTES.md`](docs/benchmark/BENCHMARK_NOTES.md) -
[`docs/benchmark/results/BENCHMARK_RESULTS.md`](docs/benchmark/results/BENCHMARK_RESULTS.md)
Design Overview
The parser stores configuration data in a nested std::unordered_map:
std::unordered_map<
    std::string,
    std::unordered_map<std::string, std::string>
> sections;
The outer map stores section names. Each inner map stores the keys and
string values belonging to that section.

Average-case lookup in std::unordered_map is O(1), while worst-case
lookup can be O(n). The nested representation performs a section lookup
followed by a key lookup.

Other design concepts used include: - Encapsulation through the Config
class. - RAII-managed file streams and standard library containers. -
Exceptions for invalid input and failed operations. - Temporary-object
parsing for transactional loading. - Move assignment for committing
successfully parsed data.

See [`docs/design/DESIGN.md`](docs/design/DESIGN.md) for detailed
architecture and design decisions.
Documentation
  Document                                                Contents
  [Design](docs/design/DESIGN.md)                         Architecture, data structures,
parsing design, C++ concepts, and
trade-offs

  [Testing](docs/testing/TESTING.md)                      GoogleTest, CTest, test
organization, and verified test
results

  [Benchmark Notes](docs/benchmark/BENCHMARK_NOTES.md)    Benchmark methodology, metrics,
limitations, and interview revision
[Benchmark                                              Recorded Default and Release
  Results](docs/benchmark/results/BENCHMARK_RESULTS.md)   performance results
Current Limitations and Possible Improvements
The following are potential future enhancements, not current features: -
Quoted values and escape-sequence support. - Inline comments. -
Additional typed getters such as floating-point conversion. -
Default-value retrieval APIs. - Ordered section and key iteration. -
Configurable duplicate-key behavior. - More detailed exception types. -
Fuzz testing and broader cross-platform testing. - Additional benchmark
workloads and comparisons with other parsers.
Engineering Workflow
The project is built with CMake and tested with GoogleTest/CTest. GitHub
Actions CI automation is a planned next engineering step; do not
consider CI/CD configured until a workflow has been added to the
repository and verified by a successful GitHub Actions run.
License
No license has been specified in this repository yet. Add a license file
and update this section before distributing the project for reuse.