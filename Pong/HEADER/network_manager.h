#pragma once

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <memory>
#include <functional>

// Game state structure for network synchronization
struct NetworkGameState {
    int ballX, ballY;
    int ballSpeedX, ballSpeedY;
    int paddle1Y, paddle2Y;
    int score1, score2;
    
    NetworkGameState() : ballX(0), ballY(0), ballSpeedX(0), ballSpeedY(0), 
                        paddle1Y(0), paddle2Y(0), score1(0), score2(0) {}
};

class NetworkManager {
public:
    enum class NetworkMode {
        NONE,
        SERVER,
        CLIENT
    };
    
    NetworkManager();
    ~NetworkManager();
    
    // Network initialization and cleanup
    bool Initialize();
    void Cleanup();
    
    // Server operations
    bool StartServer(int port);
    bool WaitForClient();
    
    // Client operations
    bool ConnectToServer(const std::string& serverIP, int port);
    
    // Data transmission
    bool SendData(const void* data, int size);
    bool ReceiveData(void* buffer, int size);
    
    // Network state
    bool IsConnected() const { return m_isConnected; }
    NetworkMode GetMode() const { return m_mode; }
    
    // Error handling
    std::string GetLastError() const { return m_lastError; }
    
    // Game state synchronization
    bool SendGameState(const NetworkGameState& state);
    bool ReceiveGameState(NetworkGameState& state);
    bool SendPaddlePosition(int paddleY);
    bool ReceivePaddlePosition(int& paddleY);
    
private:
    // Network state
    NetworkMode m_mode;
    bool m_isConnected;
    bool m_isInitialized;
    
    // Sockets
    SOCKET m_serverSocket;
    SOCKET m_clientSocket;
    SOCKET m_connectionSocket;
    
    // Error handling
    std::string m_lastError;
    
    // Private methods
    void SetLastError(const std::string& error);
    bool SetSocketTimeout(SOCKET socket, int timeoutMs);
    void CloseAllSockets();
    
    // Network validation
    bool ValidateSocket(SOCKET socket);
    bool IsValidPort(int port);
    bool IsValidIP(const std::string& ip);
};

#endif // NETWORK_MANAGER_H
