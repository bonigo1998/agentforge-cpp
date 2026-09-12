#include "agent.hpp"
#include "chat_message.hpp"
#include "ollama_client.hpp"

#include <cstddef>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

void Agent::run() {
    OllamaClient client;
    std::vector<ChatMessage> history;

    // Keep the last four completed exchanges.
    constexpr std::size_t max_history_messages = 8;

    std::cout << "AgentForge C++\n";
    std::cout << "Enter a task, '/clear' to clear history, "
                 "or 'exit' to quit.\n\n";

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

        if (task.find_first_not_of(" \t\r\n") == std::string::npos) {
            std::cout << "Please enter a non-empty task.\n\n";
            continue;
        }

        try {
            // Prepare the request using a copy of the saved history.
            auto next_history = history;
            next_history.push_back({"user", task});

            std::cout << "Contacting local model...\n" << std::flush;
            const std::string answer = client.chat(next_history);

            next_history.push_back({"assistant", answer});

            // Remove the oldest user-and-assistant pair.
            if (next_history.size() > max_history_messages) {
                next_history.erase(
                    next_history.begin(),
                    next_history.begin() + 2
                );
            }

            // Save the updated history after a successful response.
            history.swap(next_history);

            std::cout << "Agent: " << answer << "\n\n";
        } catch (const std::exception& error) {
            std::cerr << "Error: " << error.what() << "\n\n";
        }
    }

    std::cout << "Goodbye!\n";
}