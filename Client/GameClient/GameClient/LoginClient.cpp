#include "LoginClient.h"
#include <cstring>

bool LoginClient::connectTo(const char* ip, int port) {
    if (!wsaInit) {
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;
        wsaInit = true;
    }
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons((u_short)port);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(sock); sock = INVALID_SOCKET; return false;
    }

    // 논블로킹으로 전환
    u_long nb = 1;
    ioctlsocket(sock, FIONBIO, &nb);
    return true;
}

bool LoginClient::sendLogin(const std::string& id, const std::string& pw) {
    if (!isConnected()) return false;
    std::string req = "LOGIN#" + id + "#" + pw;     // 서버가 기대하는 포맷
    return send(sock, req.c_str(), (int)req.size(), 0) != SOCKET_ERROR;
}

bool LoginClient::sendLogout(const std::string& id) {
    if (!isConnected()) return false;
    std::string req = "LOGOUT#" + id;               // 서버가 기대하는 포맷
    return send(sock, req.c_str(), (int)req.size(), 0) != SOCKET_ERROR;
}

std::string LoginClient::pollResponse() {
    if (!isConnected()) return {};
    char buf[1024];
    int n = recv(sock, buf, sizeof(buf) - 1, 0);
    if (n > 0) { buf[n] = '\0'; return std::string(buf); }
    if (n == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK) return {};
    // 그 외 에러/연결 종료
    return {};
}

void LoginClient::close() {
    if (sock != INVALID_SOCKET) { closesocket(sock); sock = INVALID_SOCKET; }
    if (wsaInit) { WSACleanup(); wsaInit = false; }
}