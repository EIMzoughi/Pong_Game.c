#include "..\HEADER\game_loops.h"

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
    if (!InitisializeSDL()) { return; }

    SDL_Event event;
    int running = 1;


    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        HandleInput();
        MoveBall(&ball, ballSpeed);
        UpdateScreen();

        //+-60fps
        SDL_Delay(16);
    }
}


void HandleOnlineLogic()
{
    int mode;
    bool isServer = false;


    printf("Select mode: [1] Server, [2] Client: ");

    do
    {
        scanf_s("%d", &mode);
    } while (mode < 0 && mode >1);

    if (!initializeWinsock()) {
        return;
    }

    if (mode == 1)
    {
        isServer = true;
        if (!startServer(12345))
        {
            printf("failed to start server");
            cleanupWinsock();
            return;
        }
    }
    else
    {

        if (!connectToServer(12345))
        {
            printf("failed to connect to server");
            cleanupWinsock();
            return;
        }
    }

    if (!InitisializeSDL())
    {
        return;
    }

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
}
