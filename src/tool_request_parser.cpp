#include "tool_request_parser.hpp"

#include <sstream>

std::optional<ToolRequest> ToolRequestParser::parse(
    const std::string& input) const {
    if (input.empty() || input.front() != '/') {
        return std::nullopt;
    }

    std::istringstream tokens(input.substr(1));

    ToolRequest request;
    tokens >> request.name;

    if (request.name.empty()) {
        return std::nullopt;
    }

    std::string argument;
    while (tokens >> argument) {
        request.arguments.push_back(argument);
    }

    return request;
}