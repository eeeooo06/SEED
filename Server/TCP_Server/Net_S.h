#pragma once
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <functional>
#include <ctime>
#include <Winsock2.h>

const int SERVER_PORT = 32000;
const int BUFFER_SIZE = 1024;

struct ServerInfo {
    std::string id, name, ip;
    int port = 0;
    int load = 5;          // 0~100
    bool online = true;
};

struct CharacterInfo {
    int id = 0;
    std::string name;
    int level = 1;
    std::string clazz;     // e.g., Warrior, Mage
};

class Net_S {
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

    // --- 로비 추가 ---
    std::string CreateSession(const std::string& userId);
    bool        CheckSession(const std::string& token, std::string& outUserId);
    std::string GenerateWorldTicket(const std::string& userId, int charId, const std::string& serverId);

    // 네트워크
    WSAData wsaData;
    SOCKET ServerSocket { INVALID_SOCKET };
    sockaddr_in serverAddr;

    // 계정/세션
    std::map<std::string, std::string> userDatabase;           // user->pw
    std::vector<std::string> loggedInUsers;                    // 간단 중복 로그인 방지
    std::unordered_map<std::string, std::string> sessionToUser; // token->user
    const std::string SESSION_SECRET = "demo-secret-change-me";

    // 서버 리스트 & 캐릭터 DB
    std::vector<ServerInfo> servers;
    std::map<std::string, std::vector<CharacterInfo>> userCharacters; // user->chars
    int nextCharId = 1000;
};
