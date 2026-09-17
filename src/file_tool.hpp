#pragma once

#include <filesystem>
#include <string>

class FileTool {
public:
    FileTool();

    std::string read(const std::string& path) const;

private:
    std::filesystem::path workspace_root_;
};