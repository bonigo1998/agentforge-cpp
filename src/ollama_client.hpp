#pragma once

#include <iostream>
#include "chat_message.hpp"
#include "model_response.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

class OllamaClient {
public:
    std::string chat(const std::vector<ChatMessage>& messages) const;

    ModelResponse chat_with_tools(
        const nlohmann::json& messages) const;
};