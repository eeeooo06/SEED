#pragma once
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")

#include <string>
#include <vector>
#include <winsock2.h>

// 서버/캐릭터 엔트리
struct ServerEntry {
    std::string id, name, ip;
    int port = 0, load = 0;
    bool online = false;
};
struct CharacterEntry {
    int id = 0, level = 1;
    std::string name, clazz;
};

// ▼ 월드 접속에 사용할 정보
struct WorldConnectInfo {
    std::string ip;
    int         port = 0;
    std::string ticket;   // SELECT_OK 로 받은 티켓 그대로
};

class LoginClient {
public:
    enum class Status { Disconnected, Connected, LoggingIn, LoggedIn, Error };

    ~LoginClient() { close(); }

    // ── 로그인 서버 쪽 ───────────────────────────────────────
    bool connectTo(const char* ip, int port);
    bool isConnected() const { return sock != INVALID_SOCKET; }
    void close();

    // 틱마다 호출(논블로킹 recv + 파싱)
    void update();

    // 액션
    bool login(const std::string& id, const std::string& pw);
    bool requestServerList();
    bool requestCharacters();
    bool createCharacter(const std::string& name, const std::string& clazz);
    bool deleteCharacter(int charId);
    bool selectCharacter(int charId, const std::string& serverId);
    bool logout();

    // 상태 조회
    Status status() const { return st; }
    const std::string& error() const { return lastErr; }
    const std::string& token() const { return sessionToken; }
    const std::vector<ServerEntry>& servers() const { return serverList; }
    const std::vector<CharacterEntry>& characters() const { return charList; }

    // ── ▼ 월드 서버 쪽 (필요 시 사용) ─────────────────────────
    bool connectWorld(const WorldConnectInfo& wc); // 월드 TCP 연결 + AUTH_TICKET 전송
    bool isWorldConnected() const { return worldSock != INVALID_SOCKET; }
    void updateWorld();                             // 월드 소켓 recv 처리
    void closeWorld();
    const std::string& worldError() const { return worldLastErr; }

private:
    // 내부 헬퍼
    void handleLine(const std::string& line);
    bool sendLine(const std::string& s); // s + "\n"
    static std::vector<std::string> split(const std::string& s, char d);

    // ── 로그인 소켓/상태 ─────────────────────────────────────
    WSAData wsa{};
    SOCKET  sock = INVALID_SOCKET;
    bool    wsaInit = false;
    std::string rxBuf;

    Status st = Status::Disconnected;
    std::string sessionToken, lastErr, loginId;

    // 데이터
    std::vector<ServerEntry> serverList;
    std::vector<CharacterEntry> charList;

    // ── 월드 소켓/상태 ───────────────────────────────────────
    SOCKET  worldSock = INVALID_SOCKET;
    std::string worldRx;
    std::string worldLastErr;
    bool worldAuthed = false;
};
