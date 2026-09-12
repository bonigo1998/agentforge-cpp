#include "agent.hpp"
#include "ollama_client.hpp"

#include <exception>
#include <iostream>
#include <string>

void Agent::run() {
    OllamaClient client;

    std::cout << "AgentForge C++\n";
    std::cout << "Enter a task, or type 'exit' to quit.\n\n";

    while (true) {
        std::cout << "agentforge> ";

        std::string task;
        if (!std::getline(std::cin, task)) {
            std::cout << '\n';
            break;
        }

        if (task == "exit") {
            break;
        }

        if (task.find_first_not_of(" \t\r\n") == std::string::npos) {
            std::cout << "Please enter a non-empty task.\n\n";
            continue;
        }

        try {
            std::cout << "Contacting local model...\n" << std::flush;

            const std::string answer = client.chat(task);
            std::cout << "Agent: " << answer << "\n\n";
        } catch (const std::exception& error) {
            std::cerr << "Error: " << error.what() << "\n\n";
        }
    }

    std::cout << "Goodbye!\n";
}