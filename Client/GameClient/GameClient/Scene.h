#pragma once

#include <memory>
#include <string>
#include <cmath>        // fmod

#include "Graphics.h"
#include "Image.h"
#include "TextureManager.h"
#include "TextDX.h"
#include "Input.h"
#include "Game.h"

#include "LoginClient.h"    // connectTo / sendLogin / pollResponse / sendLogout

// 화면(씬) 구분
enum class SceneList { Lobby, Loading, Main };

// ------------------------------
// 로그인 상태 보관(네트워크 결과 반영용, 지연 시뮬 없음)
// ------------------------------
class LoginService {
public:
    enum class Status { Idle, Pending, Success, Failed, Cancelled };

private:
    Status status_ = Status::Idle;
    std::string id_, pw_, err_;

public:
    void startPending(const std::string& id, const std::string& pw) {
        id_ = id; pw_ = pw; err_.clear(); status_ = Status::Pending;
    }
    void setSuccess() { status_ = Status::Success; err_.clear(); }
    void setFailed(const std::string& e) { status_ = Status::Failed; err_ = e; }
    void cancel() { status_ = Status::Cancelled; }
    void reset() { status_ = Status::Idle; err_.clear(); id_.clear(); pw_.clear(); }

    // 틱 업데이트는 네트워크가 처리 → 여기선 no-op
    void update(float) {}

    Status status() const { return status_; }
    const std::string& error() const { return err_; }
    const std::string& id() const { return id_; }
    const std::string& pw() const { return pw_; }
};

// 씬들이 공유할 의존성
struct SceneDeps {
    Graphics* g = nullptr;
    Input* i = nullptr;
    TextureManager* atlas = nullptr;
    LoginService* login = nullptr;
    LoginClient* client = nullptr;   // ← 네 게임용 TCP 클라이언트
};

// 공통 인터페이스
class IScene {
public:
    virtual ~IScene() {}
    virtual void enter() {}
    virtual void exit() {}
    virtual void update(float dt) = 0;
    virtual void render() = 0;
};

// ---------------------------------------------------------
// 1) LOBBY (로그인 입력)
// ---------------------------------------------------------
class LobbyScene : public IScene {
    SceneDeps d;
    TextDX font;

    // 위치/레아웃
    RECT rcId{}, rcPw{}, rcBtn{}, rcMsg{};
    RECT rcPanel{};               // 패널 전체 영역

    // 정점 버퍼 (한 번 만들어서 계속 사용)
    LP_VERTEXBUFFER vbPanel = nullptr;
    LP_VERTEXBUFFER vbIdBox = nullptr;
    LP_VERTEXBUFFER vbPwBox = nullptr;
    LP_VERTEXBUFFER vbBtn = nullptr;

    // 입력 상태
    bool focusPw = false;
    std::string id, pw;
    bool lmbPrev = false;

    // 헬퍼: 하단-우측 고정 패널 재계산 + 정점 버퍼 생성
    void buildPanelAndBoxes() {
        // ===== 레이아웃 설정 =====
        const int panelW = 420;
        const int panelH = 220;
        const int margin = 24;              // 화면 오른쪽/아래 여백
        const int x = GAME_WIDTH - panelW - margin;
        const int y = GAME_HEIGHT - panelH - margin;
        rcPanel = { x, y, x + panelW, y + panelH };

        // 내부 여백, 박스 크기
        const int pad = 18;
        const int boxH = 38;
        const int boxW = panelW - pad * 2;
        int yy = y + pad + 28;              // 라벨 영역 조금 띄우기

        rcId = { x + pad, yy, x + pad + boxW, yy + boxH }; yy += boxH + 16;
        rcPw = { x + pad, yy, x + pad + boxW, yy + boxH }; yy += boxH + 24;

        const int btnW = 160, btnH = 42;
        const int btnX = x + (panelW - btnW) / 2;
        rcBtn = { btnX, yy, btnX + btnW, yy + btnH };

        rcMsg = { x + pad, y + 8, x + panelW - pad, y + 28 };

        // ===== 정점 버퍼 생성(패널/박스/버튼) =====
        auto makeVB = [&](const RECT& r, DWORD argb, LP_VERTEXBUFFER& out) {
            if (out) { out->Release(); out = nullptr; }
            VertexC v[4] = {
                { (float)r.left,  (float)r.top,    0, 1, argb },
                { (float)r.right, (float)r.top,    0, 1, argb },
                { (float)r.right, (float)r.bottom, 0, 1, argb },
                { (float)r.left,  (float)r.bottom, 0, 1, argb },
            };
            d.g->createVertexBuffer(v, sizeof(v), out);
            };

        // 반투명 패널, 밝은 입력박스, 버튼
        makeVB(rcPanel, D3DCOLOR_ARGB(200, 20, 24, 32), vbPanel);   // 진한 남색 반투명
        makeVB(rcId, D3DCOLOR_ARGB(230, 245, 245, 248), vbIdBox); // 밝은 회색
        makeVB(rcPw, D3DCOLOR_ARGB(230, 245, 245, 248), vbPwBox);
        makeVB(rcBtn, D3DCOLOR_ARGB(255, 80, 140, 240), vbBtn);   // 파란 버튼
    }

    static bool pointInRect(int x, int y, const RECT& r) {
        return (x >= r.left && x < r.right && y >= r.top && y < r.bottom);
    }

public:
    LobbyScene(const SceneDeps& deps) : d(deps) {}

    void enter() override 
    {
        ShowCursor(TRUE);
        font.initialize(d.g, 24, true, false, "Arial");
        buildPanelAndBoxes();   // <-- 처음 진입 시 생성
    }

    void exit() override 
    {
        if (vbPanel) vbPanel->Release(), vbPanel = nullptr;
        if (vbIdBox) vbIdBox->Release(), vbIdBox = nullptr;
        if (vbPwBox) vbPwBox->Release(), vbPwBox = nullptr;
        if (vbBtn)   vbBtn->Release(), vbBtn = nullptr;
    }

    void tryLogin() 
    {
        if (id.empty() || pw.empty()) return;

        if (!d.client) { d.login->setFailed("Client pointer is null"); return; }

        if (!d.client->isConnected()) 
        {
            if (!d.client->connectTo("222.232.36.150", 32000)) 
            {
                int e = WSAGetLastError();
                char buf[64]; 
                sprintf_s(buf, "Connect failed (%d)", e);
                d.login->setFailed(buf);    // 10061: 서버 미동작, 10060: 타임아웃 등
                return;
            }
        }

        if (!d.client->sendLogin(id, pw)) {
            int e = WSAGetLastError();
            char buf[64]; 
            sprintf_s(buf, "Send failed (%d)", e);
            d.login->setFailed(buf);
            return;
        }

        d.login->startPending(id, pw);
    }

    void update(float) override 
    {
        // 패널은 고정이지만, 만약 해상도를 나중에 바꿀 계획이면
        // 여기서 GAME_WIDTH/HEIGHT 변경 감지 후 buildPanelAndBoxes() 재호출하면 됨.

        // 마우스 포커스/클릭
        int mx = d.i->getMouseX(), my = d.i->getMouseY();
        bool lmb = d.i->getMouseLButton();

        if (lmb && !lmbPrev) {
            if (pointInRect(mx, my, rcId)) focusPw = false;
            if (pointInRect(mx, my, rcPw)) focusPw = true;
            if (pointInRect(mx, my, rcBtn)) tryLogin();
        }
        lmbPrev = lmb;

        // 문자 입력
        for (int ch = d.i->getCharIn(); ch != 0; ch = d.i->getCharIn()) {
            if (ch == '\r' || ch == '\n') {             // Enter
                if (!focusPw) focusPw = true;
                else          tryLogin();
                continue;
            }
            if (ch == '\t') { focusPw = !focusPw; continue; }
            if (ch == 8) {                               // Backspace
                auto& s = (focusPw ? pw : id);
                if (!s.empty()) s.pop_back();
                continue;
            }
            if (ch >= 32 && ch < 127) {
                auto& s = (focusPw ? pw : id);
                if (s.size() < 32) s.push_back(char(ch));
            }
        }
    }

    void render() override 
    {
        // 1) 패널/박스/버튼 도형
        d.g->drawQuad(vbPanel);
        d.g->drawQuad(vbIdBox);
        d.g->drawQuad(vbPwBox);
        d.g->drawQuad(vbBtn);

        // 2) 텍스트
        // 에러 메시지 (패널 상단)
        if (d.login->status() == LoginService::Status::Failed) {
            font.print(d.login->error().c_str(), rcMsg, DT_CENTER | DT_VCENTER);
        }

        // 라벨
        RECT r;
        r = { rcId.left, rcId.top - 22, rcId.right, rcId.top };
        font.print("ID", r, DT_LEFT);
        r = { rcPw.left, rcPw.top - 22, rcPw.right, rcPw.top };
        font.print("PASSWORD", r, DT_LEFT);

        // 내용
        auto drawBoxText = [&](RECT rc, const std::string& text, bool pwmask) {
            std::string s = pwmask ? std::string(text.size(), '*') : text;
            RECT inner = { rc.left + 10, rc.top + 6, rc.right - 10, rc.bottom - 6 };
            font.print(s.c_str(), inner, DT_LEFT | DT_VCENTER);
            };
        drawBoxText(rcId, id, false);
        drawBoxText(rcPw, pw, true);

        // 버튼 라벨
        font.print("LOGIN", rcBtn, DT_CENTER | DT_VCENTER);
    }

    bool hasPending() const {
        return d.login->status() == LoginService::Status::Pending;
    }
};

// ---------------------------------------------------------
// 2) LOADING (로그인 기다림) : 서버 응답 폴링
// ---------------------------------------------------------
class LoadingScene : public IScene 
{
    SceneDeps d; TextDX font;
    float t = 0.f;

    static bool startsWith(const std::string& s, const char* prefix) {
        const size_t n = std::strlen(prefix);
        return s.size() >= n && std::memcmp(s.data(), prefix, n) == 0;
    }

public:
    LoadingScene(const SceneDeps& deps) : d(deps) {}
    void enter() override { font.initialize(d.g, 24, true, false, "Arial"); ShowCursor(TRUE); }

    void update(float dt) override {
        t += dt;
        d.login->update(dt); // no-op (상태 보관만)

        if (!d.client) return;

        std::string resp = d.client->pollResponse();
        if (resp.empty()) return;

        if (resp == "LOGIN_SUCCESS") {
            d.login->setSuccess();
        }
        else if (startsWith(resp, "LOGIN_FAIL")) {
            // 서버가 주는 실패코드: LOGIN_FAIL_INVALID_CREDENTIALS / LOGIN_FAIL_ALREADY_LOGGED_IN / ...
            std::string nice = "Login failed";
            if (resp == "LOGIN_FAIL_INVALID_CREDENTIALS") nice = "Invalid ID or password";
            else if (resp == "LOGIN_FAIL_ALREADY_LOGGED_IN") nice = "Already logged in";
            d.login->setFailed(nice);
        }
        else if (resp == "INVALID_COMMAND") {
            d.login->setFailed("Invalid command");
        }
        // 그 외 문자열은 무시(필요시 로깅)
    }

    void render() override {
        RECT r = { 0, GAME_HEIGHT / 2 - 20, GAME_WIDTH, GAME_HEIGHT / 2 + 20 };
        std::string dots(int(std::fmod(t * 3.0, 3.0)) + 1, '.');
        font.print(("Logging in" + dots).c_str(), r, DT_CENTER | DT_VCENTER);
        RECT r2 = { 0, r.bottom + 30, GAME_WIDTH, r.bottom + 60 };
        font.print("Press ESC to cancel", r2, DT_CENTER);
    }

    bool isDoneSuccess() const { return d.login->status() == LoginService::Status::Success; }
    bool isFailed() const { return d.login->status() == LoginService::Status::Failed; }
    void cancelIfEsc(Input* in) { if (in->isKeyDown(ESC_KEY)) d.login->cancel(); }
};

// ---------------------------------------------------------
// 3) MAIN MAP
// ---------------------------------------------------------
class MainMapScene : public IScene {
    SceneDeps d;
    TextureManager mapTex; Image map;
    TextDX font;

public:
    MainMapScene(const SceneDeps& deps) : d(deps) {}

    void enter() override {
        ShowCursor(FALSE);
        // 임시 맵(파일 없으면 menu.png 재사용)
        if (!mapTex.initialize(d.g, "pictures\\map.png"))
            mapTex.initialize(d.g, "pictures\\menu.png");
        map.initialize(d.g, 0, 0, 1, &mapTex);
        map.setX(0); map.setY(0);
        font.initialize(d.g, 18, true, false, "Arial");
    }

    void update(float) override {
        // ESC → 로그아웃(로비로)
        if (d.i && d.i->isKeyDown(ESC_KEY)) {
            if (d.client && !d.login->id().empty()) {
                d.client->sendLogout(d.login->id());  // "LOGOUT#id" (서버가 LOGOUT_SUCCESS 응답) 
            }
            // 실제 전환은 RPG 쪽 로직에서 처리하면 됨
        }
    }

    void render() override {
        map.draw();
        RECT r = { 0, GAME_HEIGHT - 28, GAME_WIDTH - 8, GAME_HEIGHT - 8 };
        font.print("ESC : Logout", r, DT_RIGHT | DT_VCENTER);
    }
};
