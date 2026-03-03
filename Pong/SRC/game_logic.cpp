#include "..\HEADER\game_logic.h"



void ResetGame() {
    ball.x = (WINDOW_SCREEN_WIDTH - ball.w) / 2;
    ball.y = (WINDOW_SCREEN_HEIGHT - ball.h) / 2;
    player1.y = player2.y = (WINDOW_SCREEN_HEIGHT - playerHeight) / 2;
}


void MoveBall(SDL_Rect* ball, Speed& speed)
{
    // Update ball position first
    ball->x += speed.x;
    ball->y += speed.y;

    // wall collision 
    if (ball->y <= 0 || ball->y + ball->h >= WINDOW_SCREEN_HEIGHT)
    {
        speed.y = -speed.y; // Reverse vertical direction
        ball->y = MAX(0, MIN(ball->y, WINDOW_SCREEN_HEIGHT - ball->h)); // Clamp position
    }

    // Ball passed left edge — Player 2 scores
    if (ball->x + ball->w < 0)
    {
        score.player2++;
        ResetGame();
        speed.x = initBallSpeed;
        speed.y = 0;
        return;
    }

    // Ball passed right edge — Player 1 scores
    if (ball->x > WINDOW_SCREEN_WIDTH)
    {
        score.player1++;
        ResetGame();
        speed.x = -initBallSpeed;
        speed.y = 0;
        return;
    }

    // Paddle 1 collision (left side)
    if (ball->x <= player1.x + player1.w && ball->x > player1.x)
    {
        if (ball->y + ball->h >= player1.y && ball->y <= player1.y + player1.h)
        {
            // Reverse x direction
            speed.x = -speed.x;

            // reflection angle
            double hitPoint = (ball->y + ball->h / 2.0) - (player1.y + player1.h / 2.0);
            double normalizedHitPoint = hitPoint / (player1.h / 2.0);

            // max bounce angle is 75 degree
            double maxAngle = 75.0 * M_PI / 180.0; // Convert to radians

            // Calculate new speed components
            double bounceAngle = normalizedHitPoint * maxAngle;
            double ballSpeedMagnitude = sqrt(speed.x * speed.x + speed.y * speed.y);

            speed.y = static_cast<int>(sin(bounceAngle) * ballSpeedMagnitude);

            // Ensure ball doesn't get stuck in the paddle
            ball->x = player1.x + player1.w + 1;
        }
    }

    // Paddle 2 collision (right side)
    if (ball->x + ball->w >= player2.x && ball->x + ball->w < player2.x + player2.w)
    {
        if (ball->y + ball->h >= player2.y && ball->y <= player2.y + player2.h)
        {
            // Reverse horizontal direction
            speed.x = -speed.x;

            // reflection angle
            double hitPoint = (ball->y + ball->h / 2.0) - (player2.y + player2.h / 2.0);
            double normalizedHitPoint = hitPoint / (player2.h / 2.0);

            // max bounce angle is 75 degree
            double maxAngle = 75.0 * M_PI / 180.0; // Convert to radians

            // Calculate new speed components
            double bounceAngle = normalizedHitPoint * maxAngle;
            double ballSpeedMagnitude = sqrt(speed.x * speed.x + speed.y * speed.y);

            speed.y = static_cast<int>(sin(bounceAngle) * ballSpeedMagnitude);

            // Ensure ball doesn't get stuck in the paddle
            ball->x = player2.x - ball->w - 1;
        }
    }
}