#include "game.h"

void game()
{
    int playMode;
    printf("Play : [0] Local, [1] Online: ");

    do
    {
        scanf_s("%i", &playMode);
    } while (playMode < 0 || playMode > 2);

    switch (playMode)
    {
    case 0:
        localGameLoop();
        break;
    case 1:
        HandleOnlineLogic();
        break;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}