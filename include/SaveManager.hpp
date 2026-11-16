#pragma once

#include <filesystem>

#include "PersistenceResult.hpp"

class CampusLifeGame;

/** @brief Stores and restores three independent fixed-size binary records. */
class SaveManager {
public:
    static constexpr int SLOT_COUNT{ 3 };
    /**
     * @brief Selects the save file without opening it.
     * @param path Path to the binary slot file.
     */
    explicit SaveManager(std::filesystem::path path);

    /**
     * @brief Replaces one slot with the current game state.
     *
     * Names must fit their fixed byte fields. A malformed file is not resized.
     * A failed write may leave the selected slot incomplete.
     * @param slot One-based slot number in 1..3.
     * @param game Game to save at the start of a week.
     * @return Operation outcome, including validation or file errors.
     */
    PersistenceResult save(int slot, const CampusLifeGame& game) const;

    /**
     * @brief Restores the selected record after validating it.
     *
     * The game and its current plan are unchanged on failure. A successful
     * load clears the plan. A missing file or empty slot returns NotFound.
     * @param slot One-based slot number in 1..3.
     * @param game Destination game; ownership remains with the caller.
     * @return Operation outcome, including a missing or malformed file.
     */
    PersistenceResult load(int slot, CampusLifeGame& game) const;

private:
    std::filesystem::path path;
};
