#include "..\HEADER\game.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    try {
        Game game;
        
        if (!game.Initialize()) {
            std::cerr << "Failed to initialize game!" << std::endl;
            return -1;
        }
        
        game.Run();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred!" << std::endl;
        return -1;
    }
    
    return 0;
} 