#pragma once

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "config.h"

#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)>(b) ? (b) : (a))

void MoveBall(SDL_Rect* ball, Speed& speed);
void ResetGame();

#endif // GAME_LOGIC_H
