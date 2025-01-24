#pragma once

#ifndef NETWORK_H
#define NETWORK_H

#include <winsock2.h>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

#pragma warning(disable:4996) 

// Game State Structure
typedef struct GameState {
    int ballX, ballY;
    int ballSpeedX, ballSpeedY;
    int paddle1Y, paddle2Y;
    int score1, score2;
} GameState;

// Function declarations
bool initializeWinsock();                // Initialize Winsock
bool startServer(int port);              // Start as server
bool connectToServer(int port); // Start as client
void sendData(const char* data, int size); // Send data
void receiveData(char* buffer, int size); // Receive data

void cleanupWinsock();                   // Cleanup Winsock

#endif
