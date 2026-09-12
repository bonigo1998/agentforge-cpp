#include "file_tool.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

std::string FileTool::read(const std::string& path) const {
    std::ifstream input(path);

    if (!input) {
        throw std::runtime_error("Could not open file: " + path);
    }

    std::ostringstream contents;
    contents << input.rdbuf();

    if (input.bad()) {
        throw std::runtime_error("Could not read file: " + path);
    }

    return contents.str();
}