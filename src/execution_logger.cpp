#include "execution_logger.hpp"

#include <chrono>
#include <exception>
#include <iostream>

ExecutionLogger::ExecutionLogger(const std::filesystem::path& path) {
    try {
        const auto parent = path.parent_path();

        if (!parent.empty()) {
            std::filesystem::create_directories(parent);
        }

        output_.open(path, std::ios::out | std::ios::app);

        if (!output_) {
            disable();
        }
    } catch (const std::exception&) {
        disable();
    }
}

void ExecutionLogger::log(
    const std::string& event,
    const nlohmann::json& details) {
    if (!enabled_) {
        return;
    }

    try {
        const auto now = std::chrono::system_clock::now();

        const auto timestamp_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()
            ).count();

        const nlohmann::json record = {
            {"timestamp_ms", timestamp_ms},
            {"event", event},
            {"details", details}
        };

        // Serialize the complete record before writing anything.
        const std::string line = record.dump() + '\n';

        output_ << line;
        output_.flush();

        if (!output_) {
            disable();
        }
    } catch (const std::exception&) {
        disable();
    }
}

void ExecutionLogger::disable() {
    if (!enabled_) {
        return;
    }

    enabled_ = false;
    std::cerr
        << "Warning: execution logging is unavailable for this session.\n";
}