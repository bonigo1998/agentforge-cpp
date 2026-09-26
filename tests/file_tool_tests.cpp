#include "file_tool.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <system_error>

namespace {

namespace fs = std::filesystem;

// Owns the test directory and restores the original working directory.
class TestWorkspace {
public:
    TestWorkspace() : original_directory_(fs::current_path()) {
        const fs::path base = fs::canonical(fs::temp_directory_path());
        std::random_device random;

        for (int attempt = 0; attempt < 100; ++attempt) {
            root_ = base / (
                "agentforge-tests-" + std::to_string(random()) +
                "-" + std::to_string(random())
            );

            if (fs::create_directory(root_)) {
                return;
            }
        }

        throw std::runtime_error("Could not create a test directory.");
    }

    TestWorkspace(const TestWorkspace&) = delete;
    TestWorkspace& operator=(const TestWorkspace&) = delete;

    ~TestWorkspace() {
        std::error_code error;
        fs::current_path(original_directory_, error);
        fs::remove_all(root_, error);
    }

    const fs::path& root() const {
        return root_;
    }

private:
    fs::path original_directory_;
    fs::path root_;
};

void write_text(const fs::path& path, const std::string& text) {
    std::ofstream output;
    output.exceptions(std::ios::failbit | std::ios::badbit);
    output.open(path);
    output << text;
    output.close();
}

void expect(bool passed, const char* description) {
    if (!passed) {
        throw std::runtime_error(description);
    }
}

void expect_rejected(
    const FileTool& tool,
    const std::string& path,
    const char* expected_error) {
    try {
        tool.read(path);
    } catch (const std::runtime_error& error) {
        if (std::string(error.what()).find(expected_error) !=
            std::string::npos) {
            return;
        }

        throw std::runtime_error(
            "Unexpected error for " + path + ": " + error.what());
    }

    throw std::runtime_error("Expected rejection for: " + path);
}

}  // namespace

int main() {
    try {
        TestWorkspace temporary;
        const fs::path workspace = temporary.root() / "workspace";
        const std::string contents = "AgentForge file test\n";

        fs::create_directories(workspace / "nested");

        write_text(workspace / "inside.txt", contents);
        write_text(temporary.root() / "outside.txt", "Outside file\n");

        fs::create_symlink(
            workspace / "inside.txt",
            workspace / "inside-link.txt");

        fs::create_symlink(
            temporary.root() / "outside.txt",
            workspace / "outside-link.txt");

        fs::current_path(workspace);
        const FileTool tool;

        expect(tool.read("inside.txt") == contents,
               "A workspace file should return its exact contents.");

        expect(tool.read("nested/../inside.txt") == contents,
               "A normalized path inside the workspace should work.");

        expect(tool.read("inside-link.txt") == contents,
               "A symlink staying inside the workspace should work.");

        expect_rejected(
            tool, "missing.txt", "Could not open file");

        expect_rejected(
            tool, (workspace / "inside.txt").string(),
            "must be relative");

        expect_rejected(
            tool, "../outside.txt", "outside the workspace");

        expect_rejected(
            tool, "outside-link.txt", "outside the workspace");

        std::cout << "All file-tool checks passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "File-tool test failed: " << error.what() << '\n';
        return 1;
    }
}