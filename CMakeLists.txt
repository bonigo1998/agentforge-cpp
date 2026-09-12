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
    src/ollama_client.cpp
src/file_tool.cpp
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
