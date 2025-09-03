#pragma once

#ifndef _RPG_H																// ���� ������ �� ������ �����ϴ� ���
#define _RPG_H																// ���� ���Ǹ� �����Ѵ�.
#define WIN32_LEAN_AND_MEAN

#include "Game.h"
#include "TextureManager.h"
#include "Image.h"
#include "Dashboard.h"
#include "Entity.h"
#include "Scene.h"

#include "LoginClient.h"

namespace rpgNS
{
    const char FONT[] = "Courier New";  // font
    const int FONT_BIG_SIZE = 100;      // font height
    const int FONT_COLOR = SETCOLOR_ARGB(255,255,255,255);
}

class RPG : public Game
{
private:
    // RPG Member Variables
    TextureManager menuTexture; // 메뉴 텍스처
    TextureManager gameTextures; // 게임 텍스처
    Entity  player;             // 플레이어
    Bar dashboard;        // 대시보드
    Image   menu;               // 메뉴 이미지

    // Scene Login Memer VArialbes
    LoginClient client;
    LoginService login;
    std::unique_ptr<IScene> scene;
    SceneList current = SceneList::Lobby;

    std::string loggedInId;

    void changeScene(SceneList k)
    {
        current = k;

        // ★ 포인터를 '이름을 지정해서' 안전하게 채우자
        SceneDeps deps{};
        deps.g = graphics;
        deps.i = input;
        deps.atlas = &gameTextures;
        deps.login = &login;
        deps.client = &client;          // ← 핵심!

        switch (k) {
        case SceneList::Lobby:   scene = std::make_unique<LobbyScene>(deps);   break;
        case SceneList::Loading: scene = std::make_unique<LoadingScene>(deps); break;
        case SceneList::Main:    scene = std::make_unique<MainMapScene>(deps); break;
        }
        scene->enter();
    }

public:
    // constructor
    RPG();
    // destructor
    ~RPG();

    void initialize(HWND hwnd);
    void update() override;
    void ai() override {};
    void collisions() override {};
    void render() override;
    void releaseAll();
    void resetAll();
    void run(HWND);
};
#pragma message("Including RPG.h from: " __FILE__)
#endif
