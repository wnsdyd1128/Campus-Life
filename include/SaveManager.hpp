#pragma once

#include <filesystem>

#include "PersistenceResult.hpp"

class CampusLifeGame;

/** @brief Stores and restores one game using sequential text records. */
class SaveManager {
public:
    /**
     * @brief Selects the save file without opening it.
     * @param path Path to the sequential save history.
     */
    explicit SaveManager(std::filesystem::path path);

    /**
     * @brief Appends the current game as a complete save record.
     *
     * A malformed existing history is not modified. A failed write can leave
     * an incomplete final record; prior records are not overwritten.
     * @param game Game to save at the start of a week.
     * @return Operation outcome, including validation or file errors.
     */
    PersistenceResult save(const CampusLifeGame& game) const;

    /**
     * @brief Restores the last record after validating the entire file.
     *
     * The game and its current plan are unchanged on failure. A successful
     * load clears the plan so the restored week can be planned afresh.
     * @param game Destination game; ownership remains with the caller.
     * @return Operation outcome, including a missing or malformed file.
     */
    PersistenceResult load(CampusLifeGame& game) const;

private:
    std::filesystem::path path;
};
