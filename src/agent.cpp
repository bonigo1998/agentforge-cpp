#include "agent_loop.hpp"
#include "agent.hpp"
#include "chat_message.hpp"
#include "file_tool.hpp"
#include "ollama_client.hpp"
#include "tool_request_parser.hpp"
#include "execution_logger.hpp"

#include <cstddef>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace {

std::string join_arguments(
    const std::vector<std::string>& arguments,
    std::size_t start_index) {
    std::string result;

    for (std::size_t index = start_index; index < arguments.size(); ++index) {
        if (!result.empty()) {
            result += ' ';
        }

        result += arguments[index];
    }

    return result;
}

}  // namespace

void Agent::run() {
    ExecutionLogger logger(".agentforge/logs/events.jsonl");
    logger.log("session_start");

    OllamaClient client;
    FileTool file_tool;
    AgentLoop loop(client, file_tool, logger);
    ToolRequestParser parser;
    std::vector<ChatMessage> history;

    constexpr std::size_t max_history_messages = 8;

    std::cout << "AgentForge C++\n";
    std::cout << "Enter a task, '/read <path>' to read a file, "
                 "'/askfile <path> <question>' to ask about a file, "
                 "'/clear' to clear history, or 'exit' to quit.\n\n";

    while (true) {
        std::cout << "agentforge> " << std::flush;

        std::string task;
        if (!std::getline(std::cin, task)) {
            std::cout << '\n';
            break;
        }

        if (task == "exit") {
            break;
        }

        const auto tool_request = parser.parse(task);

        if (tool_request.has_value()) {
            const std::string& command = tool_request->name;
            const auto& arguments = tool_request->arguments;

            if (command == "clear") {
                if (!arguments.empty()) {
                    std::cout << "Usage: /clear\n\n";
                } else {
                    history.clear();
                    std::cout << "Conversation history cleared.\n\n";
                }

                continue;
            }

            if (command == "read") {
                if (arguments.size() != 1) {
                    std::cout << "Usage: /read <path>\n\n";
                    continue;
                }

                try {
                    std::cout << file_tool.read(arguments[0]) << "\n\n";
                } catch (const std::exception& error) {
                    std::cerr << "File error: "
                              << error.what() << "\n\n";
                }

                continue;
            }

            if (command == "askfile") {
                if (arguments.size() < 2) {
                    std::cout << "Usage: /askfile <path> <question>\n\n";
                    continue;
                }

                const std::string& path = arguments[0];
                const std::string question = join_arguments(arguments, 1);

                try {
                    const std::string file_contents = file_tool.read(path);
                    const std::string model_task =
                        question + "\n\nFile contents from " + path + ":\n" +
                        file_contents;

                    auto next_history = history;
                    next_history.push_back({"user", model_task});

                    std::cout << "Contacting local model...\n" << std::flush;
                    const std::string answer = client.chat(next_history);
                    next_history.push_back({"assistant", answer});

                    if (next_history.size() > max_history_messages) {
                        next_history.erase(
                            next_history.begin(),
                            next_history.begin() + 2
                        );
                    }

                    history.swap(next_history);
                    std::cout << "Agent: " << answer << "\n\n";
                } catch (const std::exception& error) {
                    std::cerr << "File or model error: "
                              << error.what() << "\n\n";
                }

                continue;
            }

            std::cout << "Unknown command: /" << command << "\n\n";
            continue;
        }

        if (task.find_first_not_of(" \t\r\n") == std::string::npos) {
            std::cout << "Please enter a non-empty task.\n\n";
            continue;
        }

        try {
            auto next_history = history;
            next_history.push_back({"user", task});

            const std::string answer = loop.run(history, task);

            next_history.push_back({"assistant", answer});

            if (next_history.size() > max_history_messages) {
                next_history.erase(
                    next_history.begin(),
                    next_history.begin() + 2
                );
            }

            history.swap(next_history);
            std::cout << "Agent: " << answer << "\n\n";
        } catch (const std::exception& error) {
            std::cerr << "Error: " << error.what() << "\n\n";
        }
    }

    logger.log("session_end");
    std::cout << "Goodbye!\n";
}