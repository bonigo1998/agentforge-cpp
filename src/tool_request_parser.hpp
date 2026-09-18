#pragma once

#include "tool_request.hpp"

#include <optional>
#include <string>

class ToolRequestParser {
public:
    std::optional<ToolRequest> parse(const std::string& input) const;
};