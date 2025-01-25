#pragma once

#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include "config.h"
#include "input.h"
#include "rendering.h"
#include "game_logic.h"
#include "network.h"
//#include "mutex"
#include "init.h"
#include "thread"

const int targetFPS = 60;
const int frameDelay = 1000 / targetFPS;

//std::mutex mtx, m;

void serverGameLoop();
void clientGameLoop();
void localGameLoop();
void HandleOnlineLogic();

#endif // !GAME_LOOP_H
