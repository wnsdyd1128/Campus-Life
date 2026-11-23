#pragma once

#include <filesystem>

#include "GameException.hpp"

class CampusLifeGame;

/** @brief Appends human-readable summaries of completed weeks. */
class LogManager {
public:
    /**
     * @brief Selects the log file without opening it.
     * @param path Destination for appended weekly results.
     */
    explicit LogManager(std::filesystem::path path);

    /**
     * @brief Appends the state after the completed week's weekend recovery.
     * @param completedWeek One-based week that was just simulated.
     * @param game Game containing the resulting student and burnout flags.
     * @throws FileOpenException If the log cannot be accessed or written.
     */
    void append(int completedWeek, const CampusLifeGame& game) const;

private:
    std::filesystem::path path;
};
