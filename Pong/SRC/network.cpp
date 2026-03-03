#include "..\HEADER\network.h"
#include <ws2tcpip.h>

SOCKET serverSocket = INVALID_SOCKET; // Server socket
SOCKET clientSocket = INVALID_SOCKET; // Client socket
SOCKET connectionSocket = INVALID_SOCKET; // For server-client connection

bool initializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %i\n", result);
        return false;
    }
    return true;
}

bool startServer(int port) {
    sockaddr_in serverAddr;

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        printf("Failed to create socket: %i\n", WSAGetLastError());
        return false;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed: %i\n", WSAGetLastError());
        closesocket(serverSocket);
        return false;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed: %i \n",WSAGetLastError());
        closesocket(serverSocket);
        return false;
    }

    printf("Server is listening on port %i ...\n", port);

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    connectionSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    if (connectionSocket == INVALID_SOCKET) {
        printf("Accept failed: %i" , WSAGetLastError() );
        closesocket(serverSocket);
        return false;
    }

    printf("Client connected!\n");
    return true;
}

bool connectToServer(int port) {
    char serverIp[16];

    printf( "Enter server ip address: ");
    scanf_s("%15s", serverIp, (unsigned)_countof(serverIp));

    sockaddr_in serverAddr;

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        printf( "Failed to create socket: %i\n" ,WSAGetLastError());
        return false;
    }

    serverAddr.sin_family = AF_INET;
    // Use inet_pton instead of deprecated inet_addr
    if (inet_pton(AF_INET, serverIp, &serverAddr.sin_addr) != 1) {
        printf("Invalid IP address format: %s\n", serverIp);
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }
    serverAddr.sin_port = htons(port);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connect failed: %i\n ", WSAGetLastError());
        closesocket(clientSocket);
        return false;
    }

    printf("Connected to server at %s : %i\n",serverIp, port);
    return true;
}

void sendData(const char* data, int size) {
    SOCKET targetSocket = (serverSocket != INVALID_SOCKET) ? connectionSocket : clientSocket;
    int bytesSent = send(targetSocket, data, size, 0);
    if (bytesSent == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
    } else if (bytesSent != size) {
        printf("send incomplete: %d/%d bytes\n", bytesSent, size);
    }
}

void receiveData(char* buffer, int size) {
    SOCKET targetSocket = (serverSocket != INVALID_SOCKET) ? connectionSocket : clientSocket;
    int bytesReceived = recv(targetSocket, buffer, size, 0);
    if (bytesReceived == SOCKET_ERROR) {
        printf("recv failed: %d\n", WSAGetLastError());
    } else if (bytesReceived == 0) {
        printf("Connection closed by peer\n");
    } else if (bytesReceived != size) {
        printf("recv incomplete: %d/%d bytes\n", bytesReceived, size);
    }
}

void cleanupWinsock() {
    if (serverSocket != INVALID_SOCKET) closesocket(serverSocket);
    if (connectionSocket != INVALID_SOCKET) closesocket(connectionSocket);
    if (clientSocket != INVALID_SOCKET) closesocket(clientSocket);
    WSACleanup();
}
