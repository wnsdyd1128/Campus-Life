#include <cstdlib>
#include <exception>
#include <iostream>

#include "ConsoleUI.hpp"

int main() {
    try {
        ConsoleUI ui{ std::cin, std::cout };
        ui.run();
    } catch (const std::exception& error) {
        std::cerr << "Game stopped: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
    return 0;
}
