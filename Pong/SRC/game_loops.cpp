#include "..\HEADER\game_loops.h"
#include <atomic>

// Shared stop flag for network threads
static std::atomic<bool> g_running(true);

void serverGameLoop() {
    GameState state = { ball.x, ball.y, ballSpeed.x, ballSpeed.y, player1.y, player2.y, score.player1, score.player2 };

    uint32_t frameStart;
    int frameTime;

    while (g_running.load()) {
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
        MoveBall(&ball, ballSpeed);
        state.ballX = ball.x;
        state.ballY = ball.y;
        state.ballSpeedX = ballSpeed.x;
        state.ballSpeedY = ballSpeed.y;
        state.paddle1Y = player1.y;
        state.score1 = score.player1;
        state.score2 = score.player2;

        // Send state to client
        int bytesSent = send(connectionSocket, (char*)&state, sizeof(GameState), 0);
        if (bytesSent == SOCKET_ERROR) {
            printf("Failed to send game state: %d\n", WSAGetLastError());
            break;
        }

        // Receive paddle2 position from client
        int bytesRecv = recv(connectionSocket, (char*)&state.paddle2Y, sizeof(int), 0);
        if (bytesRecv == SOCKET_ERROR || bytesRecv == 0) {
            printf("Failed to receive paddle position: %d\n", WSAGetLastError());
            break;
        }
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
    int frameTime;

    while (g_running.load()) {

        frameStart = SDL_GetTicks();

        //handle players movement:
       // Get current keyboard state
        const Uint8* keystate = SDL_GetKeyboardState(NULL);

        // Player 2 Movement
        int movingPlayer2 = 0;
        if (keystate[SDL_SCANCODE_UP] && player2.y > 0) movingPlayer2 = -1;
        if (keystate[SDL_SCANCODE_DOWN] && player2.y < WINDOW_SCREEN_HEIGHT - player2.h) movingPlayer2 = 1;
        player2.y += movingPlayer2 * movementSpeed;

        // Send paddle position to server
        int bytesSent = send(clientSocket, (char*)&player2.y, sizeof(int), 0);
        if (bytesSent == SOCKET_ERROR) {
            printf("Failed to send paddle position: %d\n", WSAGetLastError());
            break;
        }

        // Receive game state from server
        int bytesRecv = recv(clientSocket, (char*)&state, sizeof(GameState), 0);
        if (bytesRecv == SOCKET_ERROR || bytesRecv == 0) {
            printf("Failed to receive game state: %d\n", WSAGetLastError());
            break;
        }

        // Update local state
        ball.x = state.ballX;
        ball.y = state.ballY;
        ballSpeed.x = state.ballSpeedX;
        ballSpeed.y = state.ballSpeedY;
        player1.y = state.paddle1Y;
        score.player1 = state.score1;
        score.player2 = state.score2;

        UpdateScreen();

        frameTime = SDL_GetTicks() - frameStart;

        // If the frame was too quick, delay to maintain fixed FPS
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void localGameLoop()
{
    if (!InitisializeSDL()) { return; }

    SDL_Event event;
    int running = 1;

    uint32_t frameStart;
    int frameTime;

    while (running) {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        HandleInput();
        MoveBall(&ball, ballSpeed);
        UpdateScreen();

        // Frame rate limiting
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }
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
    } while (mode < 1 || mode > 2);

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

    // Reset the stop flag
    g_running.store(true);

    std::thread gameThread;
    if (isServer) {
        gameThread = std::thread(serverGameLoop);
    }
    else {
        gameThread = std::thread(clientGameLoop);
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

    // Signal the game thread to stop and wait for it
    g_running.store(false);
    if (gameThread.joinable()) {
        gameThread.join();
    }

    cleanupWinsock();
}
