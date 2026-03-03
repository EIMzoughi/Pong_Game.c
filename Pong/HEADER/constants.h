#pragma once

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL2/SDL.h>

// Window and display constants
namespace Constants {
    // Screen dimensions
    constexpr int WINDOW_WIDTH = 840;
    constexpr int WINDOW_HEIGHT = 680;
    
    // Gameplay settings
    constexpr int PLAYER_SPEED = 10;
    constexpr double INITIAL_BALL_SPEED = 10.0;
    constexpr int PLAYER_WIDTH = 20;
    constexpr int PLAYER_HEIGHT = 100;
    constexpr int PLAYER_PADDING_FROM_SCREEN = 20;
    constexpr int BALL_SIZE = 20;
    
    // Physics constants
    constexpr double MAX_BOUNCE_ANGLE_DEGREES = 75.0;
    constexpr double PI = 3.14159265358979323846;
    
    // UI constants
    constexpr int SCORE_DISPLAY_Y = 20;
    constexpr int PLAYER1_SCORE_X = 200;
    constexpr int PLAYER2_SCORE_X = 400;
    constexpr int FONT_SIZE = 42;
    
    // Game constants
    constexpr int TARGET_FPS = 60;
    constexpr int FRAME_DELAY_MS = 1000 / TARGET_FPS;
    
    // Network constants
    constexpr int DEFAULT_PORT = 12345;
    constexpr int MAX_PACKET_SIZE = 1024;
    constexpr int NETWORK_TIMEOUT_MS = 5000;
    
    // Colors
    constexpr SDL_Color BACKGROUND_COLOR = {0, 0, 0, 255};
    constexpr SDL_Color PLAYER_COLOR = {255, 255, 255, 255};
    constexpr SDL_Color BALL_COLOR = {255, 255, 255, 255};
    constexpr SDL_Color TEXT_COLOR = {255, 255, 255, 255};
}

#endif // CONSTANTS_H
