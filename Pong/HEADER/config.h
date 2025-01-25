#pragma once

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL.h>
#include <SDL_ttf.h>

// screen dimensions
const int WINDOW_SCREEN_WIDTH = 840;
const int WINDOW_SCREEN_HEIGHT = 680;

// gameplay settings
const int movementSpeed = 10;     // player speed
const int initBallSpeed = 10;     // ball speed
const int playerWidth = 20;
const int playerHeight = 100;
const int playersPaddingFromScreen = 20;
const int ballWidth = 20;

// Global variables
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

extern SDL_Rect player1;
extern SDL_Rect player2;
extern SDL_Rect ball;

// ball speed
typedef struct Speed {
    int x;
    int y;
} Speed;
extern Speed ballSpeed;

// score structure
typedef struct Score {
    int player1;
    int player2;
} Score;
extern Score score;

#endif // CONSTANTS_H


