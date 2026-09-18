#pragma once

#include <string>
#include <vector>

struct ToolRequest {
    std::string name;
    std::vector<std::string> arguments;
};