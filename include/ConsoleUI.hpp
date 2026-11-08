#pragma once

#include <filesystem>
#include <iosfwd>
#include <string>

class CampusLifeGame;

/** @brief Drives console menus using borrowed input and output streams. */
class ConsoleUI {
public:
    /**
     * @brief Binds streams for the lifetime of this UI.
     *
     * @param input Input stream; must outlive this UI.
     * @param output Output stream; must outlive this UI.
     * @param dataDirectory Directory for the sequential save and weekly log.
     */
    ConsoleUI(std::istream& input, std::ostream& output,
              std::filesystem::path dataDirectory = "data");

    /** @brief Shows the main menu until exit or end of input. */
    void run();

    /**
     * @brief Plays weeks until scholarship, semester end, exit, or end of input.
     *
     * The game's plan and student change only after a complete plan is confirmed.
     * @param game Game whose student and weekly plan are used.
     */
    void play(CampusLifeGame& game);

private:
    bool readInt(const std::string& prompt, int min, int max, int& value);
    bool readConfirmation(bool& confirmed);
    std::istream& input;
    std::ostream& output;
    std::filesystem::path dataDirectory;
};
