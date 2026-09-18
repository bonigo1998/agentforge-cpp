#pragma once

#include <nlohmann/json.hpp>

#include <string>

struct ModelResponse {
    std::string content;
    nlohmann::json tool_calls = nlohmann::json::array();
};