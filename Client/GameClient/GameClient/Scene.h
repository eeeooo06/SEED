#pragma once
#define NOMINMAX

#include <memory>
#include <string>
#include <cmath>        // fmod
#include <algorithm>

#include "Graphics.h"
#include "Image.h"
#include "TextureManager.h"
#include "TextDX.h"
#include "Input.h"
#include "Game.h"

#include "LoginClient.h"    // connectTo / login / update / status / requestServerList ...


// 화면(씬) 구분
enum class SceneList { Lobby, Loading, ServerList, Main };

class LoginClient;
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

    void update(float) {} // no-op

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
    LoginClient* client = nullptr;   // 네트워크 클라이언트(상태머신)
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

    // 위치/레이아웃
    RECT rcId{}, rcPw{}, rcMsg{};
    RECT rcPanel{};
    RECT rcLoginBtn{}, rcRegisterBtn{}, rcExitBtn{};

    // 반투명 패널만 쿼드로 (배경 톤다운용). 입력 박스/버튼은 이미지로 교체.
    LP_VERTEXBUFFER vbPanel = nullptr;

    // 아틀라스(한 장) + 요소 이미지
    TextureManager lobbyAtlas;
    TextureManager loginUiTex;
    Image loginUi;
    Image idBoxImg, pwBoxImg;
    Image loginBtnImg, registerBtnImg, exitBtnImg;

    // 입력 상태
    bool focusPw = false;
    std::string id, pw;
    bool lmbPrev = false;

    static bool pointInRect(int x, int y, const RECT& r) {
        return (x >= r.left && x < r.right && y >= r.top && y < r.bottom);
    }

    // 하단-우측 고정 패널 레이아웃
    void buildPanelAndBoxes() {
        const int panelW = 420;
        const int panelH = 220;
        const int margin = 24;
        const int x = GAME_WIDTH - panelW - margin;
        const int y = GAME_HEIGHT - panelH - margin;
        rcPanel = { x, y, x + panelW, y + panelH };

        const int pad = 18;
        const int boxH = 38;
        const int boxW = panelW - pad * 2;
        int yy = y + pad + 28;

        rcId = { x + pad, yy, x + pad + boxW, yy + boxH }; yy += boxH + 16;
        rcPw = { x + pad, yy, x + pad + boxW, yy + boxH }; yy += boxH + 24;

        const int btnW = 100, btnH = 42;
        const int btnSpacing = 20;
        const int totalBtnW = btnW * 3 + btnSpacing * 2;
        int btnX = x + (panelW - totalBtnW) / 2;

        rcLoginBtn = { btnX, yy, btnX + btnW, yy + btnH }; btnX += btnW + btnSpacing;
        rcRegisterBtn = { btnX, yy, btnX + btnW, yy + btnH }; btnX += btnW + btnSpacing;
        rcExitBtn = { btnX, yy, btnX + btnW, yy + btnH };

        rcMsg = { x + pad, y + 8, x + panelW - pad, y + 28 };

        // 패널만 반투명 쿼드 생성 (입력 박스/버튼은 이미지로 그림)
        if (vbPanel) { vbPanel->Release(); vbPanel = nullptr; }
        VertexC v[4] = {
            { (float)rcPanel.left,  (float)rcPanel.top,    0, 1, D3DCOLOR_ARGB(200, 20, 24, 32) },
            { (float)rcPanel.right, (float)rcPanel.top,    0, 1, D3DCOLOR_ARGB(200, 20, 24, 32) },
            { (float)rcPanel.right, (float)rcPanel.bottom, 0, 1, D3DCOLOR_ARGB(200, 20, 24, 32) },
            { (float)rcPanel.left,  (float)rcPanel.bottom, 0, 1, D3DCOLOR_ARGB(200, 20, 24, 32) },
        };
        d.g->createVertexBuffer(v, sizeof(v), vbPanel);
    }

    // src(아틀라스 내 픽셀 Rect) -> 화면 rc에 맞춰 배치/스케일
    void setupSprite(Image& dst, const RECT& src, const RECT& rc) {
        const int sw = src.right - src.left;
        const int sh = src.bottom - src.top;

        if (!dst.initialize(d.g, sw, sh, 1, &lobbyAtlas)) // 적절한 포인터 사용
            throw GameError(gameErrorNS::FATAL_ERROR, "Image init failed");

        dst.setSpriteDataRect(src);

        const float dw = float(rc.right - rc.left);
        const float dh = float(rc.bottom - rc.top);

        // 1) 가로 기준으로 꽉 채우고(혹은 min(sx,sy)로 '전체가 보이게'도 가능)
        const float sx = dw / float(sw);
        const float sy = dh / float(sh);
        const float s = sx;           // 가로를 기준으로 맞춤 (필요하면 float s = std::min(sx, sy);)

        dst.setScale(s);

        // 2) 실제 그려질 크기
        const float drawnW = float(sw) * s;
        const float drawnH = float(sh) * s;

        // 3) 정렬 방식: 중앙 정렬
        const float x = float(rc.left) + (dw - drawnW) * 0.5f;
        const float y = float(rc.top) + (dh - drawnH) * 0.5f;
        dst.setX(x);
        dst.setY(y);
    }

public:
    LobbyScene(const SceneDeps& deps) : d(deps) {}

    void enter() override {
        ShowCursor(TRUE);
        font.initialize(d.g, 24, true, false, "Arial");

        // 1) 한 장짜리 로그인 UI 로드
        if (!loginUiTex.initialize(d.g, "pictures\\loginboxUI.png"))
            throw(GameError(gameErrorNS::FATAL_ERROR, "login UI load fail"));
        loginUi.initialize(d.g, 0, 0, 1, &loginUiTex);

        // 2) 우하단 앵커 배치
        const int margin = 24;
        const int panelW = (int)loginUi.getWidth();   // 472
        const int panelH = (int)loginUi.getHeight();  // 229
        const int x = GAME_WIDTH - panelW - margin;
        const int y = GAME_HEIGHT - panelH - margin;
        loginUi.setX((float)x);
        loginUi.setY((float)y);

        // 3) 클릭/텍스트 영역(이미지 기준 오프셋, 필요시 1~2px 미세조정)
        auto R = [&](int l, int t, int r, int b) { return RECT{ x + l, y + t, x + r, y + b }; };
        rcPanel = R(0, 0, panelW, panelH);
        rcId = R(70, 24, 70 + 320, 24 + 36);
        rcPw = R(70, 74, 70 + 320, 74 + 36);
        rcLoginBtn = R(20, 154, 20 + 120, 154 + 52);
        rcRegisterBtn = R(176, 154, 176 + 120, 154 + 52);
        rcExitBtn = R(332, 154, 332 + 120, 154 + 52);
        rcMsg = R(16, 8, panelW - 16, 30);
    }

    void exit() override {
        if (vbPanel) vbPanel->Release(), vbPanel = nullptr;
        lobbyAtlas.onLostDevice();
        loginUiTex.onLostDevice();
    }

    void tryLogin() {
        if (id.empty() || pw.empty()) return;
        if (!d.client) { d.login->setFailed("Client pointer is null"); return; }

        if (!d.client->isConnected()) {
            if (!d.client->connectTo("127.0.0.1", 32000)) {
                int e = WSAGetLastError();
                char buf[64]; sprintf_s(buf, "Connect failed (%d)", e);
                d.login->setFailed(buf);
                return;
            }
        }
        if (!d.client->login(id, pw)) {
            int e = WSAGetLastError();
            char buf[64]; sprintf_s(buf, "Send failed (%d)", e);
            d.login->setFailed(buf);
            return;
        }
        d.login->startPending(id, pw);
    }

    void update(float) override {
        int mx = d.i->getMouseX(), my = d.i->getMouseY();
        bool lmb = d.i->getMouseLButton();

        if (lmb && !lmbPrev) {
            if (pointInRect(mx, my, rcId))      focusPw = false;
            if (pointInRect(mx, my, rcPw))      focusPw = true;
            if (pointInRect(mx, my, rcLoginBtn))    tryLogin();
            if (pointInRect(mx, my, rcRegisterBtn)) { /* TODO: Register */ }
            if (pointInRect(mx, my, rcExitBtn))     PostQuitMessage(0);
        }
        lmbPrev = lmb;

        for (int ch = d.i->getCharIn(); ch != 0; ch = d.i->getCharIn()) {
            if (ch == '\r' || ch == '\n') { if (!focusPw) focusPw = true; else tryLogin(); continue; }
            if (ch == '\t') { focusPw = !focusPw; continue; }
            if (ch == 8) { auto& s = (focusPw ? pw : id); if (!s.empty()) s.pop_back(); continue; }
            if (ch >= 32 && ch < 127) {
                auto& s = (focusPw ? pw : id);
                if (s.size() < 32) s.push_back(char(ch));
            }
        }
    }

    void render() override {
        // 배경은 RPG가 그림 → 여기서는 UI만
        loginUi.draw();

        if (d.login->status() == LoginService::Status::Failed)
            font.print(d.login->error().c_str(), rcMsg, DT_CENTER | DT_VCENTER);

        // 입력 텍스트만 (라벨 "ID:/PW:"는 이미지에 그려져 있음)
        auto drawBoxText = [&](RECT rc, const std::string& text, bool pwmask) {
            std::string s = pwmask ? std::string(text.size(), '*') : text;

            // 원하는 만큼 조정(필요시 값만 바꾸면 됨)
            constexpr int kShiftX = 36;  // ▶ 오른쪽으로
            constexpr int kShiftY = 6;   // ▶ 아래로

            RECT inner = {
                rc.left + 10 + kShiftX,
                rc.top + 6 + kShiftY,
                rc.right - 10,
                rc.bottom - 6
            };
            font.print(s.c_str(), inner, DT_LEFT | DT_VCENTER);
            };

        drawBoxText(rcId, id, false);
        drawBoxText(rcPw, pw, true);
    }

    bool hasPending() const {
        return d.login->status() == LoginService::Status::Pending;
    }
};

// ---------------------------------------------------------
// 2) LOADING (로그인 대기) : 네트워크 업데이트/파싱은 LoginClient가 수행
// ---------------------------------------------------------
class LoadingScene : public IScene {
    SceneDeps d; TextDX font;
    float t = 0.f;

    TextureManager loadingTex;
    Image loadingBg;

public:
    LoadingScene(const SceneDeps& deps) : d(deps) {}
    void enter() override 
    {
        ShowCursor(TRUE);
        font.initialize(d.g, 24, true, false, "Arial");

        if (loadingTex.initialize(d.g, "pictures\\loading.png")) {
            loadingBg.initialize(d.g, 0, 0, 1, &loadingTex);
            loadingBg.setX(0); loadingBg.setY(0);
        }
    }

    void update(float dt) override {
        t += dt;
        d.login->update(dt);

        if (!d.client) return;
        d.client->update();

        using S = LoginClient::Status;
        if (d.client->status() == S::LoggedIn) {
            d.client->requestServerList();
            d.login->setSuccess();
        }
        else if (d.client->status() == S::Error) {
            d.login->setFailed(d.client->error());
        }
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
// 3) SERVER LIST
// ---------------------------------------------------------
class ServerListScene : public IScene {
    SceneDeps d; TextDX fontTitle, fontItem;
    int sel = 0;
    float reqTimer = 0.f;

    float enterCooldown = 0.25f;
    bool confirmed_ = false;
    std::string chosenServerId;

    TextureManager serverUiTex;
    Image serverUi;
    RECT lineRects[3]{};     // 서버 3줄 표기 영역
    RECT rcConnect{}, rcRefresh{}, rcBack{};
    bool lmbPrev = false;

public:
    ServerListScene(const SceneDeps& deps) : d(deps) {}

    void enter() override {
        ShowCursor(TRUE);
        fontTitle.initialize(d.g, 28, true, false, "Arial");
        fontItem.initialize(d.g, 20, false, false, "Arial");
        if (d.client) d.client->requestServerList();
        enterCooldown = 0.0f;
        confirmed_ = false;   // ← 멤버 값

        // UI 로드
        if (!serverUiTex.initialize(d.g, "pictures\\serverlistUI.png"))
            throw(GameError(gameErrorNS::FATAL_ERROR, "serverlist UI load fail"));
        serverUi.initialize(d.g, 0, 0, 1, &serverUiTex);

        // 우하단 앵커
        const int margin = 24;
        const int panelW = (int)serverUi.getWidth();   // 598
        const int panelH = (int)serverUi.getHeight();  // 319
        const int x = GAME_WIDTH - panelW - margin;
        const int y = GAME_HEIGHT - panelH - margin;
        serverUi.setX((float)x);
        serverUi.setY((float)y);

        // 슬롯/버튼 사각형(그림 기준 오프셋)
        auto R = [&](int l, int t, int r, int b) { return RECT{ x + l, y + t, x + r, y + b }; };
        lineRects[0] = R(36, 24, 560, 24 + 28);
        lineRects[1] = R(36, 64, 560, 64 + 28);
        lineRects[2] = R(36, 104, 560, 104 + 28);
        rcConnect = R(24, 244, 24 + 160, 244 + 60);
        rcRefresh = R(220, 244, 220 + 160, 244 + 60);
        rcBack = R(416, 244, 416 + 160, 244 + 60);
    }

    void update(float dt) override {
        if (!d.client) return;
        d.client->update();

        if (enterCooldown > 0.f) enterCooldown -= dt;

        // 키보드
        if (d.i->wasKeyPressed(VK_UP))   sel = (std::max)(0, sel - 1);
        if (d.i->wasKeyPressed(VK_DOWN)) {
            int n = (int)d.client->servers().size();
            if (n) sel = (std::min)(n - 1, sel + 1);
        }
        if (enterCooldown <= 0.f && d.i->wasKeyPressed(VK_RETURN)) {
            const auto& arr = d.client->servers();
            if (!arr.empty()) { confirmed_ = true; chosenServerId = arr[sel].id; }
        }

        // 마우스
        int mx = d.i->getMouseX(), my = d.i->getMouseY();
        bool lmb = d.i->getMouseLButton();
        auto hit = [&](const RECT& r) { return (mx >= r.left && mx < r.right && my >= r.top && my < r.bottom); };
        if (lmb && !lmbPrev) {
            if (hit(rcBack)) { confirmed_ = false; chosenServerId.clear(); /* 뒤로: 로비로 */ if (d.login) d.login->reset(); }
            if (hit(rcRefresh)) { if (d.client) d.client->requestServerList(); }
            if (hit(rcConnect)) {
                const auto& arr = d.client->servers();
                if (!arr.empty()) { confirmed_ = true; chosenServerId = arr[sel].id; }
            }
            // 라인 클릭으로 선택
            for (int i = 0; i < 3; i++) if (hit(lineRects[i])) sel = i;
        }
        lmbPrev = lmb;

        // 주기적 새로고침
        reqTimer += dt;
        if (reqTimer > 5.f) { d.client->requestServerList(); reqTimer = 0.f; }
    }

    void render() override {
        serverUi.draw();

        if (!d.client) return;
        const auto& arr = d.client->servers();

        if (arr.empty()) {
            RECT rr = lineRects[1];
            fontItem.print("Fetching server list...", rr, DT_LEFT | DT_VCENTER);
            return;
        }

        for (int i = 0; i < (int)std::min<size_t>(arr.size(), 3); ++i) {
            const auto& s = arr[i];
            char line[256];
            sprintf_s(line, "%s  %s:%d  load:%d%%  %s",
                s.id.c_str(), s.ip.c_str(), s.port, s.load, (s.online ? "online" : "down"));
            // 선택 강조(간단히 좌측에 '>' 붙이기)
            if (i == sel) {
                RECT mark = lineRects[i]; mark.right = mark.left + 12;
                fontItem.print(">", mark, DT_LEFT | DT_VCENTER);
            }
            fontItem.print(line, lineRects[i], DT_LEFT | DT_VCENTER);
        }
    }

    bool takeConfirmed() {
        if (!confirmed_) return false;
        confirmed_ = false;
        return true;
    }


    bool consumeConfirmed()
    {
        bool t = confirmed_; confirmed_ = false; return t;
    }
    int selectedIndex() const { return sel; }
    const std::string& selectedServerId() const { return chosenServerId; }
};

// ---------------------------------------------------------
// 4) MAIN MAP (임시)
// ---------------------------------------------------------
class MainMapScene : public IScene {
    SceneDeps d;
    TextureManager mapTex; Image map;
    TextDX font;

public:
    MainMapScene(const SceneDeps& deps) : d(deps) {}

    void enter() override {
        ShowCursor(FALSE);
        if (!mapTex.initialize(d.g, "pictures\\map.png"))
            mapTex.initialize(d.g, "pictures\\main.png");
        map.initialize(d.g, 0, 0, 1, &mapTex);
        map.setX(0); map.setY(0);
        font.initialize(d.g, 18, true, false, "Arial");
    }

    void update(float) override {
        if (d.i && d.i->isKeyDown(ESC_KEY)) {
            if (d.client) d.client->logout();
            if (d.login)  d.login->reset();
        }
    }

    void render() override {
        map.draw();
        RECT r = { 0, GAME_HEIGHT - 28, GAME_WIDTH - 8, GAME_HEIGHT - 8 };
        font.print("ESC : Logout", r, DT_RIGHT | DT_VCENTER);
    }
};