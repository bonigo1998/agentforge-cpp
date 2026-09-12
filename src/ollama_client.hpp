#pragma once

#include "chat_message.hpp"

#include <string>
#include <vector>

class OllamaClient {
public:
    std::string chat(const std::vector<ChatMessage>& messages) const;
};