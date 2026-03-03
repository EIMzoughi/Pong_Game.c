#pragma once

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "config.h"

void ResetGame();
void MoveBall(SDL_Rect* ball, Speed& speed);

#endif // GAME_LOGIC_H
