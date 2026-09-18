#include "agent_loop.hpp"

#include "file_tool.hpp"
#include "ollama_client.hpp"

#include <nlohmann/json.hpp>

#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

AgentLoop::AgentLoop(const OllamaClient& client, const FileTool& file_tool)
    : client_(client), file_tool_(file_tool) {}

std::string AgentLoop::run(
    const std::vector<ChatMessage>& history,
    const std::string& task) const {
    using nlohmann::json;

    json messages = json::array();

    for (const auto& message : history) {
        messages.push_back({
            {"role", message.role},
            {"content", message.content}
        });
    }

    messages.push_back({{"role", "user"}, {"content", task}});

    constexpr std::size_t max_model_steps = 3;
    constexpr std::size_t max_file_bytes = 4000;

    for (std::size_t step = 0; step < max_model_steps; ++step) {
        const ModelResponse response = client_.chat_with_tools(messages);

        if (response.tool_calls.empty()) {
            return response.content;
        }

        if (step + 1 == max_model_steps) {
            throw std::runtime_error(
                "Agent step limit reached before a final answer.");
        }

        if (response.tool_calls.size() != 1) {
            throw std::runtime_error(
                "Expected exactly one tool call.");
        }

        const json& call = response.tool_calls.at(0);

        if (!call.is_object() ||
            !call.contains("function") ||
            !call.at("function").is_object()) {
            throw std::runtime_error("Invalid tool call.");
        }

        const json& function = call.at("function");

        if (!function.contains("name") ||
            function.at("name") != "read_file" ||
            !function.contains("arguments") ||
            !function.at("arguments").is_object()) {
            throw std::runtime_error("Unsupported tool call.");
        }

        const json& arguments = function.at("arguments");

        if (arguments.size() != 1 ||
            !arguments.contains("path") ||
            !arguments.at("path").is_string()) {
            throw std::runtime_error("Invalid read_file arguments.");
        }

        const std::string path =
            arguments.at("path").get<std::string>();

        if (path.empty()) {
            throw std::runtime_error("read_file path is empty.");
        }

        messages.push_back({
            {"role", "assistant"},
            {"content", response.content},
            {"tool_calls", response.tool_calls}
        });

        std::cout << "Using read_file tool...\n" << std::flush;

        std::string tool_result;

        try {
            tool_result = file_tool_.read(path);

            if (tool_result.size() > max_file_bytes) {
                tool_result = "File is too large to send to the model.";
            }
        } catch (const std::exception& error) {
            tool_result = std::string("File error: ") + error.what();
        }

        messages.push_back({
            {"role", "tool"},
            {"tool_name", "read_file"},
            {"content", tool_result}
        });
    }

    throw std::runtime_error("Agent step limit reached.");
}