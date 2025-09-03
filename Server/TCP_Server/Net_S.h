#pragma once

#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <Winsock2.h>

const int SERVER_PORT = 32000;
const int BUFFER_SIZE = 1024;

class Net_S
{
public:
    Net_S();
    ~Net_S();

    bool Initialize();
    bool BindAndListen();
    void Run();
    void Cleanup();

private:
    void HandleClient(SOCKET clientSocket);
    void InitializeDatabase();
    std::vector<std::string> Split(const std::string& s, char delimiter);

    WSAData wsaData;
    SOCKET ServerSocket{ INVALID_SOCKET };
    
    sockaddr_in serverAddr;

    // User database and session management
    std::map<std::string, std::string> userDatabase;
    std::vector<std::string> loggedInUsers;
};
