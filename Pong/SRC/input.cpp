#include "..\HEADER\input.h"

void HandleInput()
{
    //handle players movement:
        // Get current keyboard state
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    // Player 1 Movement
    int movingPlayer1 = 0;
    if (keystate[SDL_SCANCODE_W] && player1.y > 0) movingPlayer1 = -1;
    if (keystate[SDL_SCANCODE_S] && player1.y < WINDOW_SCREEN_HEIGHT - player1.h) movingPlayer1 = 1;

    // Player 2 Movement
    int movingPlayer2 = 0;
    if (keystate[SDL_SCANCODE_UP] && player2.y > 0) movingPlayer2 = -1;
    if (keystate[SDL_SCANCODE_DOWN] && player2.y < WINDOW_SCREEN_HEIGHT - player2.h) movingPlayer2 = 1;

    //move players
    player1.y += movingPlayer1 * movementSpeed;
    player2.y += movingPlayer2 * movementSpeed;
}