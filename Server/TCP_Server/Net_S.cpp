#define TESTWORLD { "TestWorld", "Test",      "222.232.36.150", 32000, 12, true }

#include "Net_S.h"
#include <sstream>
#include <iomanip>

// ------------ 유틸: 앞/뒤 공백 및 개행 제거 ------------
static inline void TrimInPlace(std::string& s) {
    size_t b = 0, e = s.size();
    auto isws = [](char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
    while (b < e && isws(s[b])) ++b;
    while (e > b && isws(s[e - 1])) --e;
    if (b == 0 && e == s.size()) return;
    s = s.substr(b, e - b);
}
// -------------------------------------------------------

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

    // 데모용 서버 리스트
    servers.push_back(TESTWORLD);
    //servers.push_back({ "world-2", "서리 협곡", "222.232.36.150", 32000, 67, true });
    //servers.push_back({ "world-3", "용암 심연", "127.0.0.1",      32000,  0, false }); // 점검중

    // 데모용 캐릭터 DB
    userCharacters["Admin"] = {
        {1001, "Akane", 12, "Warrior"},
        {1002, "Lize",   7, "Mage"}
    };
    userCharacters["Danjee"] = {
        {1003, "Seong", 20, "Rogue"}
    };
    nextCharId = 1004;
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

    // 큐 넉넉히
    result = listen(ServerSocket, SOMAXCONN);
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
        std::cout << "Received from client: [" << buffer << "]" << std::endl;

        std::vector<std::string> parts = Split(buffer, '#');

        // ---- 여기! 각 파트 트림 ----
        for (auto& p : parts) TrimInPlace(p);

        std::string response = "";

        if (!parts.empty())
        {
            std::string command = parts[0];

            // --- LOGIN ---
            if (command == "LOGIN" && parts.size() == 3)
            {
                std::string userId = parts[1];
                std::string password = parts[2];

                if (userDatabase.count(userId) && userDatabase[userId] == password)
                {
                    if (std::find(loggedInUsers.begin(), loggedInUsers.end(), userId) == loggedInUsers.end())
                    {
                        loggedInUsers.push_back(userId);

                        // 세션 토큰 발급
                        std::string token = CreateSession(userId);
                        response = "LOGIN_SUCCESS#" + token;
                        std::cout << "User '" << userId << "' logged in. token=" << token << std::endl;
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
            // --- LOGOUT ---
            else if (command == "LOGOUT" && parts.size() == 2)
            {
                std::string token = parts[1];
                std::string userId;
                if (CheckSession(token, userId))
                {
                    // 세션 제거
                    sessionToUser.erase(token);
                    auto it = std::find(loggedInUsers.begin(), loggedInUsers.end(), userId);
                    if (it != loggedInUsers.end()) loggedInUsers.erase(it);
                    response = "LOGOUT_SUCCESS";
                    std::cout << "User '" << userId << "' logged out." << std::endl;
                }
                else response = "LOGOUT_FAIL_NOT_LOGGED_IN";
            }
            // --- GET_SERVERS ---
            else if (command == "GET_SERVERS" && parts.size() == 2)
            {
                std::string token = parts[1];
                std::string userId;
                if (!CheckSession(token, userId))
                {
                    response = "SERVER_LIST_FAIL#INVALID_SESSION";
                }
                else
                {
                    std::ostringstream oss;
                    oss << "SERVER_LIST#" << servers.size() << "#";
                    for (size_t i = 0; i < servers.size(); ++i)
                    {
                        const auto& s = servers[i];
                        oss << s.id << "|" << s.name << "|" << s.ip << "|" << s.port
                            << "|" << s.load << "|" << (s.online ? 1 : 0);
                        if (i + 1 < servers.size()) oss << ";";
                    }
                    response = oss.str();
                }
            }
            // --- GET_CHARACTERS ---
            else if (command == "GET_CHARACTERS" && parts.size() == 2)
            {
                std::string token = parts[1];
                std::string userId;
                if (!CheckSession(token, userId))
                {
                    response = "CHAR_LIST_FAIL#INVALID_SESSION";
                }
                else
                {
                    auto& vec = userCharacters[userId]; // 없으면 생성
                    std::ostringstream oss;
                    oss << "CHAR_LIST#" << vec.size() << "#";
                    for (size_t i = 0; i < vec.size(); ++i)
                    {
                        const auto& c = vec[i];
                        oss << c.id << "|" << c.name << "|" << c.level << "|" << c.clazz;
                        if (i + 1 < vec.size()) oss << ";";
                    }
                    response = oss.str();
                }
            }
            // --- CREATE_CHARACTER ---
            else if (command == "CREATE_CHARACTER" && parts.size() == 4)
            {
                std::string token = parts[1];
                std::string name = parts[2];
                std::string clazz = parts[3];

                std::string userId;
                if (!CheckSession(token, userId))
                {
                    response = "CREATE_FAIL#INVALID_SESSION";
                }
                else
                {
                    CharacterInfo c;
                    c.id = nextCharId++;
                    c.name = name;
                    c.level = 1;
                    c.clazz = clazz;
                    userCharacters[userId].push_back(c);
                    response = "CREATE_OK#" + std::to_string(c.id);
                }
            }
            // --- DELETE_CHARACTER ---
            else if (command == "DELETE_CHARACTER" && parts.size() == 3)
            {
                std::string token = parts[1];
                int         charId = std::stoi(parts[2]);

                std::string userId;
                if (!CheckSession(token, userId))
                {
                    response = "DELETE_FAIL#INVALID_SESSION";
                }
                else
                {
                    auto& vec = userCharacters[userId];
                    auto it = std::remove_if(vec.begin(), vec.end(),
                        [&](const CharacterInfo& c) { return c.id == charId; });
                    if (it != vec.end())
                    {
                        vec.erase(it, vec.end());
                        response = "DELETE_OK";
                    }
                    else response = "DELETE_FAIL#NOT_FOUND";
                }
            }
            // --- SELECT_CHARACTER (월드 접속 티켓 발급) ---
            else if (command == "SELECT_CHARACTER" && parts.size() == 4)
            {
                std::string token = parts[1];
                int         charId = std::stoi(parts[2]);
                std::string serverId = parts[3];

                std::string userId;
                if (!CheckSession(token, userId))
                {
                    response = "SELECT_FAIL#INVALID_SESSION";
                }
                else
                {
                    // 캐릭터 존재 확인
                    auto& vec = userCharacters[userId];
                    auto itc = std::find_if(vec.begin(), vec.end(),
                        [&](const CharacterInfo& c) { return c.id == charId; });
                    if (itc == vec.end())
                    {
                        response = "SELECT_FAIL#CHAR_NOT_FOUND";
                    }
                    else
                    {
                        // 서버 확인
                        auto its = std::find_if(servers.begin(), servers.end(),
                            [&](const ServerInfo& s) { return s.id == serverId; });
                        if (its == servers.end() || !its->online)
                        {
                            response = "SELECT_FAIL#SERVER_UNAVAILABLE";
                        }
                        else
                        {
                            std::string ticket = GenerateWorldTicket(userId, charId, serverId);
                            std::ostringstream oss;
                            oss << "SELECT_OK#" << its->ip << "#" << its->port << "#" << ticket;
                            response = oss.str();
                        }
                    }
                }
            }
            else
            {
                response = "INVALID_COMMAND";
            }
        }

        if (!response.empty()) {
            response += "\n"; // 메시지 경계
            int sent = send(clientSocket, response.c_str(), (int)response.length(), 0);
            std::cout << "Send: [" << response << "] (" << sent << " bytes)" << std::endl;
        }

        memset(buffer, 0, BUFFER_SIZE);
    }

    if (recvResult == 0)  std::cout << "Connection closing..." << std::endl;
    else                  std::cout << "Recv failed: " << WSAGetLastError() << std::endl;

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

std::string Net_S::CreateSession(const std::string& userId)
{
    // token = userId:timestamp:hash(userId+timestamp+SECRET)
    std::time_t now = std::time(nullptr);
    std::string core = userId + ":" + std::to_string((long long)now);
    std::hash<std::string> h;
    std::string sig = std::to_string(h(core + ":" + SESSION_SECRET));
    std::string token = core + ":" + sig;
    sessionToUser[token] = userId;
    return token;
}

bool Net_S::CheckSession(const std::string& token, std::string& outUserId)
{
    auto it = sessionToUser.find(token);
    if (it == sessionToUser.end()) return false;

    // (데모) 만료 검사 생략. 실제에선 timestamp + 만료 + 서명 재검증 필요.
    outUserId = it->second;
    return true;
}

std::string Net_S::GenerateWorldTicket(const std::string& userId, int charId, const std::string& serverId)
{
    // 간단 서명 티켓: userId|charId|serverId|ts|sig
    std::time_t now = std::time(nullptr);
    std::string payload = userId + "|" + std::to_string(charId) + "|" + serverId + "|" + std::to_string((long long)now);
    std::hash<std::string> h;
    std::string sig = std::to_string(h(payload + ":" + SESSION_SECRET));
    return payload + "|" + sig;
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
