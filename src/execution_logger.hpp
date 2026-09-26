#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <string>

class ExecutionLogger {
public:
    explicit ExecutionLogger(const std::filesystem::path& path);

    void log(
        const std::string& event,
        const nlohmann::json& details = nlohmann::json::object());

private:
    void disable();

    std::ofstream output_;
    bool enabled_ = true;
};