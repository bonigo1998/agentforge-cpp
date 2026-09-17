#include "file_tool.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {

bool is_inside_workspace(
    const std::filesystem::path& path,
    const std::filesystem::path& workspace_root) {
    std::error_code error;
    const std::filesystem::path relative =
        std::filesystem::relative(path, workspace_root, error);

    if (error) {
        return false;
    }

    for (const auto& part : relative) {
        if (part == "..") {
            return false;
        }
    }

    return true;
}

}  // namespace

FileTool::FileTool()
    : workspace_root_(std::filesystem::weakly_canonical(
          std::filesystem::current_path())) {}

std::string FileTool::read(const std::string& path) const {
    const std::filesystem::path requested_path(path);

    if (requested_path.is_absolute()) {
        throw std::runtime_error(
            "File paths must be relative to the workspace.");
    }

    std::error_code error;
    const std::filesystem::path resolved_path =
        std::filesystem::weakly_canonical(
            workspace_root_ / requested_path, error);

    if (error) {
        throw std::runtime_error(
            "Could not resolve file path: " + path);
    }

    if (!is_inside_workspace(resolved_path, workspace_root_)) {
        throw std::runtime_error(
            "File path is outside the workspace: " + path);
    }

    std::ifstream input(resolved_path);

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