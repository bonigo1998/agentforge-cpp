#pragma once

#include <string>

class OllamaClient {
public:
    std::string chat(const std::string& task) const;
};