cmake_minimum_required(VERSION 3.20)

project(AgentForge VERSION 0.1.0 LANGUAGES CXX)

add_executable(agentforge src/main.cpp)

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