#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>

/** @brief Reports a game error that a console command can handle. */
class GameException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/** @brief Reports a failure to access a save or log file. */
class FileOpenException : public GameException {
public:
    /**
     * @brief Provides the failed operation and file name for the error message.
     *
     * @param path Path to the file whose access failed.
     * @param operation Description of the access that failed.
     */
    FileOpenException(const std::filesystem::path& path, const std::string& operation)
        : GameException{ operation + ": " + path.filename().string() } {
    }
};

/** @brief Reports a slot number outside the supported range. */
class InvalidSaveSlotException : public GameException {
public:
    /**
     * @brief Includes the rejected slot number in the error message.
     *
     * @param slot Requested one-based slot number.
     */
    explicit InvalidSaveSlotException(int slot)
        : GameException{ "invalid save slot: " + std::to_string(slot) } {
    }
};

/** @brief Reports a save file or record whose contents cannot be restored. */
class CorruptedSaveDataException : public GameException {
public:
    using GameException::GameException;
};
