#include <iostream>
#include <string>

int main() {
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

        std::cout << "Task received: " << task << '\n';
        std::cout << "AI integration is coming later.\n\n";
    }

    std::cout << "Goodbye!\n";
    return 0;
}