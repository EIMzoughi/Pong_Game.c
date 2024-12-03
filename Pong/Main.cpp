#include "init.h"
#include "game_logic.h"
#include "input.h"
#include "rendering.h"

int main(int argc, char* argv[])
{
    if (!Initisialize())
    {
        return -1;
    }

    //SDL_Rect Border = { WINDOW_SCREEN_WIDTH/2 ,0,1,WINDOW_SCREEN_HEIGHT };
    //SDL_RenderFillRect(renderer, &Border);
   
    SDL_Event event;
    int running = 1;

    while (running)
    {
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT: running = 0; break;
            }
        }

        HandleInput();
        MoveBall(&ball, ballSpeed);
        UpdateScreen();

        //+-60fps
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}













////////////////////////////////////////////////////////////////////////////////////////////////////
/*void ResizeWindow(int width, int height)
{
    int newWidth = width;
    int newHeight = height;

    float widthRatio = (float)newWidth / WINDOW_SCREEN_WIDTH;
    float heightRatio = (float)newHeight / WINDOW_SCREEN_HEIGHT;

    // Update positions based on new size
    player1.x = playersPaddingFromScreen * widthRatio;
    player1.y = (player1.y * heightRatio);
    player1.w = player1.w * widthRatio;
    player1.h = player1.h * heightRatio;

    player2.x = newWidth - playerWidth * widthRatio - playersPaddingFromScreen * widthRatio;
    player2.y = (player2.y * heightRatio);
    player2.w = player2.w * widthRatio;
    player2.h = player2.h * heightRatio;

    ball.x = (newWidth - ball.x * widthRatio) ;
    ball.y = (newHeight - ball.y * heightRatio) ;
    ball.w *= widthRatio;
    ball.h *= heightRatio;

    ballSpeed.x *= widthRatio;
    ballSpeed.y *= heightRatio;

    // Update the old window size values
    WINDOW_SCREEN_WIDTH = newWidth;
    WINDOW_SCREEN_HEIGHT = newHeight;

    printf("Window resized to %dx%d\n", newWidth, newHeight);
}*/