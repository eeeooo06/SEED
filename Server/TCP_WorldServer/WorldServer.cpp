// WorldServer.cpp
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOMINMAX
#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <functional>

static const char* WORLD_ID = "world-1";      // 로그인서버에서 발급한 serverId와 일치해야 함
static const int   WORLD_PORT = 32100;          // world-1의 실제 포트 (리스트에 맞게)
static const char* SESSION_SECRET = "demo-secret-change-me"; // 로그인 서버와 동일!

struct Client {
    SOCKET s = INVALID_SOCKET;
    std::string rx;
    bool authed = false;
    std::string userId;
    int charId = 0;
    int eid = 0; // 엔티티 id (간단히 charId 재사용해도 됨)
    float x = 100.f, y = 100.f;
};

WSADATA g_wsa;
SOCKET g_listenSock = INVALID_SOCKET;
std::vector<Client> g_clients;

// ---- 유틸 ----
static inline void trim(std::string& s) {
    auto isws = [](char c) {return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
    size_t b = 0, e = s.size();
    while (b < e && isws(s[b])) ++b; while (e > b && isws(s[e - 1])) --e;
    if (b > 0 || e < s.size()) s = s.substr(b, e - b);
}
static std::vector<std::string> split(const std::string& s, char d) {
    std::vector<std::string> out; std::string t; std::istringstream iss(s);
    while (std::getline(iss, t, d)) out.push_back(t);
    return out;
}
static bool verifyTicket(const std::string& ticket, std::string& userId, int& charId) {
    // ticket = userId|charId|serverId|ts|sig
    auto f = split(ticket, '|');
    if (f.size() != 5) return false;
    userId = f[0];
    charId = std::stoi(f[1]);
    std::string serverId = f[2];
    long long ts = std::stoll(f[3]);
    std::string sig = f[4];

    if (serverId != WORLD_ID) return false; // 내 월드가 맞는지
    // 만료(예: 5분)
    long long now = (long long)std::time(nullptr);
    if (now - ts > 300) return false;

    std::hash<std::string> h;
    std::string payload = f[0] + "|" + f[1] + "|" + f[2] + "|" + f[3];
    std::string expect = std::to_string(h(payload + std::string(":") + SESSION_SECRET));
    return (sig == expect);
}
static bool sendLine(SOCKET s, const std::string& line) {
    std::string out = line + "\n";
    return send(s, out.c_str(), (int)out.size(), 0) != SOCKET_ERROR;
}
static void broadcast(const std::string& line) {
    for (auto& c : g_clients) if (c.authed && c.s != INVALID_SOCKET) sendLine(c.s, line);
}

// ---- 네트워크 초기화 ----
bool netInit() {
    if (WSAStartup(MAKEWORD(2, 2), &g_wsa) != 0) return false;
    g_listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (g_listenSock == INVALID_SOCKET) return false;

    sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(WORLD_PORT);
    if (bind(g_listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) return false;
    if (listen(g_listenSock, SOMAXCONN) == SOCKET_ERROR) return false;

    // 논블로킹
    u_long nb = 1; ioctlsocket(g_listenSock, FIONBIO, &nb);
    printf("WorldServer(%s) listening on %d\n", WORLD_ID, WORLD_PORT);
    return true;
}

void acceptLoop() {
    sockaddr_in ca; int alen = sizeof(ca);
    SOCKET s = accept(g_listenSock, (sockaddr*)&ca, &alen);
    if (s == INVALID_SOCKET) return;
    u_long nb = 1; ioctlsocket(s, FIONBIO, &nb);
    Client c; c.s = s; g_clients.push_back(c);
    printf("client connected. total=%zu\n", g_clients.size());
}

void handleLine(Client& c, const std::string& line) {
    // printf("RX: %s\n", line.c_str());
    if (!c.authed) {
        // AUTH_TICKET#<ticket>
        if (line.rfind("AUTH_TICKET#", 0) == 0) {
            std::string ticket = line.substr(12);
            trim(ticket);
            std::string uid; int chid = 0;
            if (verifyTicket(ticket, uid, chid)) {
                c.authed = true; c.userId = uid; c.charId = chid; c.eid = chid;
                sendLine(c.s, "AUTH_OK#" + uid + "#" + std::to_string(chid));
                // 간단 스폰 통지
                sendLine(c.s, "SPAWN#" + std::to_string(c.eid) + "#"
                    + std::to_string((int)c.x) + "#"
                    + std::to_string((int)c.y));
                printf("auth ok: %s/%d\n", uid.c_str(), chid);
            }
            else {
                sendLine(c.s, "AUTH_FAIL#INVALID_TICKET");
                printf("auth fail\n");
            }
        }
        else {
            sendLine(c.s, "ERR#NEED_AUTH");
        }
        return;
    }

    // ---- 인증 이후 명령 ----
    if (line.rfind("MOVE#", 0) == 0) {
        // MOVE#x#y
        auto a = split(line, '#');
        if (a.size() == 3) {
            c.x = (float)std::stoi(a[1]);
            c.y = (float)std::stoi(a[2]);
            // 변경사항을 즉시 브로드캐스트 (간단 버전)
            // SNAPSHOT#1#eid|x|y
            std::string pkt = "SNAPSHOT#1#" + std::to_string(c.eid) + "|" + std::to_string((int)c.x) + "|" + std::to_string((int)c.y);
            broadcast(pkt);
        }
    }
    else if (line.rfind("PING#", 0) == 0) {
        sendLine(c.s, "PONG#" + line.substr(5));
    }
    else if (line == "JOIN_WORLD") {
        // 필요 시 초기 스냅샷, 주변 엔티티 목록 전송
    }
}

void pump() {
    char buf[1024];
    for (auto it = g_clients.begin(); it != g_clients.end(); ) {
        Client& c = *it;
        int n = recv(c.s, buf, sizeof(buf) - 1, 0);
        if (n > 0) {
            buf[n] = 0; c.rx.append(buf);
            size_t p;
            while ((p = c.rx.find('\n')) != std::string::npos) {
                std::string line = c.rx.substr(0, p);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                handleLine(c, line);
                c.rx.erase(0, p + 1);
            }
            ++it;
        }
        else {
            if (n == 0 || (n == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)) {
                printf("client disconnected\n");
                closesocket(c.s);
                it = g_clients.erase(it);
            }
            else {
                ++it;
            }
        }
    }
}

int main() {
    if (!netInit()) { printf("net init failed\n"); return 1; }
    while (true) {
        acceptLoop();     // 논블로킹 accept
        pump();           // 클라이언트 수신/처리
        // TODO: 월드 업데이트(물리/AI) 30~60Hz로 돌리고, 틱마다 스냅샷 브로드캐스트
        Sleep(5);
    }
    return 0;
}
