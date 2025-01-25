#pragma once

#ifndef RENDERING_H
#define RENDERING_H

#include "config.h"
#include <string>

void RenderText(const std::string& text, int x, int y);
void ClearScreen();
void UpdateScreen();

#endif // RENDERING_H