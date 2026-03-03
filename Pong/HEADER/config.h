#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>

// Screen dimensions
constexpr int WINDOW_SCREEN_WIDTH = 840;
constexpr int WINDOW_SCREEN_HEIGHT = 680;

// Gameplay settings
constexpr int movementSpeed = 10;
constexpr int initBallSpeed = 10;
constexpr int playerWidth = 20;
constexpr int playerHeight = 100;
constexpr int playersPaddingFromScreen = 20;
constexpr int ballWidth = 20;

// Frame timing
constexpr int TARGET_FPS = 60;
constexpr int frameDelay = 1000 / TARGET_FPS;

// Macros for min/max (used in game_logic.cpp)
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Structures
struct Speed {
    int x, y;
};

struct Score {
    int player1, player2;
};

// Global variables (defined in config.cpp)
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

extern SDL_Rect player1;
extern SDL_Rect player2;
extern SDL_Rect ball;
extern Speed ballSpeed;
extern Score score;

#endif // CONFIG_H
