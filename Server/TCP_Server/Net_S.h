#pragma once
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "DB.h"
#include "Auth.h"
#include "Directory.h"
#include "Game.h"

class Net_S {
public:
    Net_S();
    ~Net_S();

    bool Initialize();
    bool BindAndListen();
    void Run();
    void Cleanup();
    bool InitializeDatabase();
    void handleLine(const std::string& input, SOCKET clientSocket);
    void HandleClient(SOCKET clientSocket);

private:
    bool CheckSession(const std::string& token, uint64_t& outAccountId);
    std::string CreateSession(uint64_t accountId);
    std::string GenerateWorldTicket(uint64_t accountId, uint64_t playerId, const std::string& serverName);

private:
    WSADATA wsaData{};
    SOCKET  ServerSocket{ INVALID_SOCKET };
    sockaddr_in serverAddr{};

    // DB
    std::unique_ptr<CommonDB::Db>         db_;
    std::unique_ptr<CommonDB::Auth>       auth_;
    std::unique_ptr<CommonDB::Directory>  dir_;
    std::unique_ptr<CommonDB::Game>       game_;

    // access_token -> account_id
    std::unordered_map<std::string, uint64_t> sessionToAccount_;
};
