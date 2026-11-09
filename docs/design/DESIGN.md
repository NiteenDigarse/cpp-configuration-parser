Configuration Parser --- Design Documentation
Project: C++ Configuration Parser
Language: C++17
Build system: CMake
Testing: GoogleTest + CTest
Status: Core implementation and 58 automated tests verified

1. Project Overview
The Configuration Parser is a reusable C++ library that loads
configuration data from a text file and provides an API to access
sections, keys, and values.

The parser currently supports an INI-style structure:
[server]
host = localhost
port = 8080
enabled = true

[database]
name = application_db
The library stores the parsed configuration in memory. Callers can
retrieve values as strings or use typed getters for integers and Boolean
values.
1.1 Project goals
- Provide a reusable configuration parsing library.
- Organize values by section and key.
- Validate malformed configuration input.
- Report parsing errors with the file name and line number.
- Preserve the existing configuration if loading a new file fails.
- Provide automated tests and performance benchmarks.
1.2 Current scope
Implemented: - Sections and key-value pairs. - Whitespace trimming
around section names, keys, and values. - Blank-line and full-line #
comment handling. - Empty values. - Values containing additional =
characters. - Repeated section declarations merged into the same
section. - Duplicate keys rejected during file parsing. - UTF-8 BOM
handling at the beginning of a file. - CRLF input handling. - String,
integer, and Boolean retrieval. - Transactional file loading.

Not established as supported features: - Inline comments. -
Quoted-string escaping rules. - Nested sections. - Arrays or lists. -
Unicode normalization. - A formal INI compatibility guarantee.
2. Requirements
2.1 Functional requirements
  ID                      Requirement             Current behavior
  FR-01                   Add a section           addSection() creates
a section if absent

  FR-02                   Add or update a value   addValue() inserts or
replaces a key

  FR-03                   Retrieve a string       getValue() returns
the stored value

  FR-04                   Retrieve an integer     getInt() validates
integer conversion

  FR-05                   Retrieve a Boolean      getBool() accepts
true, false, 1,
and 0,
case-insensitively

  FR-06                   Check section existence hasSection() returns
a Boolean

  FR-07                   Check key existence     hasKey() returns a
Boolean

  FR-08                   Load from a file        loadFromFile() parses
a configuration file

  FR-09                   Report malformed input  Parsing exceptions
include file path and
line number
FR-10                   Preserve old state on   Failed file loading
failure                 does not commit partial
data
2.2 Non-functional goals
- Use standard C++17 facilities.
- Keep the public API small and understandable.
- Use RAII-managed standard library resources.
- Make parsing failures observable through exceptions.
- Keep the library buildable through CMake.
- Verify behavior with automated tests.
3. High-Level Architecture
The project separates the public interface, implementation, example
program, tests, and benchmarks.
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
|   +-- testing/
|   +-- benchmark/
|
+-- CMakeLists.txt
3.1 Responsibilities
  Component                           Responsibility
  config.hpp                        Public Config class declaration
and API

  config.cpp                        Storage operations, conversions,
parsing, and validation

  examples/main.cpp                 Demonstrates library usage

  tests/                            Unit and integration tests

  benchmarks/                       Measures loading and lookup
performance
CMakeLists.txt                    Defines build options and targets
The public header exposes the interface. The source file contains
implementation details such as trimming and parser error helpers.
4. Data Structure Design
The configuration is stored using a nested unordered map:
std::unordered_map<
    std::string,
    std::unordered_map<std::string, std::string>
> sections;
Conceptually:
sections
  |
  +-- "server"
  |     +-- "host"    -> "localhost"
  |     +-- "port"    -> "8080"
  |
  +-- "database"
        +-- "name"    -> "application_db"
4.1 Why a nested std::unordered_map?
The configuration naturally has two lookup levels:
1. Find a section by its name.
2. Find a key inside that section.
The outer map stores section names. Each outer-map value is another map
containing key-value pairs.
4.2 Expected lookup complexity
For std::unordered_map, lookup is average-case constant time, commonly
described as O(1). Its worst-case lookup complexity can be O(n)
due to hash collisions.

For a nested lookup, the average-case work involves two hash-map
lookups:
- One lookup for the section.
- One lookup for the key within that section.
Therefore, average-case lookup is expected to be O(1) with respect to
the number of sections and keys, while worst-case behavior can be linear
at either level.
4.3 Trade-offs
Advantages: - Convenient lookup by section and key. - Natural
representation of the file structure. - Standard library manages
container memory. - Insertion and update operations are straightforward.

Trade-offs: - Iteration order is not guaranteed to follow file order. -
Hash maps have memory and bucket overhead. - Worst-case lookup can
degrade when collisions occur. - The current API does not provide
ordered traversal.
5. Public API Design
The Config class provides the following operations.
  API                               Purpose                 Behavior
  addSection(name)                Add a section           Creates it if it does
not already exist

  addValue(section, key, value)   Store a value           Creates a missing
section and inserts or
updates the key

  getValue(section, key)          Read a string           Throws if section or
key is missing

  getInt(section, key)            Read an integer         Trims surrounding
whitespace and
validates full
conversion

  getBool(section, key)           Read a Boolean          Trims whitespace;
accepts true,
false, 1, 0

  hasSection(name)                Check a section         Returns true if found

  hasKey(section, key)            Check a key             Returns true if found
in that section
loadFromFile(path)              Parse a file            Replaces current
configuration only
after successful
parsing
5.1 Insert-or-update behavior
addValue() uses:
sections[sectionName].insert_or_assign(keyName, valueName);
The operator[] access creates the section if it does not exist.
insert_or_assign() inserts a new key or replaces the value associated
with an existing key.

This behavior is part of the current API contract and is covered by
tests.
5.2 Missing values
getValue() checks whether the section exists before checking the key.
It throws a std::runtime_error with a message identifying the missing
section or key.

hasSection() and hasKey() provide non-throwing existence checks.
6. Typed Value Conversion
Configuration values are stored as strings. Typed getters convert them
when requested.
6.1 Integer conversion
getInt(): 1. Retrieves the string value. 2. Trims surrounding
whitespace. 3. Calls std::stoi(). 4. Checks the number of consumed
characters. 5. Reports invalid input or range overflow.

Checking the consumed-character position is important. Without it, a
value such as 8080abc could be partially interpreted as an integer.

Expected examples:

  Stored value                  Result
  "8080"                      8080
" 8080 "                    8080
"-10"                       -10
"80abc"                     Error
Integer outside int range   Error
6.2 Boolean conversion
getBool() trims whitespace and converts characters to lowercase using
std::tolower() with an unsigned char argument.

Accepted values:

  Input     Result
  true    true
TRUE    true
1       true
false   false
FALSE   false
0       false

Other values cause an exception.
7. Parsing Design
The parser reads the file line by line using std::getline().
7.1 Parsing flow
Open file
   |
   v
Create temporary Config
   |
   v
Read next line
   |
   v
Handle first-line UTF-8 BOM
   |
   v
Trim surrounding whitespace
   |
   v
Blank line or full-line comment?
   | yes
   +------> Skip line
   |
   no
   v
Section declaration?
   | yes
   +------> Validate section
   |        Extract section name
   |        Add section
   |
   no
   v
Find '='
   |
   v
Validate current section and key
   |
   v
Extract key and value
   |
   v
Reject duplicate key
   |
   v
Insert value into temporary Config
   |
   v
More lines?
   | yes
   +------> Repeat
   |
   no
   v
Check file read error
   |
   v
Commit temporary configuration
7.2 Whitespace trimming
The internal trim() helper removes leading and trailing spaces, tabs,
carriage returns, and newlines.

It is used for: - Section names. - Keys. - Values. - Typed getter input.
7.3 Section validation
A section declaration must: - Have at least an opening and closing
bracket. - Start with [. - End with ]. - Not contain another [ or
] inside the section name. - Have a non-empty trimmed section name.

Examples:
[server]       ; valid
[[server]      ; invalid
[server]]      ; invalid
[]             ; invalid
[   ]          ; invalid after trimming
7.4 Key-value parsing
The parser locates the first = character.
const std::size_t equalPosition = line.find('=');
The text before the first = is the key. The remainder is the value.
This preserves additional equals signs inside values.

For example:
connection = user=admin;mode=fast
The stored value is:
user=admin;mode=fast
An empty value is allowed:
optional_value =
A missing = or an empty key is rejected.
7.5 Comments
The current parser ignores full-line comments beginning with # after
trimming.
# This is a comment
[server]
host = localhost
Inline comment removal is not implemented. A # occurring inside a
value is preserved.
7.6 Repeated sections and duplicate keys
Repeated section declarations are merged:
[server]
host = localhost

[server]
port = 8080
The server section contains both keys.

A duplicate key within the same section is rejected during file parsing.
The same key name can exist in different sections.
8. File Handling and RAII
The file is opened with:
std::ifstream file(filePath, std::ios::binary);
Binary mode allows the parser to inspect the initial bytes for a UTF-8
BOM and handle line endings consistently.

The file stream is an automatic local object. Its destructor closes the
file when it leaves scope. This is an example of RAII (Resource
Acquisition Is Initialization).

The implementation also uses standard library strings and containers,
which manage their own memory.
8.1 UTF-8 BOM handling
At the beginning of the first line, the parser checks for the UTF-8 BOM
byte sequence:
EF BB BF
If found, those three bytes are removed before parsing the line.

The BOM check is restricted to the beginning of the file.
8.2 CRLF handling
The trim() helper includes \r in its whitespace characters. This
allows trailing carriage returns from Windows CRLF line endings to be
removed.
9. Transactional File Loading
One of the important design decisions is that loadFromFile() does not
parse directly into the existing Config object.

Instead, it creates a temporary object:
Config temporaryConfig;
All parsed sections and values are inserted into this temporary
configuration.

Only after the complete file has been parsed successfully does the
implementation commit the new data:
sections = std::move(temporaryConfig.sections);
9.1 Why use a temporary object?
Consider an existing configuration:
server.port = 8080
Now suppose a new file contains valid data at the beginning but has a
malformed line near the end.

If parsing modified the existing object directly, the object could be
left partially updated when parsing failed.

With the temporary-object approach: - Existing state remains untouched
during parsing. - A parsing exception prevents the commit. - Successful
parsing replaces the old configuration.

This provides transactional behavior for file loading: either the
new configuration is committed after successful parsing, or the old
configuration remains in place when parsing fails.
9.2 Role of std::move
std::move converts the expression to an rvalue, allowing the map's
move-assignment operation to transfer resources where supported.

It does not itself move data. The selected move operation performs the
transfer.

The project tests both: - Failed loading preserves the old
configuration. - Successful loading replaces the old configuration.
10. Error Handling Strategy
The implementation uses std::runtime_error for: - Failure to open a
file. - Failure to read a file. - Missing sections and keys. - Invalid
integer or Boolean values. - Malformed section declarations. - Missing
=. - Key-value entries before a section. - Empty keys. - Duplicate
keys.

Parsing errors use an internal helper that includes the file path and
line number.

Example format:
settings.ini: Line 12: Empty key.
This makes configuration errors easier to locate.
11. Important C++ Concepts Used
  Concept                 Where it is used        Why it matters
  Classes and             Config                Groups configuration
encapsulation                                   data and operations

  std::unordered_map    Internal storage        Provides hash-based
lookup

  Nested containers       Section/key storage     Models two-level
configuration lookup

  const member          Read-only API           Expresses that getters
functions                                       do not modify the
object

  References              Function parameters     Avoids unnecessary
copies of input strings

  std::string           Parsing and storage     Manages dynamic text

  std::getline          File parsing            Reads input line by
line

  std::ifstream         File access             Provides RAII-managed
file handling

  std::move             Transactional commit    Enables move assignment
of stored data

  std::stoi             Integer getter          Converts text to an
integer

  Exceptions              Validation and errors   Reports invalid
operations and input

  std::transform        Boolean conversion      Normalizes case

  std::tolower          Boolean conversion      Converts characters to
lowercase

  try_emplace           Section insertion       Inserts a section only
when absent

  insert_or_assign      Value insertion         Inserts or updates a
key
std::size_t           Positions and line      Represents sizes and
numbers                 indexes
12. Design Decisions and Trade-offs
  Decision                Reason                  Trade-off
  Nested unordered maps   Direct section/key      No guaranteed insertion
lookup                  order

  Store values as strings Flexible configuration  Conversion is needed
representation          for typed access

  Return strings by value Simple value-returning  Copies the returned
from getValue()       API                     string

  Throw exceptions for    Errors are explicit     Callers must handle
invalid input                                   exceptions

  Parse line by line      Simple and bounded      Does not provide a
input processing        tokenized parse tree

  Temporary configuration Preserves old state on  Requires temporary
before commit           parse failure           storage while parsing

  Reject duplicate file   Avoids ambiguous        Does not support
keys                    configuration           last-value-wins file
semantics
Merge repeated sections Allows sections to be   Duplicate keys across
extended later in the   repeated sections are
file                    rejected
13. Current Limitations and Possible Future Improvements
These are possible future enhancements, not claims about current
functionality.
- Add configurable comment syntax or inline comments.
- Add quoted values and escape-sequence rules.
- Add getDouble() or other typed getters.
- Add optional default-value retrieval APIs.
- Add ordered iteration if preserving file order becomes a
  requirement.
- Add more detailed error types instead of using only
  std::runtime_error.
- Add configurable duplicate-key policies.
- Add support for arrays or lists.
- Add fuzz testing for malformed input.
- Add more benchmark scenarios and compare against other parser
  implementations.
Any future feature should be added with explicit requirements and tests.
14. Interview Revision Questions
Q1. Why did you use a nested std::unordered_map?
Because configuration data has two lookup levels: section and key. The
outer map stores sections and the inner map stores key-value pairs. This
gives average-case constant-time lookup at each level.
Q2. What is the average and worst-case lookup complexity of std::unordered_map?
Average-case lookup is O(1). Worst-case lookup is O(n), for example when
many keys collide into the same bucket.
Q3. What is RAII, and where is it used in this project?
RAII ties resource lifetime to object lifetime. The local
std::ifstream manages the file resource and closes it automatically
when the stream object is destroyed.
Q4. Why do you parse into a temporary Config object?
To avoid partially modifying the existing configuration if parsing
fails. The new data is committed only after the entire file has been
parsed successfully.
Q5. What does std::move do?
It casts an expression to an rvalue reference, enabling move operations
when the type supports them. It does not independently move the object's
data.
Q6. Why do you check the position returned by std::stoi()?
To ensure the entire input was consumed. This rejects partially valid
input such as 123abc.
Q7. Why is std::tolower() called with an unsigned char conversion?
Passing a negative char value other than EOF to std::tolower() can
cause undefined behavior. Converting to unsigned char avoids that
issue.
Q8. Why use insert_or_assign()?
It provides one operation that inserts a new key or replaces the value
of an existing key.
Q9. Why does the parser use binary file mode?
It allows inspection of the initial bytes for a UTF-8 BOM and makes
line-ending handling explicit.
Q10. What is the difference between unit and integration testing in this project?
Unit tests verify individual API behaviors. Integration tests verify
workflows such as loading configuration files and handling complete
parsing scenarios.
15. Build and Verification Commands
Configure:
cmake -S . -B build -G "MinGW Makefiles" -DCONFIG_PARSER_BUILD_BENCHMARKS=ON
Build:
cmake --build build
Run tests:
ctest --test-dir build --output-on-failure
Run benchmark:
.\build\config_parser_benchmark.exe
Release configuration:
cmake -S . -B build-release -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCONFIG_PARSER_BUILD_BENCHMARKS=ON -DCONFIG_PARSER_BUILD_TESTS=OFF -DCONFIG_PARSER_BUILD_EXAMPLES=OFF
Release build:
cmake --build build-release
Release benchmark:
.\build-release\config_parser_benchmark.exe
Verified project results
- Build completed successfully.
- 58 out of 58 automated tests passed.
- Default benchmark completed successfully.
- Release benchmark completed successfully.
- Default and Release CSV reports were generated.
The benchmark measurements are machine- and environment-dependent. See
docs/benchmark/BENCHMARK.md for the recorded results and methodology.
16. Documentation Maintenance
When implementation changes: 1. Update the relevant design section. 2.
Add or update tests for the changed behavior. 3. Update benchmark
documentation if performance behavior or methodology changes. 4. Keep
documented behavior consistent with the actual public API and
implementation.

Documentation rule: Do not describe a planned feature as
implemented. Keep current behavior, limitations, and future ideas
clearly separated.