#include "init.h"
#include "game_logic.h"
#include "input.h"
#include "rendering.h"
#include "network.h"
#include "iostream"
#include "config.h"
#include "mutex"
#include "thread"


const int targetFPS = 60;
const int frameDelay = 1000 / targetFPS;

std::mutex mtx, m;

// Game State Structure
typedef struct GameState {
    int ballX, ballY;
    int ballSpeedX, ballSpeedY;
    int paddle1Y, paddle2Y;
    int score1, score2;
} GameState;

// Server Game Loop
void serverGameLoop() {
    GameState state = { ball.x, ball.y, ballSpeed.x, ballSpeed.y, player1.y, player2.y, score.player1, score.player2 };

    uint32_t frameStart;
    int frameTime;

    while (true) {
        frameStart = SDL_GetTicks();

        //handle players movement:
        // Get current keyboard state
        const Uint8* keystate = SDL_GetKeyboardState(NULL);

        // Player 1 Movement
        int movingPlayer1 = 0;
        if (keystate[SDL_SCANCODE_W] && player1.y > 0) movingPlayer1 = -1;
        if (keystate[SDL_SCANCODE_S] && player1.y < WINDOW_SCREEN_HEIGHT - player1.h) movingPlayer1 = 1;

        player1.y += movingPlayer1 * movementSpeed;

        // Update ball and check collisions
        //mtx.lock();
        MoveBall(&ball, ballSpeed);
        state.ballX = ball.x;
        state.ballY = ball.y;
        state.ballSpeedX = ballSpeed.x;
        state.ballSpeedY = ballSpeed.y;
        state.paddle1Y = player1.y;
        state.score1 = score.player1;
        state.score2 = score.player2;
        //mtx.unlock();
        // Send state to client
        sendData((char*)&state, sizeof(GameState));

        // Receive paddle2 position from client
        receiveData((char*)&state.paddle2Y, sizeof(int));
        player2.y = state.paddle2Y;

        UpdateScreen();

        frameTime = SDL_GetTicks() - frameStart;

        // If the frame was too quick, delay to maintain fixed FPS
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

// Client Game Loop
void clientGameLoop() {
    GameState state;

    uint32_t frameStart;
    int frametime;


    while (true) {

        frameStart = SDL_GetTicks();

        //handle players movement:
       // Get current keyboard state
        const Uint8* keystate = SDL_GetKeyboardState(NULL);

        // Player 2 Movement
        int movingPlayer2 = 0;
        if (keystate[SDL_SCANCODE_UP] && player2.y > 0) movingPlayer2 = -1;
        if (keystate[SDL_SCANCODE_DOWN] && player2.y < WINDOW_SCREEN_HEIGHT - player2.h) movingPlayer2 = 1;
        player2.y += movingPlayer2 * movementSpeed;


        sendData((char*)&player2.y, sizeof(int));

        // Receive game state from server
        receiveData((char*)&state, sizeof(GameState));

        //m.lock();
        // Update local state
        ball.x = state.ballX;
        ball.y = state.ballY;
        ballSpeed.x = state.ballSpeedX;
        ballSpeed.y = state.ballSpeedY;
        player1.y = state.paddle1Y;
        score.player1 = state.score1;
        score.player2 = state.score2;
        //m.unlock();
        UpdateScreen();

        frametime = SDL_GetTicks() - frameStart;

        // If the frame was too quick, delay to maintain fixed FPS
        if (frametime < frameDelay) {
            SDL_Delay(frameDelay - frametime);
        }
    
    }
}

void localGameLoop()
{
    SDL_Event event;
    int running = 1;

    while (running)
    {

        HandleInput();
        MoveBall(&ball, ballSpeed);
        UpdateScreen();

        //+-60fps
        SDL_Delay(16);
    }
}


int main(int argc, char* argv[])
{
    if (!initializeWinsock()) {
        return -1;
    }

    printf("Select mode: [1] Server, [2] Client: ");

    int mode;
    bool isServer = false;


    do
    {
        scanf_s("%d", &mode);
    } while (mode < 0 && mode >1);

    if (mode == 1)
    {
        isServer = true;
        if (!startServer(12345))
        {
            printf("failed to start server");
            cleanupWinsock();
            return -1;
        }
    }
    else
    {

        if (!connectToServer(12345))
        {
            printf("failed to connect to server");
            cleanupWinsock();
            return -1;
        }
    }





    if (!InitisializeSDL())
    {
        return -1;
    }

    //SDL_Rect Border = { WINDOW_SCREEN_WIDTH/2 ,0,1,WINDOW_SCREEN_HEIGHT };
    //SDL_RenderFillRect(renderer, &Border);

    if (isServer) {
        std::thread t1(serverGameLoop);
        t1.detach();
    }
    else {
        std::thread t2(clientGameLoop);
        t2.detach();
    }


    SDL_Event event;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
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