#pragma once

#include <atomic>
#include <chrono>
#include <filesystem>
#include <stdexcept>
#include <string>

/** A uniquely created test directory, removed with its contents on destruction. */
class TemporaryDirectory {
public:
    TemporaryDirectory() {
        static std::atomic<unsigned> sequence{ 0 };
        const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
        for (int attempt = 0; attempt < 100; ++attempt) {
            path_ = std::filesystem::temp_directory_path() /
                ("campus-life-test-" + std::to_string(ticks) + "-" +
                 std::to_string(sequence++));
            std::error_code error;
            if (std::filesystem::create_directory(path_, error)) return;
            if (error) throw std::filesystem::filesystem_error("create test directory", path_, error);
        }
        throw std::runtime_error("Unable to create unique test directory");
    }

    ~TemporaryDirectory() {
        std::error_code error;
        std::filesystem::remove_all(path_, error);
    }

    const std::filesystem::path& path() const { return path_; }

    TemporaryDirectory(const TemporaryDirectory&) = delete;
    TemporaryDirectory& operator=(const TemporaryDirectory&) = delete;

private:
    std::filesystem::path path_;
};
