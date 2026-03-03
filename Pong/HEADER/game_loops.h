#pragma once

#ifndef GAME_LOOPS_H
#define GAME_LOOPS_H

#include "config.h"
#include "init.h"
#include "input.h"
#include "rendering.h"
#include "game_logic.h"
#include "network.h"
#include <thread>
#include <atomic>

void serverGameLoop();
void clientGameLoop();
void localGameLoop();
void HandleOnlineLogic();

#endif // GAME_LOOPS_H
