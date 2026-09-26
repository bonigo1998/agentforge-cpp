#pragma once

#include "chat_message.hpp"

#include <string>
#include <vector>

class ExecutionLogger;
class FileTool;
class OllamaClient;

class AgentLoop {
public:
    AgentLoop(
        const OllamaClient& client,
        const FileTool& file_tool,
        ExecutionLogger& logger);

    std::string run(
        const std::vector<ChatMessage>& history,
        const std::string& task) const;

private:
    const OllamaClient& client_;
    const FileTool& file_tool_;
    ExecutionLogger& logger_;
};