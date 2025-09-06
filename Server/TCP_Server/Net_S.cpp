#define _CRT_SECURE_NO_WARNINGS
#include "Net_S.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "SqlUtil.h"

// MySQL Connector/C++ 8.x
#include <mysql/jdbc.h>
#include <cppconn/exception.h>

// 링킹
#pragma comment(lib, "Ws2_32.lib")

// -------------------------------------------------------------
// 환경
// -------------------------------------------------------------
static constexpr int   SERVER_PORT = 32000;
static constexpr int   BUFFER_SIZE = 4096;
static const    char* SESSION_SECRET = "dev-only-secret";
static const    char* DEFAULT_WORLD_CODE = "TEST";

// -------------------------------------------------------------
// 간단 유틸
// -------------------------------------------------------------
static inline void TrimInPlace(std::string& s) {
    size_t b = 0, e = s.size();
    auto isws = [](char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
    while (b < e && isws(s[b])) ++b;
    while (e > b && isws(s[e - 1])) --e;
    if (b != 0 || e != s.size()) s = s.substr(b, e - b);
}
static std::vector<std::string> Split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens; std::string tok; std::istringstream is(s);
    while (std::getline(is, tok, delimiter)) tokens.push_back(tok);
    return tokens;
}

// -------------------------------------------------------------
// Net_S
// -------------------------------------------------------------
Net_S::Net_S() {}
Net_S::~Net_S() { Cleanup(); }

// DB 연결
bool Net_S::InitializeDatabase() {
    CommonDB::DbConfig cfg;
    cfg.host = "tcp://222.232.36.150:3306";
    cfg.user = "GM_Admin";
    cfg.pass = "GameMaster!@Admin";

    try {
        db_ = std::make_unique<CommonDB::Db>(cfg);
        auth_ = std::make_unique<CommonDB::Auth>(*db_);
        dir_ = std::make_unique<CommonDB::Directory>(*db_);
        game_ = std::make_unique<CommonDB::Game>(*db_);
        std::cout << "[DB] connected.\n";
        return true;
    }
    catch (const sql::SQLException& e) {
        const char* state = e.getSQLStateCStr();
        std::cerr << "[DB] connect error: " << e.what()
            << " (sqlstate=" << (state ? state : "NULL")
            << ", code=" << e.getErrorCode() << ")\n";
        return false; // ❗ 예외 다시 던지지 말고 false
    }
    catch (const std::exception& e) {
        std::cerr << "[DB] std::exception: " << e.what() << "\n";
        return false;
    }
}

// 네트워킹
bool Net_S::Initialize() {
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) { std::cout << "WSAStartup failed: " << result << std::endl; return false; }
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ServerSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed: " << WSAGetLastError() << std::endl; WSACleanup(); return false;
    }
    return true;
}
bool Net_S::BindAndListen() {
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    int result = bind(ServerSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        std::cout << "Bind failed: " << WSAGetLastError() << std::endl; closesocket(ServerSocket); WSACleanup(); return false;
    }
    result = listen(ServerSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cout << "Listen failed: " << WSAGetLastError() << std::endl; closesocket(ServerSocket); WSACleanup(); return false;
    }
    std::cout << "Server listening on " << SERVER_PORT << std::endl;
    return true;
}

void Net_S::Run() {
    if (!Initialize() || !BindAndListen()) return;

    while (true) {
        sockaddr_in clientAddr{}; int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(ServerSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) { std::cout << "Accept failed: " << WSAGetLastError() << std::endl; continue; }
        std::cout << "Client connected." << std::endl;
        HandleClient(clientSocket);
    }
}
void Net_S::Cleanup() {
    if (ServerSocket != INVALID_SOCKET) closesocket(ServerSocket);
    WSACleanup();
}

// 세션/티켓
std::string Net_S::CreateSession(uint64_t accountId) {
    std::time_t now = std::time(nullptr);
    std::string core = std::to_string((unsigned long long)accountId) + ":" + std::to_string((long long)now);
    std::hash<std::string> h; std::string sig = std::to_string(h(core + ":" + SESSION_SECRET));
    std::string token = core + ":" + sig;
    sessionToAccount_[token] = accountId;
    return token;
}
bool Net_S::CheckSession(const std::string& token, uint64_t& outAccountId) {
    auto it = sessionToAccount_.find(token);
    if (it == sessionToAccount_.end()) return false;
    outAccountId = it->second; return true;
}
std::string Net_S::GenerateWorldTicket(uint64_t accountId, uint64_t playerId, const std::string& serverName) {
    std::time_t now = std::time(nullptr);
    std::string payload = std::to_string((unsigned long long)accountId) + "|" +
        std::to_string((unsigned long long)playerId) + "|" +
        serverName + "|" + std::to_string((long long)now);
    std::hash<std::string> h; std::string sig = std::to_string(h(payload + ":" + SESSION_SECRET));
    return payload + "|" + sig;
}

// 클라 처리 (DB 기반 명령)
void Net_S::HandleClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE]{};
    int recvResult = 0;

    while ((recvResult = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[recvResult] = '\0';
        std::string input = buffer;
        std::cout << "Recv: " << input << std::endl;

        auto parts = Split(input, '#'); for (auto& p : parts) TrimInPlace(p);
        std::string response;

        if (!parts.empty()) {
            const std::string& command = parts[0];

            // -------------------------------------------------
            // REGISTER#username#password
            // -------------------------------------------------
            if (command == "REGISTER" && parts.size() == 3) {
                const std::string& username = parts[1];
                const std::string& password = parts[2];

                auto validUser = [](const std::string& u) {
                    if (u.size() < 3 || u.size() > 20) return false;
                    for (char c : u) if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_')) return false;
                    return true;
                    };
                if (!validUser(username)) {
                    response = "REGISTER_FAIL#INVALID_USERNAME";
                }
                else if (password.size() < 8 || password.size() > 64) {
                    response = "REGISTER_FAIL#INVALID_PASSWORD";
                }
                else {
                    try {
                        bool ok = auth_->CreateAccount(username, password);
                        response = ok ? "REGISTER_OK" : "REGISTER_FAIL#USER_EXISTS";
                    }
                    catch (const sql::SQLException& e) {
                        std::cerr << "[SQL] register: " << CommonDB::SqlUtil::Explain(e) << std::endl;
                        response = "REGISTER_FAIL#SERVER_ERROR";
                    }
                }
            }
            // -------------------------------------------------
            // LOGIN#username#password
            // -------------------------------------------------
            else if (command == "LOGIN" && parts.size() == 3) {
                const std::string& username = parts[1];
                const std::string& password = parts[2];
                try {
                    uint64_t accId = auth_->VerifyPasswordAndGetId(username, password);
                    if (accId == 0) {
                        response = "LOGIN_FAIL_INVALID_CREDENTIALS";
                    }
                    else {
                        auto token = CreateSession(accId);
                        response = "LOGIN_SUCCESS#" + token;
                        std::cout << "[login] " << username << " (id=" << accId << ")\n";
                    }
                }
                catch (const sql::SQLException& e) {
                    std::cerr << "[SQL] login error: " << CommonDB::SqlUtil::Explain(e) << std::endl;
                    response = "LOGIN_FAIL#SERVER_ERROR";
                }
            }
            // -------------------------------------------------
            // LOGOUT#token
            // -------------------------------------------------
            else if (command == "LOGOUT" && parts.size() == 2) {
                std::string token = parts[1];
                if (sessionToAccount_.erase(token)) response = "LOGOUT_SUCCESS";
                else                                 response = "LOGOUT_FAIL_NOT_LOGGED_IN";
            }
            // -------------------------------------------------
            // GET_SERVERS#token
            // -------------------------------------------------
            else if (command == "GET_SERVERS" && parts.size() == 2) {
                std::string token = parts[1]; uint64_t accountId = 0;
                if (!CheckSession(token, accountId)) {
                    response = "SERVER_LIST_FAIL#INVALID_SESSION";
                }
                else {
                    try {
                        auto list = dir_->FetchOnlineGameServers();
                        std::ostringstream oss;
                        oss << "SERVER_LIST#" << list.size() << "#";
                        for (size_t i = 0; i < list.size(); ++i) {
                            const auto& [world, name, host, port] = list[i];
                            oss << name << "|" << world << "|" << host << "|" << port << "|0|1";
                            if (i + 1 < list.size()) oss << ";";
                        }
                        response = oss.str();
                    }
                    catch (const sql::SQLException& e) {
                        std::cerr << "[SQL] serverlist: " << CommonDB::SqlUtil::Explain(e) << std::endl;
                        response = "SERVER_LIST_FAIL#SERVER_ERROR";
                    }
                }
            }
            // -------------------------------------------------
            // GET_CHARACTERS#token
            // -------------------------------------------------
            else if (command == "GET_CHARACTERS" && parts.size() == 2) {
                std::string token = parts[1]; uint64_t accountId = 0;
                if (!CheckSession(token, accountId)) {
                    response = "CHAR_LIST_FAIL#INVALID_SESSION";
                }
                else {
                    try {
                        auto rows = game_->ListPlayersByAccount(accountId);
                        std::ostringstream oss;
                        oss << "CHAR_LIST#" << rows.size() << "#";
                        for (size_t i = 0; i < rows.size(); ++i) {
                            const auto& r = rows[i];
                            oss << r.player_id << "|" << r.name << "|" << r.level << "|Unknown";
                            if (i + 1 < rows.size()) oss << ";";
                        }
                        response = oss.str();
                    }
                    catch (const sql::SQLException& e) {
                        std::cerr << "[SQL] charlist: " << CommonDB::SqlUtil::Explain(e) << std::endl;
                        response = "CHAR_LIST_FAIL#SERVER_ERROR";
                    }
                }
            }
            // -------------------------------------------------
            // CREATE_CHARACTER#token#name#class
            // -------------------------------------------------
            else if (command == "CREATE_CHARACTER" && parts.size() == 4) {
                std::string token = parts[1];
                std::string name = parts[2];
                std::string clazz = parts[3]; (void)clazz;
                uint64_t accountId = 0;
                if (!CheckSession(token, accountId)) {
                    response = "CREATE_FAIL#INVALID_SESSION";
                }
                else {
                    try {
                        bool ok = game_->CreatePlayer(accountId, DEFAULT_WORLD_CODE, name);
                        response = ok ? "CREATE_OK#0" : "CREATE_FAIL#DB_ERROR";
                    }
                    catch (const sql::SQLException& e) {
                        std::cerr << "[SQL] create: " << CommonDB::SqlUtil::Explain(e) << std::endl;
                        response = "CREATE_FAIL#SERVER_ERROR";
                    }
                }
            }
            // -------------------------------------------------
            // DELETE_CHARACTER#token#playerId
            // -------------------------------------------------
            else if (command == "DELETE_CHARACTER" && parts.size() == 3) {
                std::string token = parts[1]; uint64_t playerId = std::stoull(parts[2]);
                uint64_t accountId = 0;
                if (!CheckSession(token, accountId)) {
                    response = "DELETE_FAIL#INVALID_SESSION";
                }
                else {
                    try {
                        db_->UseGame();
                        std::unique_ptr<sql::PreparedStatement> ps(
                            db_->conn()->prepareStatement(
                                "DELETE FROM players WHERE player_id=? AND account_id=?"));
                        ps->setUInt64(1, playerId);
                        ps->setUInt64(2, accountId);
                        int n = ps->executeUpdate();
                        response = (n > 0) ? "DELETE_OK" : "DELETE_FAIL#NOT_FOUND";
                    }
                    catch (const sql::SQLException& e) {
                        std::cerr << "[SQL] delete: " << CommonDB::SqlUtil::Explain(e) << std::endl;
                        response = "DELETE_FAIL#SERVER_ERROR";
                    }
                }
            }
            // -------------------------------------------------
            // SELECT_CHARACTER#token#playerId#serverName
            // -------------------------------------------------
            else if (command == "SELECT_CHARACTER" && parts.size() == 4) {
                std::string token = parts[1];
                uint64_t    playerId = std::stoull(parts[2]);
                std::string serverName = parts[3];
                uint64_t accountId = 0;
                if (!CheckSession(token, accountId)) {
                    response = "SELECT_FAIL#INVALID_SESSION";
                }
                else {
                    try {
                        // 소유 확인
                        db_->UseGame();
                        std::unique_ptr<sql::PreparedStatement> ps(
                            db_->conn()->prepareStatement(
                                "SELECT 1 FROM players WHERE player_id=? AND account_id=?"));
                        ps->setUInt64(1, playerId);
                        ps->setUInt64(2, accountId);
                        std::unique_ptr<sql::ResultSet> rs(ps->executeQuery());
                        if (!rs->next()) {
                            response = "SELECT_FAIL#CHAR_NOT_FOUND";
                        }
                        else {
                            // 서버 host/port
                            db_->UseDirectory();
                            std::unique_ptr<sql::PreparedStatement> ps2(
                                db_->conn()->prepareStatement(
                                    "SELECT host, port FROM servers "
                                    "WHERE name=? AND server_type='GAME' AND status='ONLINE' LIMIT 1"));
                            ps2->setString(1, serverName);
                            std::unique_ptr<sql::ResultSet> rs2(ps2->executeQuery());
                            if (!rs2->next()) {
                                response = "SELECT_FAIL#SERVER_UNAVAILABLE";
                            }
                            else {
                                std::string host = rs2->getString(1);
                                int         port = rs2->getInt(2);
                                std::string ticket = GenerateWorldTicket(accountId, playerId, serverName);
                                std::ostringstream oss;
                                oss << "SELECT_OK#" << host << "#" << port << "#" << ticket;
                                response = oss.str();
                            }
                        }
                    }
                    catch (const sql::SQLException& e) {
                        std::cerr << "[SQL] select: " << CommonDB::SqlUtil::Explain(e) << std::endl;
                        response = "SELECT_FAIL#SERVER_ERROR";
                    }
                }
            }
            else {
                response = "INVALID_COMMAND";
            }
        }

        if (!response.empty()) {
            response.push_back('\n');
            send(clientSocket, response.c_str(), (int)response.size(), 0);
            std::cout << "Send: " << response;
        }
        memset(buffer, 0, sizeof(buffer));
    }

    if (recvResult == 0)  std::cout << "Connection closing...\n";
    else                  std::cout << "Recv failed: " << WSAGetLastError() << "\n";

    closesocket(clientSocket);
    std::cout << "Client disconnected.\n";
}

// main
int main() {
    try { Net_S server; server.Run(); }
    catch (...) { std::cerr << "Fatal: initialization failed.\n"; }
    return 0;
}
