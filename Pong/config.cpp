#include "config.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

SDL_Rect player1 = { playersPaddingFromScreen, (WINDOW_SCREEN_HEIGHT - playerHeight) / 2, playerWidth, playerHeight };
SDL_Rect player2 = { WINDOW_SCREEN_WIDTH - playerWidth - playersPaddingFromScreen, (WINDOW_SCREEN_HEIGHT - playerHeight) / 2, playerWidth, playerHeight };
SDL_Rect ball = { (WINDOW_SCREEN_WIDTH - 20) / 2, (WINDOW_SCREEN_HEIGHT - 20) / 2, 20, 20 };

Speed ballSpeed = { initBallSpeed, 0 };
Score score = { 0, 0 };