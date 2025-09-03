#include "RPG.h"

//=============================================================================
// Constructor
//=============================================================================
RPG::RPG()
{}

//=============================================================================
// Destructor
//=============================================================================
RPG::~RPG()
{
    //releaseAll();           // 모든 리소스를 해제합니다.
}

//=============================================================================
// 게임을 초기화합니다.
//=============================================================================
void RPG::initialize(HWND hwnd)
{
    Game::initialize(hwnd); // 부모 클래스의 초기화 함수를 호출합니다.

    // 메뉴 텍스처 초기화
    if (!menuTexture.initialize(graphics, "pictures\\menu.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menu texture"));

    // 게임 텍스처 초기화
    if (!gameTextures.initialize(graphics, "pictures\\textures.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing game textures"));

    // 메뉴 이미지 초기화
    if (!menu.initialize(graphics, 0, 0, 1, &menuTexture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menu image"));
    menu.setX(0.f); menu.setY(0.f);


    // 서버 접속 초기화
    if (!client.connectTo("127.0.0.1", 32000)) {  // 서버 포트 32000
        MessageBoxA(NULL, "Login server connect failed", "ERROR", MB_OK);
    }

    changeScene(SceneList::Lobby);

    // 플레이어 초기화
    //if (!player.initialize(this, playerNS::WIDTH, playerNS::HEIGHT, playerNS::TEXTURE_COLS, &gameTextures))
    //    throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing player"));
    //player.setFrames(playerNS::START_FRAME, playerNS::END_FRAME);
    //player.setCurrentFrame(playerNS::START_FRAME);
    //player.setX(GAME_WIDTH / 4);
    //player.setY(GAME_HEIGHT / 4);
    //player.setHealth(100);
    

    // 대시보드 초기화
    if (!dashboard.initialize(graphics, &gameTextures, 0, 0, 10, D3DCOLOR_ARGB(255, 255, 0, 0)))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing dashboard"));

    // 초기에는 메뉴 화면을 보여주기 위해 paused 상태로 설정
    setPaused(true);

    return;
}

//=============================================================================
// 게임 루프
//=============================================================================
void RPG::run(HWND hwnd)
{
    if (graphics == NULL)
        return;

    MSG msg;

    // 타이머 시작
    QueryPerformanceCounter(&timeStart);

    // 메인 게임 루프
    while (true)
    {
        // 윈도우 메시지 처리
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // 매 프레임마다 update와 render를 호출
            QueryPerformanceCounter(&timeEnd);
            frameTime = (float)(timeEnd.QuadPart - timeStart.QuadPart) / (float)timerFreq.QuadPart;

            if (frameTime < MIN_FRAME_TIME)
            {
                sleepTime = (DWORD)((MIN_FRAME_TIME - frameTime) * 1000);
                timeBeginPeriod(1);
                Sleep(sleepTime);
                timeEndPeriod(1);
            }
            timeStart = timeEnd;

            update();
            render();
        }
    }
}


//=============================================================================
// 게임 상태를 업데이트합니다.
//=============================================================================
void RPG::update()
{
    //if (getPaused()) // 메뉴 상태일 때
    //{
    //    if (input->anyKeyPressed())
    //    {
    //        setPaused(false);
    //        input->clearAll();
    //    }
    //}
    //else // 인게임 상태일 때
    //{
    //    //player.update(frameTime);
    //    dashboard.update(player.getHealth());

    //    if (input->isKeyDown(ESC_KEY))
    //        setPaused(true);
    //}
    // 씬 업데이트
    scene->update(frameTime);

    // 씬 전환 조건 (상태를 RPG가 감시)
    if (current == SceneList::Lobby) {
        auto* s = dynamic_cast<LobbyScene*>(scene.get());
        if (s && s->hasPending()) changeScene(SceneList::Loading);
    }
    else if (current == SceneList::Loading) {
        auto* s = dynamic_cast<LoadingScene*>(scene.get());
        if (s) {
            if (s->isDoneSuccess()) changeScene(SceneList::Main);
            else if (s->isFailed() || input->isKeyDown(ESC_KEY)) {
                login.reset(); changeScene(SceneList::Lobby);
            }
        }
    }
    else if (current == SceneList::Main) {
        if (input->isKeyDown(ESC_KEY)) {      // 로그아웃
            login.reset(); changeScene(SceneList::Lobby);
        }
    }
}

//=============================================================================
// 화면에 그래픽을 렌더링합니다.
//=============================================================================
void RPG::render()
{
    if (!graphics) return;
    
    graphics->beginScene();

    graphics->spriteBegin();                // 스프라이트 그리기 시작
    
    //if (getPaused()) // 메뉴 상태일 때
    //{
    //    menu.draw();
    //}
    //else // 인게임 상태일 때
    //{
    //    dashboard.draw();
    //    //player.draw();
    //}
    if (current == SceneList::Lobby || current == SceneList::Loading) {
        menu.draw();
    }
    scene->render();

    graphics->spriteEnd();                  // 스프라이트 그리기 종료

    graphics->endScene();
    graphics->showBackbuffer();
}

//=============================================================================
// 사용된 모든 리소스를 해제합니다.
//=============================================================================
void RPG::releaseAll()
{
    menuTexture.onLostDevice();
    gameTextures.onLostDevice();
    Game::releaseAll();
    return;
}

//=============================================================================
// 디바이스 리셋 시 호출됩니다.
//=============================================================================
void RPG::resetAll()
{
    gameTextures.onResetDevice();
    menuTexture.onResetDevice();
    Game::resetAll();
    return;
}