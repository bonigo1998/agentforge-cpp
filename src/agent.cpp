#include "agent.hpp"
#include "chat_message.hpp"
#include "file_tool.hpp"
#include "ollama_client.hpp"

#include <cstddef>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

void Agent::run() {
    OllamaClient client;
    FileTool file_tool;
    std::vector<ChatMessage> history;

    constexpr std::size_t max_history_messages = 8;

    std::cout << "AgentForge C++\n";
    std::cout << "Enter a task, '/read <path>' to read a file, "
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

        if (task == "/clear") {
            history.clear();
            std::cout << "Conversation history cleared.\n\n";
            continue;
        }

        constexpr std::string_view read_prefix = "/read ";

    "'/read <path>' to read a file, "
"'/askfile <path> <question>' to ask about a file, "
        constexpr std::string_view ask_file_prefix = "/askfile ";

if (task.starts_with(ask_file_prefix)) {
    const std::string request = task.substr(ask_file_prefix.size());
    const std::size_t separator = request.find(' ');

    if (separator == std::string::npos) {
        std::cout << "Usage: /askfile <path> <question>\n\n";
        continue;
    }

    const std::string path = request.substr(0, separator);
    const std::string question = request.substr(separator + 1);

    if (path.empty() || question.empty()) {
        std::cout << "Usage: /askfile <path> <question>\n\n";
        continue;
    }

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

            try {
                std::cout << file_tool.read(path) << "\n\n";
            } catch (const std::exception& error) {
                std::cerr << "File error: " << error.what() << "\n\n";
            }

            continue;
        }

        if (task.find_first_not_of(" \t\r\n") == std::string::npos) {
            std::cout << "Please enter a non-empty task.\n\n";
            continue;
        }

        try {
            auto next_history = history;
            next_history.push_back({"user", task});

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
            std::cerr << "Error: " << error.what() << "\n\n";
        }
    }

    std::cout << "Goodbye!\n";
}