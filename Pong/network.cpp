#include "network.h"
#include <iostream>

SOCKET serverSocket = INVALID_SOCKET; // Server socket
SOCKET clientSocket = INVALID_SOCKET; // Client socket
SOCKET connectionSocket = INVALID_SOCKET; // For server-client connection

bool initializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return false;
    }
    return true;
}

bool startServer(int port) {
    sockaddr_in serverAddr;

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        return false;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        return false;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        return false;
    }

    std::cout << "Server is listening on port " << port << "..." << std::endl;

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    connectionSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    if (connectionSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        return false;
    }

    std::cout << "Client connected!\n";
    return true;
}

bool connectToServer(const char* ip, int port) {
    sockaddr_in serverAddr;

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        return false;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    serverAddr.sin_port = htons(port);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return false;
    }

    std::cout << "Connected to server at " << ip << ":" << port << std::endl;
    return true;
}

void sendData(const char* data, int size) {
    SOCKET targetSocket = (serverSocket != INVALID_SOCKET) ? connectionSocket : clientSocket;
    send(targetSocket, data, size, 0);
}

void receiveData(char* buffer, int size) {
    SOCKET targetSocket = (serverSocket != INVALID_SOCKET) ? connectionSocket : clientSocket;
    recv(targetSocket, buffer, size, 0);
}

void cleanupWinsock() {
    if (serverSocket != INVALID_SOCKET) closesocket(serverSocket);
    if (connectionSocket != INVALID_SOCKET) closesocket(connectionSocket);
    if (clientSocket != INVALID_SOCKET) closesocket(clientSocket);
    WSACleanup();
}

void sendDataSafe(const char* data, int size) {
    SOCKET targetSocket = (serverSocket != INVALID_SOCKET) ? connectionSocket : clientSocket;
    if (targetSocket == INVALID_SOCKET) {
        std::cerr << "Socket invalid. Cannot send data." << std::endl;
        return;
    }

    int result = send(targetSocket, data, size, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        // Handle send error (e.g., reconnect, terminate loop)
    }
}

void receiveDataSafe(char* buffer, int size) {
    SOCKET targetSocket = (serverSocket != INVALID_SOCKET) ? connectionSocket : clientSocket;
    if (targetSocket == INVALID_SOCKET) {
        std::cerr << "Socket invalid. Cannot receive data." << std::endl;
        return;
    }

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(targetSocket, &readSet);

    timeval timeout;
    timeout.tv_sec = 5; // 5 seconds
    timeout.tv_usec = 0;

    int result = select(targetSocket + 1, &readSet, NULL, NULL, &timeout);
    if (result > 0) {
        int bytesReceived = recv(targetSocket, buffer, size, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Connection closed or error: " << WSAGetLastError() << std::endl;
        }
    }
    else if (result == 0) {
        std::cerr << "Receive timeout." << std::endl;
    }
    else {
        std::cerr << "Select error: " << WSAGetLastError() << std::endl;
    }
}
