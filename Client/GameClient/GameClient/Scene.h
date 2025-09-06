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
    RECT rcPanel{};               // 패널 전체 영역
    RECT rcLoginBtn{}, rcRegisterBtn{}, rcExitBtn{};

    // 정점 버퍼 (한 번 만들어서 계속 사용)
    LP_VERTEXBUFFER vbPanel = nullptr;
    LP_VERTEXBUFFER vbIdBox = nullptr;
    LP_VERTEXBUFFER vbPwBox = nullptr;

    // 버튼 이미지
    TextureManager buttonTexture;
    Image loginButton;
    Image registerButton;
    Image exitButton;

    // 입력 상태
    bool focusPw = false;
    std::string id, pw;
    bool lmbPrev = false;

    // 하단-우측 고정 패널 재계산 + 정점 버퍼 생성
    void buildPanelAndBoxes() {
        const int panelW = 420;
        const int panelH = 220;
        const int margin = 24;              // 화면 오른쪽/아래 여백
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

        // 반투명 패널, 밝은 입력박스
        makeVB(rcPanel, D3DCOLOR_ARGB(200, 20, 24, 32), vbPanel);
        makeVB(rcId, D3DCOLOR_ARGB(230, 245, 245, 248), vbIdBox);
        makeVB(rcPw, D3DCOLOR_ARGB(230, 245, 245, 248), vbPwBox);
    }

    static bool pointInRect(int x, int y, const RECT& r) {
        return (x >= r.left && x < r.right && y >= r.top && y < r.bottom);
    }

public:
    LobbyScene(const SceneDeps& deps) : d(deps) {}

    void enter() override {
        ShowCursor(TRUE);
        font.initialize(d.g, 24, true, false, "Arial");
        buildPanelAndBoxes();

        if (!buttonTexture.initialize(d.g, "pictures\\menu.png"))
            throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing button texture"));

        // Initialize buttons
        if (!loginButton.initialize(d.g, 0, 0, 0, &buttonTexture))
            throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing login button"));
        loginButton.setSpriteDataRect({ 0, 0, 64, 32 });
        loginButton.setX(rcLoginBtn.left);
        loginButton.setY(rcLoginBtn.top);
        loginButton.setScale( (float)(rcLoginBtn.right - rcLoginBtn.left) / 64.0f);

        if (!registerButton.initialize(d.g, 0, 0, 0, &buttonTexture))
            throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing register button"));
        registerButton.setSpriteDataRect({ 64, 0, 128, 32 });
        registerButton.setX(rcRegisterBtn.left);
        registerButton.setY(rcRegisterBtn.top);
        registerButton.setScale( (float)(rcRegisterBtn.right - rcRegisterBtn.left) / 64.0f);

        if (!exitButton.initialize(d.g, 0, 0, 0, &buttonTexture))
            throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing exit button"));
        exitButton.setSpriteDataRect({ 128, 0, 192, 32 });
        exitButton.setX(rcExitBtn.left);
        exitButton.setY(rcExitBtn.top);
        exitButton.setScale( (float)(rcExitBtn.right - rcExitBtn.left) / 64.0f);
    }

    void exit() override {
        if (vbPanel) vbPanel->Release(), vbPanel = nullptr;
        if (vbIdBox) vbIdBox->Release(), vbIdBox = nullptr;
        if (vbPwBox) vbPwBox->Release(), vbPwBox = nullptr;
        buttonTexture.onLostDevice();
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
            if (pointInRect(mx, my, rcId)) focusPw = false;
            if (pointInRect(mx, my, rcPw)) focusPw = true;
            if (pointInRect(mx, my, rcLoginBtn)) tryLogin();
            if (pointInRect(mx, my, rcRegisterBtn)) { /* Register logic here */ }
            if (pointInRect(mx, my, rcExitBtn)) PostQuitMessage(0);
        }
        lmbPrev = lmb;

        for (int ch = d.i->getCharIn(); ch != 0; ch = d.i->getCharIn()) {
            if (ch == '\r' || ch == '\n') {
                if (!focusPw) focusPw = true;
                else tryLogin();
                continue;
            }
            if (ch == '\t') { focusPw = !focusPw; continue; }
            if (ch == 8) {
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

    void render() override {
        d.g->drawQuad(vbPanel);
        d.g->drawQuad(vbIdBox);
        d.g->drawQuad(vbPwBox);

        loginButton.draw();
        registerButton.draw();
        exitButton.draw();

        if (d.login->status() == LoginService::Status::Failed) {
            font.print(d.login->error().c_str(), rcMsg, DT_CENTER | DT_VCENTER);
        }

        RECT r;
        r = { rcId.left, rcId.top - 22, rcId.right, rcId.top };
        font.print("ID", r, DT_LEFT);
        r = { rcPw.left, rcPw.top - 22, rcPw.right, rcPw.top };
        font.print("PASSWORD", r, DT_LEFT);

        auto drawBoxText = [&](RECT rc, const std::string& text, bool pwmask) {
            std::string s = pwmask ? std::string(text.size(), '*') : text;
            RECT inner = { rc.left + 10, rc.top + 6, rc.right - 10, rc.bottom - 6 };
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

public:
    ServerListScene(const SceneDeps& deps) : d(deps) {}

    void enter() override {
        ShowCursor(TRUE);
        fontTitle.initialize(d.g, 28, true, false, "Arial");
        fontItem.initialize(d.g, 20, false, false, "Arial");
        if (d.client) d.client->requestServerList();
        enterCooldown = 0.0f;
        bool confirmed_ = false;
    }

    void update(float dt) override {
        if (!d.client) return;
        d.client->update();

        if (enterCooldown > 0.f) enterCooldown -= dt;

        if (d.i->wasKeyPressed(VK_UP))
            sel = (std::max)(0, sel - 1);
        if (d.i->wasKeyPressed(VK_DOWN)) {
            int n = (int)d.client->servers().size();
            if (n) sel = (std::min)(n - 1, sel + 1);
        }

        if (enterCooldown <= 0.f && d.i->wasKeyPressed(VK_RETURN)) {
            const auto& arr = d.client->servers();
            if (!arr.empty()) {
                confirmed_ = true;
                chosenServerId = arr[sel].id;
            }
        }

        reqTimer += dt;
        if (reqTimer > 5.f) { d.client->requestServerList(); reqTimer = 0.f; }
    }

    void render() override {
        RECT r = { 0, 30, GAME_WIDTH, 60 };
        fontTitle.print("Select a Server", r, DT_CENTER);

        const auto& arr = d.client->servers();
        int y = 100;
        if (arr.empty()) {
            RECT rr = { 0, y, GAME_WIDTH, y + 30 };
            fontItem.print("Fetching server list...", rr, DT_CENTER);
            return;
        }

        for (int i = 0; i < (int)arr.size(); ++i) {
            const auto& s = arr[i];
            char line[256];
            sprintf_s(line, "[%c]  %-10s  %-12s:%d   load:%d%%   %s",
                (i == sel ? '*' : ' '),
                s.id.c_str(), s.ip.c_str(), s.port, s.load, (s.online ? "online" : "down"));
            RECT rr = { 80, y, GAME_WIDTH - 80, y + 26 };
            fontItem.print(line, rr, DT_LEFT | DT_VCENTER);
            y += 28;
        }

        RECT hint = { 0, GAME_HEIGHT - 40, GAME_WIDTH, GAME_HEIGHT - 10 };
        fontItem.print("UP/DOWN: select   ENTER: confirm", hint, DT_CENTER);
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
            mapTex.initialize(d.g, "pictures\\menu.png");
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