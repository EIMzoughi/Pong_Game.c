#include "..\HEADER\game.h"
#include "..\HEADER\constants.h"
#include "..\HEADER\network_manager.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <thread>
#include <chrono>

Game::Game() 
    : m_window(nullptr)
    , m_renderer(nullptr)
    , m_font(nullptr)
    , m_isRunning(false)
    , m_isNetworkMode(false)
{
    // Initialize game settings
    m_settings.windowWidth = Constants::WINDOW_WIDTH;
    m_settings.windowHeight = Constants::WINDOW_HEIGHT;
    m_settings.playerSpeed = Constants::PLAYER_SPEED;
    m_settings.ballSpeed = Constants::INITIAL_BALL_SPEED;
    m_settings.playerWidth = Constants::PLAYER_WIDTH;
    m_settings.playerHeight = Constants::PLAYER_HEIGHT;
    m_settings.ballSize = Constants::BALL_SIZE;
    m_settings.playerPadding = Constants::PLAYER_PADDING_FROM_SCREEN;
    m_settings.targetFPS = Constants::TARGET_FPS;
    
    // Initialize game objects
    InitializeGameObjects();
}

Game::~Game() {
    Cleanup();
}

bool Game::Initialize() {
    if (!InitializeSDL()) {
        std::cerr << "Failed to initialize SDL" << std::endl;
        return false;
    }
    
    if (!InitializeFont()) {
        std::cerr << "Failed to initialize font" << std::endl;
        return false;
    }
    
    m_isRunning = true;
    return true;
}

void Game::Run() {
    if (!m_isRunning) return;
    
    // Game mode selection
    int playMode = 0;
    std::cout << "Play: [0] Local, [1] Online: ";
    std::cin >> playMode;
    
    if (playMode == 0) {
        RunLocalGame();
    } else if (playMode == 1) {
        RunNetworkGame();
    }
}

void Game::Cleanup() {
    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
    
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    
    TTF_Quit();
    SDL_Quit();
}

bool Game::InitializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    m_window = SDL_CreateWindow(
        "Pong Game - Refactored", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        m_settings.windowWidth, 
        m_settings.windowHeight, 
        0
    );
    
    if (!m_window) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    return true;
}

bool Game::InitializeFont() {
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }
    
    m_font = TTF_OpenFont("FONT/ARCADE_I.TTF", Constants::FONT_SIZE);
    if (!m_font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }
    
    return true;
}

void Game::InitializeGameObjects() {
    // Initialize players
    m_gameObjects.player1 = {
        m_settings.playerPadding,
        (m_settings.windowHeight - m_settings.playerHeight) / 2,
        m_settings.playerWidth,
        m_settings.playerHeight
    };
    
    m_gameObjects.player2 = {
        m_settings.windowWidth - m_settings.playerWidth - m_settings.playerPadding,
        (m_settings.windowHeight - m_settings.playerHeight) / 2,
        m_settings.playerWidth,
        m_settings.playerHeight
    };
    
    // Initialize ball
    m_gameObjects.ball = {
        (m_settings.windowWidth - m_settings.ballSize) / 2,
        (m_settings.windowHeight - m_settings.ballSize) / 2,
        m_settings.ballSize,
        m_settings.ballSize
    };
    
    // Initialize ball speed
    m_gameObjects.ballSpeed.x = static_cast<double>(m_settings.ballSpeed);
    m_gameObjects.ballSpeed.y = 0.0;
    
    // Initialize score
    m_gameObjects.score.player1 = 0;
    m_gameObjects.score.player2 = 0;
}

void Game::RunLocalGame() {
    
    uint32_t frameStart;
    int frameTime;
    
    while (m_isRunning) {
        frameStart = SDL_GetTicks();
        
        HandleEvents();
        HandleInput();
        Update();
        Render();
        
        // Frame rate limiting
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < Constants::FRAME_DELAY_MS) {
            SDL_Delay(Constants::FRAME_DELAY_MS - frameTime);
        }
    }
}

void Game::RunNetworkGame() {
    m_isNetworkMode = true;
    
    int mode = 0;
    std::cout << "Select mode: [1] Server, [2] Client: ";
    std::cin >> mode;
    
    NetworkManager networkManager;
    if (!networkManager.Initialize()) {
        std::cerr << "Failed to initialize network: " << networkManager.GetLastError() << std::endl;
        return;
    }
    
    if (mode == 1) {
        // Server mode
        if (!networkManager.StartServer(Constants::DEFAULT_PORT)) {
            std::cerr << "Failed to start server: " << networkManager.GetLastError() << std::endl;
            return;
        }
        
        if (!networkManager.WaitForClient()) {
            std::cerr << "Failed to accept client: " << networkManager.GetLastError() << std::endl;
            return;
        }
        
        RunServerMode(networkManager);
    } else if (mode == 2) {
        // Client mode
        std::string serverIP;
        std::cout << "Enter server IP address: ";
        std::cin >> serverIP;
        
        if (!networkManager.ConnectToServer(serverIP, Constants::DEFAULT_PORT)) {
            std::cerr << "Failed to connect to server: " << networkManager.GetLastError() << std::endl;
            return;
        }
        
        RunClientMode(networkManager);
    }
}



void Game::RunServerMode(NetworkManager& networkManager) {
    uint32_t frameStart;
    int frameTime;
    
    while (m_isRunning && networkManager.IsConnected()) {
        frameStart = SDL_GetTicks();
        
        HandleEvents();
        HandleInput();
        UpdateBallPhysics();
        
        // Send game state to client
        NetworkGameState state;
        state.ballX = m_gameObjects.ball.x;
        state.ballY = m_gameObjects.ball.y;
        state.ballSpeedX = m_gameObjects.ballSpeed.x;
        state.ballSpeedY = m_gameObjects.ballSpeed.y;
        state.paddle1Y = m_gameObjects.player1.y;
        state.score1 = m_gameObjects.score.player1;
        state.score2 = m_gameObjects.score.player2;
        
        if (!networkManager.SendGameState(state)) {
            std::cerr << "Failed to send game state" << std::endl;
            break;
        }
        
        // Receive client paddle position
        int clientPaddleY;
        if (networkManager.ReceivePaddlePosition(clientPaddleY)) {
            m_gameObjects.player2.y = clientPaddleY;
        }
        
        Render();
        
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < Constants::FRAME_DELAY_MS) {
            SDL_Delay(Constants::FRAME_DELAY_MS - frameTime);
        }
    }
}

void Game::RunClientMode(NetworkManager& networkManager) {
    uint32_t frameStart;
    int frameTime;
    
    while (m_isRunning && networkManager.IsConnected()) {
        frameStart = SDL_GetTicks();
        
        HandleEvents();
        HandleInput();
        
        // Send paddle position to server
        if (!networkManager.SendPaddlePosition(m_gameObjects.player2.y)) {
            std::cerr << "Failed to send paddle position" << std::endl;
            break;
        }
        
        // Receive game state from server
        NetworkGameState state;
        if (networkManager.ReceiveGameState(state)) {
            m_gameObjects.ball.x = state.ballX;
            m_gameObjects.ball.y = state.ballY;
            m_gameObjects.ballSpeed.x = state.ballSpeedX;
            m_gameObjects.ballSpeed.y = state.ballSpeedY;
            m_gameObjects.player1.y = state.paddle1Y;
            m_gameObjects.score.player1 = state.score1;
            m_gameObjects.score.player2 = state.score2;
        }
        
        Render();
        
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < Constants::FRAME_DELAY_MS) {
            SDL_Delay(Constants::FRAME_DELAY_MS - frameTime);
        }
    }
}

void Game::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                m_isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    m_isRunning = false;
                }
                break;
        }
    }
}

void Game::HandleInput() {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    
    // Player 1 movement (W/S keys)
    if (keystate[SDL_SCANCODE_W] && m_gameObjects.player1.y > 0) {
        m_gameObjects.player1.y -= m_settings.playerSpeed;
    }
    if (keystate[SDL_SCANCODE_S] && m_gameObjects.player1.y < m_settings.windowHeight - m_settings.playerHeight) {
        m_gameObjects.player1.y += m_settings.playerSpeed;
    }
    
    // Player 2 movement (Arrow keys)
    if (keystate[SDL_SCANCODE_UP] && m_gameObjects.player2.y > 0) {
        m_gameObjects.player2.y -= m_settings.playerSpeed;
    }
    if (keystate[SDL_SCANCODE_DOWN] && m_gameObjects.player2.y < m_settings.windowHeight - m_settings.playerHeight) {
        m_gameObjects.player2.y += m_settings.playerSpeed;
    }
}

void Game::Update() {
    if (!m_isNetworkMode) {
        UpdateBallPhysics();
    }
}

void Game::UpdateBallPhysics() {
    // Update ball position
    m_gameObjects.ball.x += m_gameObjects.ballSpeed.x;
    m_gameObjects.ball.y += m_gameObjects.ballSpeed.y;
    
    // Check collisions
    CheckCollisions();
}

bool Game::CheckCollisions() {
    // Wall collisions (top and bottom)
    if (m_gameObjects.ball.y <= 0 || m_gameObjects.ball.y + m_gameObjects.ball.h >= m_settings.windowHeight) {
        m_gameObjects.ballSpeed.y = -m_gameObjects.ballSpeed.y;
        m_gameObjects.ball.y = std::max(0, std::min(m_gameObjects.ball.y, m_settings.windowHeight - m_gameObjects.ball.h));
    }
    
    // Ball passed left edge — Player 2 scores
    if (m_gameObjects.ball.x + m_gameObjects.ball.w < 0) {
        m_gameObjects.score.player2++;
        ResetGame();
        m_gameObjects.ballSpeed.x = m_settings.ballSpeed;
        m_gameObjects.ballSpeed.y = 0.0;
        return true;
    }
    
    // Ball passed right edge — Player 1 scores
    if (m_gameObjects.ball.x > m_settings.windowWidth) {
        m_gameObjects.score.player1++;
        ResetGame();
        m_gameObjects.ballSpeed.x = -m_settings.ballSpeed;
        m_gameObjects.ballSpeed.y = 0.0;
        return true;
    }
    
    // Paddle 1 collision (left side)
    if (m_gameObjects.ball.x <= m_gameObjects.player1.x + m_gameObjects.player1.w && 
        m_gameObjects.ball.x > m_gameObjects.player1.x) {
        if (m_gameObjects.ball.y + m_gameObjects.ball.h >= m_gameObjects.player1.y && 
            m_gameObjects.ball.y <= m_gameObjects.player1.y + m_gameObjects.player1.h) {
            HandlePaddleCollision(m_gameObjects.player1, true);
        }
    }
    
    // Paddle 2 collision (right side)
    if (m_gameObjects.ball.x + m_gameObjects.ball.w >= m_gameObjects.player2.x && 
        m_gameObjects.ball.x + m_gameObjects.ball.w < m_gameObjects.player2.x + m_gameObjects.player2.w) {
        if (m_gameObjects.ball.y + m_gameObjects.ball.h >= m_gameObjects.player2.y && 
            m_gameObjects.ball.y <= m_gameObjects.player2.y + m_gameObjects.player2.h) {
            HandlePaddleCollision(m_gameObjects.player2, false);
        }
    }
    
    return true;
}

void Game::HandlePaddleCollision(const SDL_Rect& paddle, bool isLeftPaddle) {
    // Reverse horizontal direction
    m_gameObjects.ballSpeed.x = -m_gameObjects.ballSpeed.x;
    
    // Calculate reflection angle based on hit point
    double hitPoint = (m_gameObjects.ball.y + m_gameObjects.ball.h / 2.0) - (paddle.y + paddle.h / 2.0);
    double normalizedHitPoint = hitPoint / (paddle.h / 2.0);
    
    // Calculate bounce angle (max 75 degrees)
    double maxAngle = Constants::MAX_BOUNCE_ANGLE_DEGREES * Constants::PI / 180.0;
    double bounceAngle = normalizedHitPoint * maxAngle;
    
    // Calculate new speed components
    double ballSpeedMagnitude = sqrt(m_gameObjects.ballSpeed.x * m_gameObjects.ballSpeed.x + 
                                   m_gameObjects.ballSpeed.y * m_gameObjects.ballSpeed.y);
    
    m_gameObjects.ballSpeed.y = sin(bounceAngle) * ballSpeedMagnitude;
    
    // Ensure ball doesn't get stuck in paddle
    if (isLeftPaddle) {
        m_gameObjects.ball.x = paddle.x + paddle.w + 1;
    } else {
        m_gameObjects.ball.x = paddle.x - m_gameObjects.ball.w - 1;
    }
}

void Game::Render() {
    // Clear screen
    SDL_SetRenderDrawColor(m_renderer, 
                          Constants::BACKGROUND_COLOR.r, 
                          Constants::BACKGROUND_COLOR.g, 
                          Constants::BACKGROUND_COLOR.b, 
                          Constants::BACKGROUND_COLOR.a);
    SDL_RenderClear(m_renderer);
    
    // Render game objects
    RenderGameObjects();
    
    // Render UI
    RenderUI();
    
    // Present renderer
    SDL_RenderPresent(m_renderer);
}

void Game::RenderGameObjects() {
    // Render players
    SDL_SetRenderDrawColor(m_renderer, 
                          Constants::PLAYER_COLOR.r, 
                          Constants::PLAYER_COLOR.g, 
                          Constants::PLAYER_COLOR.b, 
                          Constants::PLAYER_COLOR.a);
    SDL_RenderFillRect(m_renderer, &m_gameObjects.player1);
    SDL_RenderFillRect(m_renderer, &m_gameObjects.player2);
    
    // Render ball
    SDL_SetRenderDrawColor(m_renderer, 
                          Constants::BALL_COLOR.r, 
                          Constants::BALL_COLOR.g, 
                          Constants::BALL_COLOR.b, 
                          Constants::BALL_COLOR.a);
    SDL_RenderFillRect(m_renderer, &m_gameObjects.ball);
}

void Game::RenderUI() {
    // Render scores
    RenderText(std::to_string(m_gameObjects.score.player1), 
               Constants::PLAYER1_SCORE_X, Constants::SCORE_DISPLAY_Y);
    RenderText(std::to_string(m_gameObjects.score.player2), 
               Constants::PLAYER2_SCORE_X, Constants::SCORE_DISPLAY_Y);
}

void Game::RenderText(const std::string& text, int x, int y, SDL_Color color) {
    if (!m_font) return;
    
    SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, text.c_str(), color);
    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }
    
    SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
    SDL_RenderCopy(m_renderer, textTexture, NULL, &renderQuad);
    
    // Clean up
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void Game::ResetGame() {
    // Reset ball position
    m_gameObjects.ball.x = (m_settings.windowWidth - m_settings.ballSize) / 2;
    m_gameObjects.ball.y = (m_settings.windowHeight - m_settings.ballSize) / 2;
    
    // Reset player positions
    m_gameObjects.player1.y = (m_settings.windowHeight - m_settings.playerHeight) / 2;
    m_gameObjects.player2.y = (m_settings.windowHeight - m_settings.playerHeight) / 2;
}