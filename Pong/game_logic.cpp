#include "game_logic.h"
#include <cmath>
#include <algorithm>

void ResetGame() {
    ball.x = (WINDOW_SCREEN_WIDTH - ball.w) / 2;
    ball.y = (WINDOW_SCREEN_HEIGHT - ball.h) / 2;
    player1.y = player2.y = (WINDOW_SCREEN_HEIGHT - playerHeight) / 2;
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