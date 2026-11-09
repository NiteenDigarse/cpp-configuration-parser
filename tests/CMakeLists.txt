
add_executable(config_parser_tests
    unit/test_smoke.cpp
    unit/test_sections.cpp
    unit/test_values.cpp
    unit/test_typed_getters.cpp
    unit/test_validation.cpp
    unit/test_api_contract.cpp

    integration/test_file_loading.cpp
    integration/test_edge_cases.cpp
)

target_link_libraries(config_parser_tests
    PRIVATE
        config_parser
        GTest::gtest_main
)

if(MSVC)
    target_compile_options(config_parser_tests PRIVATE /W4)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(config_parser_tests
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

include(GoogleTest)

gtest_discover_tests(config_parser_tests)
