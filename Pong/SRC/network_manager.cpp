#include "..\HEADER\network_manager.h"
#include "..\HEADER\constants.h"
#include <iostream>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "Ws2_32.lib")

NetworkManager::NetworkManager()
    : m_mode(NetworkMode::NONE)
    , m_isConnected(false)
    , m_isInitialized(false)
    , m_serverSocket(INVALID_SOCKET)
    , m_clientSocket(INVALID_SOCKET)
    , m_connectionSocket(INVALID_SOCKET)
{
}

NetworkManager::~NetworkManager() {
    Cleanup();
}

bool NetworkManager::Initialize() {
    if (m_isInitialized) return true;
    
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        SetLastError("WSAStartup failed: " + std::to_string(result));
        return false;
    }
    
    m_isInitialized = true;
    return true;
}

void NetworkManager::Cleanup() {
    CloseAllSockets();
    
    if (m_isInitialized) {
        WSACleanup();
        m_isInitialized = false;
    }
    
    m_isConnected = false;
    m_mode = NetworkMode::NONE;
}

bool NetworkManager::StartServer(int port) {
    if (!m_isInitialized) {
        SetLastError("Network not initialized");
        return false;
    }
    
    if (!IsValidPort(port)) {
        SetLastError("Invalid port number: " + std::to_string(port));
        return false;
    }
    
    // Create server socket
    m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (!ValidateSocket(m_serverSocket)) {
        SetLastError("Failed to create server socket: " + std::to_string(WSAGetLastError()));
        return false;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
        SetLastError("Failed to set socket options: " + std::to_string(WSAGetLastError()));
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
        return false;
    }
    
    // Bind socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    
    if (bind(m_serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        SetLastError("Bind failed: " + std::to_string(WSAGetLastError()));
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
        return false;
    }
    
    // Listen for connections
    if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        SetLastError("Listen failed: " + std::to_string(WSAGetLastError()));
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
        return false;
    }
    
    m_mode = NetworkMode::SERVER;
    std::cout << "Server is listening on port " << port << "..." << std::endl;
    return true;
}

bool NetworkManager::WaitForClient() {
    if (m_mode != NetworkMode::SERVER || m_serverSocket == INVALID_SOCKET) {
        SetLastError("Server not properly initialized");
        return false;
    }
    
    std::cout << "Waiting for client connection..." << std::endl;
    
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    m_connectionSocket = accept(m_serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    
    if (!ValidateSocket(m_connectionSocket)) {
        SetLastError("Accept failed: " + std::to_string(WSAGetLastError()));
        return false;
    }
    
    // Set socket timeout
    if (!SetSocketTimeout(m_connectionSocket, Constants::NETWORK_TIMEOUT_MS)) {
        std::cerr << "Warning: Failed to set socket timeout" << std::endl;
    }
    
    m_isConnected = true;
    std::cout << "Client connected from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;
    return true;
}

bool NetworkManager::ConnectToServer(const std::string& serverIP, int port) {
    if (!m_isInitialized) {
        SetLastError("Network not initialized");
        return false;
    }
    
    if (!IsValidIP(serverIP)) {
        SetLastError("Invalid IP address: " + serverIP);
        return false;
    }
    
    if (!IsValidPort(port)) {
        SetLastError("Invalid port number: " + std::to_string(port));
        return false;
    }
    
    // Create client socket
    m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (!ValidateSocket(m_clientSocket)) {
        SetLastError("Failed to create client socket: " + std::to_string(WSAGetLastError()));
        return false;
    }
    
    // Set socket timeout
    if (!SetSocketTimeout(m_clientSocket, Constants::NETWORK_TIMEOUT_MS)) {
        std::cerr << "Warning: Failed to set socket timeout" << std::endl;
    }
    
    // Connect to server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) != 1) {
        SetLastError("Invalid IP address format: " + serverIP);
        closesocket(m_clientSocket);
        m_clientSocket = INVALID_SOCKET;
        return false;
    }
    serverAddr.sin_port = htons(port);
    
    if (connect(m_clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        SetLastError("Connect failed: " + std::to_string(WSAGetLastError()));
        closesocket(m_clientSocket);
        m_clientSocket = INVALID_SOCKET;
        return false;
    }
    
    m_mode = NetworkMode::CLIENT;
    m_isConnected = true;
    std::cout << "Connected to server at " << serverIP << ":" << port << std::endl;
    return true;
}

bool NetworkManager::SendData(const void* data, int size) {
    if (!m_isConnected) {
        SetLastError("Not connected to any peer");
        return false;
    }
    
    SOCKET targetSocket = (m_mode == NetworkMode::SERVER) ? m_connectionSocket : m_clientSocket;
    if (!ValidateSocket(targetSocket)) {
        SetLastError("Invalid target socket");
        return false;
    }
    
    int bytesSent = send(targetSocket, (const char*)data, size, 0);
    if (bytesSent == SOCKET_ERROR) {
        SetLastError("Send failed: " + std::to_string(WSAGetLastError()));
        m_isConnected = false;
        return false;
    }
    
    if (bytesSent != size) {
        SetLastError("Incomplete send: " + std::to_string(bytesSent) + "/" + std::to_string(size));
        return false;
    }
    
    return true;
}

bool NetworkManager::ReceiveData(void* buffer, int size) {
    if (!m_isConnected) {
        SetLastError("Not connected to any peer");
        return false;
    }
    
    SOCKET targetSocket = (m_mode == NetworkMode::SERVER) ? m_connectionSocket : m_clientSocket;
    if (!ValidateSocket(targetSocket)) {
        SetLastError("Invalid target socket");
        return false;
    }
    
    int bytesReceived = recv(targetSocket, (char*)buffer, size, 0);
    if (bytesReceived == SOCKET_ERROR) {
        SetLastError("Receive failed: " + std::to_string(WSAGetLastError()));
        m_isConnected = false;
        return false;
    }
    
    if (bytesReceived == 0) {
        SetLastError("Connection closed by peer");
        m_isConnected = false;
        return false;
    }
    
    if (bytesReceived != size) {
        SetLastError("Incomplete receive: " + std::to_string(bytesReceived) + "/" + std::to_string(size));
        return false;
    }
    
    return true;
}

bool NetworkManager::SendGameState(const NetworkGameState& state) {
    return SendData(&state, sizeof(NetworkGameState));
}

bool NetworkManager::ReceiveGameState(NetworkGameState& state) {
    return ReceiveData(&state, sizeof(NetworkGameState));
}

bool NetworkManager::SendPaddlePosition(int paddleY) {
    return SendData(&paddleY, sizeof(int));
}

bool NetworkManager::ReceivePaddlePosition(int& paddleY) {
    return ReceiveData(&paddleY, sizeof(int));
}

void NetworkManager::SetLastError(const std::string& error) {
    m_lastError = error;
    std::cerr << "Network Error: " << error << std::endl;
}

bool NetworkManager::SetSocketTimeout(SOCKET socket, int timeoutMs) {
    if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutMs, sizeof(timeoutMs)) < 0) {
        return false;
    }
    
    if (setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeoutMs, sizeof(timeoutMs)) < 0) {
        return false;
    }
    
    return true;
}

void NetworkManager::CloseAllSockets() {
    if (m_connectionSocket != INVALID_SOCKET) {
        closesocket(m_connectionSocket);
        m_connectionSocket = INVALID_SOCKET;
    }
    
    if (m_clientSocket != INVALID_SOCKET) {
        closesocket(m_clientSocket);
        m_clientSocket = INVALID_SOCKET;
    }
    
    if (m_serverSocket != INVALID_SOCKET) {
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
    }
}

bool NetworkManager::ValidateSocket(SOCKET socket) {
    return socket != INVALID_SOCKET;
}

bool NetworkManager::IsValidPort(int port) {
    return port > 0 && port <= 65535;
}

bool NetworkManager::IsValidIP(const std::string& ip) {
    // Simple IP validation - check if it's a valid IPv4 address
    std::istringstream iss(ip);
    std::string token;
    int segments = 0;
    
    while (std::getline(iss, token, '.')) {
        if (token.empty() || token.length() > 3) return false;
        
        for (char c : token) {
            if (!isdigit(c)) return false;
        }
        
        int num = std::stoi(token);
        if (num < 0 || num > 255) return false;
        
        segments++;
    }
    
    return segments == 4;
}
