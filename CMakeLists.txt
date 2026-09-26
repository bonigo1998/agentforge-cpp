cmake_minimum_required(VERSION 3.20)

project(AgentForge VERSION 0.1.0 LANGUAGES CXX)

# Homebrew installs nlohmann-json outside CMake's default search path.
if(APPLE AND EXISTS "/opt/homebrew/opt/nlohmann-json")
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/nlohmann-json")
endif()

find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(agentforge
    src/main.cpp
    src/agent.cpp
    src/agent_loop.cpp
    src/ollama_client.cpp
    src/file_tool.cpp
    src/tool_request_parser.cpp
    src/execution_logger.cpp
)

set_target_properties(agentforge PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_compile_options(agentforge PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(agentforge PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
)

include(CTest)

if(BUILD_TESTING)
    add_executable(tool_request_parser_tests
        tests/tool_request_parser_tests.cpp
        src/tool_request_parser.cpp
    )

    target_include_directories(tool_request_parser_tests PRIVATE src)
    target_compile_features(tool_request_parser_tests PRIVATE cxx_std_20)
    set_target_properties(tool_request_parser_tests PROPERTIES
        CXX_EXTENSIONS OFF
    )

    add_test(
        NAME tool_request_parser
        COMMAND tool_request_parser_tests
    )

    add_executable(file_tool_tests
        tests/file_tool_tests.cpp
        src/file_tool.cpp
    )

    target_include_directories(file_tool_tests PRIVATE src)
    target_compile_features(file_tool_tests PRIVATE cxx_std_20)
    set_target_properties(file_tool_tests PROPERTIES
        CXX_EXTENSIONS OFF
    )

    add_test(
        NAME workspace_file_tool
        COMMAND file_tool_tests
    )
endif()