#pragma once

#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>

// Forward declaration
class NetworkManager;

class Game {
public:
    Game();
    ~Game();
    
    // Main game functions
    bool Initialize();
    void Run();
    void Cleanup();
    
    // Game state management
    bool IsRunning() const { return m_isRunning; }
    void Quit() { m_isRunning = false; }
    
    // Game modes
    void RunLocalGame();
    void RunNetworkGame();
    
private:
    // Core SDL objects
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    TTF_Font* m_font;
    
    // Game state
    bool m_isRunning;
    bool m_isNetworkMode;
    
    // Game objects
    struct GameObjects {
        SDL_Rect player1;
        SDL_Rect player2;
        SDL_Rect ball;
        struct {
            double x, y;
        } ballSpeed;
        struct {
            int player1, player2;
        } score;
    } m_gameObjects;
    
    // Game settings
    struct GameSettings {
        int windowWidth;
        int windowHeight;
        int playerSpeed;
        double ballSpeed;
        int playerWidth;
        int playerHeight;
        int ballSize;
        int playerPadding;
        int targetFPS;
    } m_settings;
    
    // Private methods
    bool InitializeSDL();
    bool InitializeFont();
    void InitializeGameObjects();
    void HandleEvents();
    void Update();
    void Render();
    void ResetGame();
    
    // Input handling
    void HandleInput();
    
    // Physics
    void UpdateBallPhysics();
    bool CheckCollisions();
    void HandlePaddleCollision(const SDL_Rect& paddle, bool isLeftPaddle);
    
    // Rendering
    void RenderGameObjects();
    void RenderUI();
    void RenderText(const std::string& text, int x, int y, SDL_Color color = {255, 255, 255, 255});
    
    // Network game methods

    void RunServerMode(NetworkManager& networkManager);
    void RunClientMode(NetworkManager& networkManager);
};

#endif // GAME_H
