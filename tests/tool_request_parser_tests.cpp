#include "tool_request_parser.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expect(bool passed, const char* description) {
    if (!passed) {
        throw std::runtime_error(description);
    }
}

}  // namespace

int main() {
    try {
        ToolRequestParser parser;

        expect(!parser.parse("Hello").has_value(),
               "Ordinary text should not become a command.");

        expect(!parser.parse("").has_value(),
               "Empty input should not become a command.");

        expect(!parser.parse("/").has_value(),
               "A slash alone should not become a command.");

        const auto read = parser.parse("/read README.md");
        expect(read &&
                   read->name == "read" &&
                   read->arguments ==
                       std::vector<std::string>{"README.md"},
               "/read should preserve the file path.");

        const auto ask = parser.parse(
            "/askfile\tREADME.md   What is this?  ");
        expect(ask &&
                   ask->name == "askfile" &&
                   ask->arguments == std::vector<std::string>{
                       "README.md", "What", "is", "this?"},
               "/askfile should split whitespace and preserve word order.");

        const auto clear = parser.parse("/clear");
        expect(clear &&
                   clear->name == "clear" &&
                   clear->arguments.empty(),
               "/clear should have no arguments.");

        const auto missing_path = parser.parse("/read");
        expect(missing_path &&
                   missing_path->name == "read" &&
                   missing_path->arguments.empty(),
               "Missing arguments should remain available for Agent validation.");

        const auto unknown = parser.parse("/unknown value");
        expect(unknown &&
                   unknown->name == "unknown" &&
                   unknown->arguments ==
                       std::vector<std::string>{"value"},
               "Unknown commands should remain available for Agent validation.");

        std::cout << "All parser checks passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Parser test failed: " << error.what() << '\n';
        return 1;
    }
}
