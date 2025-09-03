#include "Net_S.h"

Net_S::Net_S()
    : wsaData{}
    , ServerSocket(INVALID_SOCKET)
    , serverAddr{}
{
    InitializeDatabase();
}

Net_S::~Net_S()
{
    Cleanup();
}

// 요기 만들어놓은 임시 데이터 베이스
void Net_S::InitializeDatabase()
{
    userDatabase["Admin"] = "123123";
    userDatabase["Danjee"] = "seongung7431";
}

bool Net_S::Initialize()
{
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cout << "WSAStartup failed: " << result << std::endl;
        return false;
    }

    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ServerSocket == INVALID_SOCKET)
    {
        std::cout << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }
    return true;
}

bool Net_S::BindAndListen()
{
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    int result = bind(ServerSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        std::cout << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(ServerSocket);
        WSACleanup();
        return false;
    }

    result = listen(ServerSocket, 5);
    if (result == SOCKET_ERROR)
    {
        std::cout << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(ServerSocket);
        WSACleanup();
        return false;
    }
    std::cout << "Server is listening on port " << SERVER_PORT << std::endl;
    return true;
}

void Net_S::Run()
{
    if (!Initialize() || !BindAndListen())
    {
        return;
    }

    while (true)
    {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(ServerSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cout << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }
        std::cout << "Client connected." << std::endl;
        
        HandleClient(clientSocket);
    }
}

void Net_S::HandleClient(SOCKET clientSocket)
{
    char buffer[BUFFER_SIZE] = { 0 };
    int recvResult = 0;

    while ((recvResult = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0)
    {
        int safeLen = (recvResult < BUFFER_SIZE) ? recvResult : (BUFFER_SIZE - 1);
        buffer[safeLen] = '\0';
        std::cout << "Received from client: " << buffer << std::endl;

        std::vector<std::string> parts = Split(buffer, '#');
        std::string response = "";

        if (!parts.empty())
        {
            std::string command = parts[0];
            if (command == "LOGIN" && parts.size() == 3)
            {
                std::string userId = parts[1];
                std::string password = parts[2];

                if (userDatabase.count(userId) && userDatabase[userId] == password)
                {
                    if (std::find(loggedInUsers.begin(), loggedInUsers.end(), userId) == loggedInUsers.end())
                    {
                        loggedInUsers.push_back(userId);
                        response = "LOGIN_SUCCESS";
                        std::cout << "User '" << userId << "' logged in." << std::endl;
                    }
                    else
                    {
                        response = "LOGIN_FAIL_ALREADY_LOGGED_IN";
                    }
                }
                else
                {
                    response = "LOGIN_FAIL_INVALID_CREDENTIALS";
                }
            }
            else if (command == "LOGOUT" && parts.size() == 2)
            {
                std::string userId = parts[1];
                auto it = std::find(loggedInUsers.begin(), loggedInUsers.end(), userId);
                if (it != loggedInUsers.end())
                {
                    loggedInUsers.erase(it);
                    response = "LOGOUT_SUCCESS";
                    std::cout << "User '" << userId << "' logged out." << std::endl;
                }
                else
                {
                    response = "LOGOUT_FAIL_NOT_LOGGED_IN";
                }
            }
            else
            {
                response = "INVALID_COMMAND";
            }
        }

        if (!response.empty())
        {
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        
        memset(buffer, 0, BUFFER_SIZE);
    }

    if (recvResult == 0)
    {
        std::cout << "Connection closing..." << std::endl;
    }
    else
    {
        std::cout << "Recv failed: " << WSAGetLastError() << std::endl;
    }

    closesocket(clientSocket);
    std::cout << "Client disconnected." << std::endl;
}


std::vector<std::string> Net_S::Split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}


void Net_S::Cleanup()
{
    if (ServerSocket != INVALID_SOCKET)
    {
        closesocket(ServerSocket);
    }
    WSACleanup();
}


int main()
{
    Net_S server;
    server.Run();
    return 0;
}