#pragma once
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")

#include <string>
#include <winsock2.h>

class LoginClient {
public:
    ~LoginClient() { close(); }

    bool connectTo(const char* ip, int port);
    bool isConnected() const { return sock != INVALID_SOCKET; }

    // 서버 프로토콜 그대로 사용
    bool sendLogin(const std::string& id, const std::string& pw); // "LOGIN#id#pw"
    bool sendLogout(const std::string& id);                       // "LOGOUT#id"

    // 논블로킹 Poll. 응답 없으면 "" 반환, 있으면 서버 문자열 그대로 반환
    std::string pollResponse();

    void close();
private:
    WSAData wsa{};
    SOCKET  sock = INVALID_SOCKET;
    bool    wsaInit = false;
};