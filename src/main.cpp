#include <iostream>
#include <string>

int main()
{
std::cout << "AgentForge c++\n";

std::string task;
    if (!std::getline(std::cin, task)) {
        std::cerr << "Failed to read input.\n";
        return 1;
    }

    if (task.find_first_not_of(" \t\r\n") == std::string::npos) {
        std::cerr << "Please enter a non-empty task.\n";
        return 1;
    }

    std::cout << "Task received: " << task << '\n';
    std::cout << "AI integration is coming next.\n";

    return 0;
}