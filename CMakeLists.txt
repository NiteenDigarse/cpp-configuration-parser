
cmake_minimum_required(VERSION 3.24)

project(ConfigurationParser
    VERSION 1.0.0
    DESCRIPTION "A reusable C++ configuration parser library"
    LANGUAGES CXX
)

# CMake Policies
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build Options
option(CONFIG_PARSER_BUILD_TESTS
    "Build unit and integration tests" ON
)

option(CONFIG_PARSER_BUILD_EXAMPLES
    "Build usage examples" ON
)

option(CONFIG_PARSER_BUILD_BENCHMARKS
    "Build performance benchmarks" OFF
)

# Compiler Warnings
function(config_parser_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target}
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
        )
    endif()
endfunction()

# Library Target
add_library(config_parser STATIC
    src/config.cpp
)

target_include_directories(config_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

config_parser_enable_warnings(config_parser)

# Example Target
if(CONFIG_PARSER_BUILD_EXAMPLES)

    add_executable(config_parser_example
        examples/main.cpp
    )

    target_link_libraries(config_parser_example
        PRIVATE config_parser
    )

    config_parser_enable_warnings(config_parser_example)

endif()

# Benchmark Target
if(CONFIG_PARSER_BUILD_BENCHMARKS)

    add_executable(config_parser_benchmark
        benchmarks/config_benchmark.cpp
    )

    target_link_libraries(config_parser_benchmark
        PRIVATE config_parser
    )

    config_parser_enable_warnings(config_parser_benchmark)

endif()


# GoogleTest Integration
if(CONFIG_PARSER_BUILD_TESTS)

    include(CTest)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)

endif()
