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
    TextureManager lobbyTex, loadingTex, serverlistTex;
    Image lobbyImg, loadingImg, serverlistImg;

    TextureManager mainTex, gameTex;
    Image mainImg, gameImg;


    Entity  player;       // 플레이어
    Bar dashboard;        // 대시보드


    // Scene Login Memer VArialbes
    LoginClient client;
    LoginService login;
    std::unique_ptr<IScene> scene;
    SceneList current = SceneList::Lobby;

    std::string loggedInId;




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

    void changeScene(SceneList k);
};
#pragma message("Including RPG.h from: " __FILE__)
#endif
