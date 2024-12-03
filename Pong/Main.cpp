#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <algorithm>
#include <string>

 int WINDOW_SCREEN_WIDTH = 840;
 int WINDOW_SCREEN_HEIGHT = 680;
const char* FONT_NAME = "ARCADE_I.ttf";

static int movementSpeed = 10;//player speed
static int initBallSpeed = 10;//ball speed
static int playerWidth = 20;
static int playerHight = 100;
static int playersPaddingFromScreen = 20;



SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

SDL_Rect player1 = { playersPaddingFromScreen, (WINDOW_SCREEN_HEIGHT - playerHight) / 2, playerWidth, playerHight };
SDL_Rect player2 = { WINDOW_SCREEN_WIDTH - playerWidth - playersPaddingFromScreen , (WINDOW_SCREEN_HEIGHT - playerHight) / 2, playerWidth, playerHight };
SDL_Rect ball = { (WINDOW_SCREEN_WIDTH - 20) / 2,(WINDOW_SCREEN_HEIGHT - 20) / 2,20,20 };

typedef struct speed
{
    int x;
    int y;
}Speed;
Speed ballSpeed = { initBallSpeed,0 };//init ball speed

typedef struct score
{
    int player1;
    int player2;
} Score;
Score score = {0,0};


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
    if (TTF_Init() == -1)
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return 0;
    }
    font = TTF_OpenFont(FONT_NAME , 42);
    if (font == NULL)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return 0;
    }

    return 1;
}
// Function to render text as a texture
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
    

    RenderText(std::to_string(score.player1), 200, 20); 
    RenderText(std::to_string(score.player2), 400, 20); 

    SDL_RenderPresent(renderer);
}
void MovePlayer(SDL_Rect* rect, int speed)
{
    rect->y += speed; 
}

void ResetGame()
{
    ball.x = (WINDOW_SCREEN_WIDTH - ball.w) / 2;
    ball.y = (WINDOW_SCREEN_HEIGHT - ball.h) / 2;
    player1.y = player2.y = (WINDOW_SCREEN_HEIGHT - playerHight) / 2;
        
}


void MoveBall(SDL_Rect* ball, Speed& speed)
{
    // wall collision 
    if (ball->y <= 0 || ball->y + ball->h >= WINDOW_SCREEN_HEIGHT)
    {
        ballSpeed.y = -ballSpeed.y; // Reverse vertical direction
        ball->y = std::max(0, std::min(ball->y, WINDOW_SCREEN_HEIGHT - ball->h)); // Clamp position
    }

    // Paddle 1 collision (left side)
    if (ball->x <= player1.x + player1.w && ball->x > player1.x)
    {
        if (ball->y + ball->h >= player1.y && ball->y <= player1.y + player1.h)
        {
            // Reverse x direction
            ballSpeed.x = -ballSpeed.x;

            // reflecton angle
            double hitPoint = (ball->y + ball->h / 2.0) - (player1.y + player1.h / 2.0);
            double normalizedHitPoint = hitPoint / (player1.h / 2.0);
            
            // max bounce angle is 75 degree
            double maxAngle = 75.0 * M_PI / 180.0; // Convert to radians

            // Calculate new speed components
            double bounceAngle = normalizedHitPoint * maxAngle;
            double ballSpeedMagnitude = sqrt(ballSpeed.x * ballSpeed.x + ballSpeed.y * ballSpeed.y);

           
            ballSpeed.y = sin(bounceAngle) * ballSpeedMagnitude;

            // Ensure ball doesn't get stuck in the paddle
            ball->x = player1.x + player1.w + 1;
        }
        else
        {
            //reset game and add score
            score.player2++;
            ResetGame();
            ballSpeed.x = -initBallSpeed;
            ballSpeed.y = 0; // Reset vertical speed
        }
    }

    // Paddle 2 collision (right side)
    if (ball->x + ball->w >= player2.x && ball->x + ball->w < player2.x + player2.w)
    {
        if (ball->y + ball->h >= player2.y && ball->y <= player2.y + player2.h)
        {
            // Reverse horizontal direction
            ballSpeed.x = -ballSpeed.x;

            // reflecton angle
            double hitPoint = (ball->y + ball->h / 2.0) - (player2.y + player2.h / 2.0);
            double normalizedHitPoint = hitPoint / (player2.h / 2.0);

            // max bounce angle is 75 degree
            double maxAngle = 75.0 * M_PI / 180.0; // Convert to radians

            // Calculate new speed components
            double bounceAngle = normalizedHitPoint * maxAngle;
            double ballSpeedMagnitude = sqrt(ballSpeed.x * ballSpeed.x + ballSpeed.y * ballSpeed.y);

            
            ballSpeed.y = sin(bounceAngle) * ballSpeedMagnitude;

            // Ensure ball doesn't get stuck in the paddle
            ball->x = player2.x - ball->w - 1;
        }
        else
        {
            // reset game and add score
            score.player1++;
            ResetGame();
            ballSpeed.x = initBallSpeed;
            ballSpeed.y = 0; // Reset vertical speed
        }
    }

    // Update ball position
    ball->x += ballSpeed.x;
    ball->y += ballSpeed.y;
}
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
    if (movingPlayer1) MovePlayer(&player1, movingPlayer1 * movementSpeed);
    if (movingPlayer2) MovePlayer(&player2, movingPlayer2 * movementSpeed);
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
                       
        HandleInput();

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