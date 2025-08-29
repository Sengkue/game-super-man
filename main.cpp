#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include "Game.h"

int main() {
    try {
        // Create game instance
        auto game = std::make_unique<Game>();
        
        // Initialize the game
        if (!game->initialize()) {
            std::cerr << "Failed to initialize game!" << std::endl;
            return -1;
        }
        
        // Run the main game loop
        game->run();
        
    } catch (const std::exception& e) {
        std::cerr << "Game error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}