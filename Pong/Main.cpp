#include <SDL.h>
#include <stdio.h>

const int WINDOW_SCREEN_WIDTH = 640;
const int WINDOW_SCREEN_HEIGHT = 480;

int movementSpeed = 10;//player speed
int playerWidth = 40;
int playerHight = 200;


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Rect player1 = { 20, (WINDOW_SCREEN_HEIGHT - playerHight) / 2, playerWidth, playerHight };
SDL_Rect player2 = { WINDOW_SCREEN_WIDTH - 60, (WINDOW_SCREEN_HEIGHT - playerHight) / 2, playerWidth, playerHight };
SDL_Rect ball = { (WINDOW_SCREEN_WIDTH - 20) / 2,(WINDOW_SCREEN_HEIGHT - 20) / 2,20,20 };

typedef struct speed
{
    int x;
    int y;
}Speed;

Speed ballSpeed = { 10,0 };//init ball speed

int Initisialize()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDLError: %s\n", SDL_GetError());
        return 0;
    }
    window = SDL_CreateWindow("Pong Game ElMzoughi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SCREEN_WIDTH, WINDOW_SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        printf("window could not be created%s", SDL_GetError());
        return 0;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("renderer could not be created SDLerror:%s", SDL_GetError());
        return 0;
    }
    return 1;
}

void ClearScreen()
{
    SDL_SetRenderDrawColor(renderer,0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void UpdateScreen()
{
    ClearScreen();
    SDL_SetRenderDrawColor(renderer, 225, 255, 255, 255);
    SDL_RenderFillRect(renderer, &player1);
    SDL_RenderFillRect(renderer, &player2);
    SDL_RenderFillRect(renderer, &ball);
    SDL_RenderPresent(renderer);
}
void MovePlayer(SDL_Rect* rect, int speed)
{
    rect->y += speed; 
}

void MoveBall(SDL_Rect* rect, Speed& speed)
{
    if (ball.x == player1.x + player1.w)
    {
        if((ball.y <= player1.y + player1.h)&&(ball.y+ball.h >= player1.y ))
        speed.x = -speed.x;
    }
    if (ball.x + ball.w == player2.x)
    {
        if ((ball.y <= player2.y + player2.h) && (ball.y + ball.h >= player2.y))
        speed.x = -speed.x;
    }

    ball.x += speed.x;
    ball.y += speed.y;
}

int main(int argc, char* argv[])
{
    if (!Initisialize())
    {
        return -1;
    }

    UpdateScreen();

    SDL_Rect Border = { WINDOW_SCREEN_WIDTH/2 ,0,1,WINDOW_SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &Border);

    

    SDL_Event event;
    int running = 1;
    int movingPlayer1 = 0, movingPlayer2 = 0;//act like bool and direction for player //0 player is not moving //1 moving down //-1 moving up
    while (running)
    {
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT: running = 0; break;
            }
        }

        //ball movement
        MoveBall(&ball, ballSpeed);
                       
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
        if (movingPlayer1) MovePlayer(&player1, movingPlayer1 * movementSpeed);
        if (movingPlayer2) MovePlayer(&player2, movingPlayer2 * movementSpeed);
        UpdateScreen();

        //+-60fps
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}