#include "LoginClient.h"
#include <cstring>
#include <sstream>

bool LoginClient::connectTo(const char* ip, int port) {
    if (!wsaInit) { if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false; wsaInit = true; }
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return false;

    sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_addr.s_addr = inet_addr(ip); addr.sin_port = htons((u_short)port);
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) { closesocket(sock); sock = INVALID_SOCKET; return false; }

    u_long nb = 1; ioctlsocket(sock, FIONBIO, &nb); // 논블로킹
    st = Status::Connected; lastErr.clear(); sessionToken.clear(); rxBuf.clear();
    serverList.clear(); charList.clear();
    return true;
}

void LoginClient::close() {
    if (sock != INVALID_SOCKET) { closesocket(sock); sock = INVALID_SOCKET; }
    if (wsaInit) { WSACleanup(); wsaInit = false; }
    st = Status::Disconnected;
}

bool LoginClient::sendLine(const std::string& s) {
    if (!isConnected()) return false;
    std::string line = s + "\n";
    return send(sock, line.c_str(), (int)line.size(), 0) != SOCKET_ERROR;
}

bool LoginClient::login(const std::string& id, const std::string& pw) {
    if (!isConnected()) return false;
    loginId = id;
    st = Status::LoggingIn;
    return sendLine("LOGIN#" + id + "#" + pw);
}

bool LoginClient::requestServerList() { return !sessionToken.empty() && sendLine("GET_SERVERS#" + sessionToken); }
bool LoginClient::requestCharacters() { return !sessionToken.empty() && sendLine("GET_CHARACTERS#" + sessionToken); }
bool LoginClient::createCharacter(const std::string& name, const std::string& clazz) {
    return !sessionToken.empty() && sendLine("CREATE_CHARACTER#" + sessionToken + "#" + name + "#" + clazz);
}
bool LoginClient::deleteCharacter(int charId) {
    return !sessionToken.empty() && sendLine("DELETE_CHARACTER#" + sessionToken + "#" + std::to_string(charId));
}
bool LoginClient::selectCharacter(int charId, const std::string& serverId) {
    return !sessionToken.empty() && sendLine("SELECT_CHARACTER#" + sessionToken + "#" + std::to_string(charId) + "#" + serverId);
}
bool LoginClient::logout() {
    // 서버는 LOGOUT#<token> 또는 #id 둘 중 하나로 했는데, 토큰 기반이 안전
    if (sessionToken.empty()) return false;
    return sendLine("LOGOUT#" + sessionToken);
}

void LoginClient::update() {
    if (!isConnected()) return;
    char buf[1024];
    for (;;) {
        int n = recv(sock, buf, sizeof(buf) - 1, 0);
        if (n > 0) {
            buf[n] = 0;
            rxBuf.append(buf);
            // 라인 분리
            size_t pos;
            while ((pos = rxBuf.find('\n')) != std::string::npos) {
                std::string line = rxBuf.substr(0, pos);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                handleLine(line);
                rxBuf.erase(0, pos + 1);
            }
        }
        else {
            if (n == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK) break;
            if (n == 0) { lastErr = "Disconnected"; st = Status::Error; close(); }
            break;
        }
    }
}

std::vector<std::string> LoginClient::split(const std::string& s, char d) {
    std::vector<std::string> out; std::string t; std::istringstream iss(s);
    while (std::getline(iss, t, d)) out.push_back(t);
    return out;
}

bool LoginClient::connectWorld(const WorldConnectInfo& wc)
{
    // WSA는 이미 켜져 있다고 가정(로그인에서 사용)
    if (!wsaInit) { if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false; wsaInit = true; }
    worldLastErr.clear(); worldAuthed = false;

    // 소켓 만들고 연결
    worldSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (worldSock == INVALID_SOCKET) { worldLastErr = "world socket create fail"; return false; }

    sockaddr_in a{}; a.sin_family = AF_INET; a.sin_addr.s_addr = inet_addr(wc.ip.c_str()); a.sin_port = htons((u_short)wc.port);
    if (connect(worldSock, (sockaddr*)&a, sizeof(a)) == SOCKET_ERROR) {
        worldLastErr = "world connect fail";
        closesocket(worldSock); worldSock = INVALID_SOCKET;
        return false;
    }
    u_long nb = 1; ioctlsocket(worldSock, FIONBIO, &nb);

    // AUTH_TICKET 전송
    std::string line = "AUTH_TICKET#" + wc.ticket + "\n";
    if (send(worldSock, line.c_str(), (int)line.size(), 0) == SOCKET_ERROR) {
        worldLastErr = "world send AUTH_TICKET fail";
        closesocket(worldSock); worldSock = INVALID_SOCKET;
        return false;
    }
    return true;
}

void LoginClient::updateWorld()
{
    if (worldSock == INVALID_SOCKET) return;
    char buf[1024];
    for (;;) {
        int n = recv(worldSock, buf, sizeof(buf) - 1, 0);
        if (n > 0) {
            buf[n] = 0; worldRx.append(buf);
            size_t p;
            while ((p = worldRx.find('\n')) != std::string::npos) {
                std::string line = worldRx.substr(0, p);
                if (!line.empty() && line.back() == '\r') line.pop_back();

                // 간단 파싱: AUTH_OK / SNAPSHOT / SPAWN ...
                if (line.rfind("AUTH_OK#", 0) == 0) {
                    worldAuthed = true;
                    // 필요하면 userId/charId 파싱
                }
                else if (line.rfind("AUTH_FAIL#", 0) == 0) {
                    worldLastErr = line;
                }
                else {
                    // TODO: SNAPSHOT/CHAT 등 처리
                }
                worldRx.erase(0, p + 1);
            }
        }
        else {
            if (n == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK) break;
            // 종료/오류
            closesocket(worldSock); worldSock = INVALID_SOCKET;
            break;
        }
    }
}

void LoginClient::closeWorld()
{
    if (worldSock != INVALID_SOCKET) { closesocket(worldSock); worldSock = INVALID_SOCKET; }
    worldRx.clear(); worldAuthed = false; worldLastErr.clear();
}

void LoginClient::handleLine(const std::string& line) {
    // LOGIN 결과
    if (line.rfind("LOGIN_SUCCESS", 0) == 0) {
        auto parts = split(line, '#');                // [LOGIN_SUCCESS, token]
        sessionToken = (parts.size() >= 2 ? parts[1] : "");
        st = Status::LoggedIn; lastErr.clear();
        return;
    }
    if (line.rfind("LOGIN_FAIL", 0) == 0) {
        st = Status::Error; lastErr = line; return;
    }

    // 서버 리스트
    if (line.rfind("SERVER_LIST#", 0) == 0) {
        serverList.clear();
        auto a = split(line, '#'); // [SERVER_LIST, N, payload]
        if (a.size() >= 3) {
            auto items = split(a[2], ';');
            for (auto& s : items) {
                auto f = split(s, '|'); // id|name|ip|port|load|online
                if (f.size() == 6) {
                    ServerEntry e;
                    e.id = f[0]; e.name = f[1]; e.ip = f[2];
                    e.port = std::stoi(f[3]); e.load = std::stoi(f[4]); e.online = (f[5] == "1");
                    serverList.push_back(e);
                }
            }
        }
        return;
    }

    // 캐릭터 목록
    if (line.rfind("CHAR_LIST#", 0) == 0) {
        charList.clear();
        auto a = split(line, '#'); // [CHAR_LIST, M, payload]
        if (a.size() >= 3) {
            auto items = split(a[2], ';');
            for (auto& s : items) if (!s.empty()) {
                auto f = split(s, '|'); // id|name|level|class
                if (f.size() == 4) {
                    CharacterEntry c;
                    c.id = std::stoi(f[0]); c.name = f[1]; c.level = std::stoi(f[2]); c.clazz = f[3];
                    charList.push_back(c);
                }
            }
        }
        return;
    }

    // 생성/삭제/선택 결과 등은 필요 시 분기 추가
    // e.g., SELECT_OK#ip#port#ticket ...
}
