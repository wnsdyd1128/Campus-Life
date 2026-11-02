#include <iostream>

#include "ConsoleUI.hpp"

int main() {
    ConsoleUI ui{ std::cin, std::cout };
    ui.run();
    return 0;
}
