#include "rendering.h"

void RenderText(const std::string& text, int x, int y)
{
    SDL_Color white = { 255, 255, 255, 255 }; // Text color
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), white);
    if (textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL)
    {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
    }
    else
    {
        SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    }

    // Clean up
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void ClearScreen()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void UpdateScreen()
{
    ClearScreen();

    SDL_SetRenderDrawColor(renderer, 225, 255, 255, 255);
    SDL_RenderFillRect(renderer, &player1);
    SDL_RenderFillRect(renderer, &player2);
    SDL_RenderFillRect(renderer, &ball);


    RenderText(std::to_string(score.player1), 200, 20);
    RenderText(std::to_string(score.player2), 400, 20);

    SDL_RenderPresent(renderer);
}