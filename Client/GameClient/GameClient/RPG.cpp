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

    // 로비 텍스처 초기화
    if (!lobbyTex.initialize(graphics, "pictures\\lobby.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing lobby texture"));

    // 로딩 텍스처 초기화
    if (!loadingTex.initialize(graphics, "pictures\\loading.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing loading texture"));

    //// 서버 리스트 텍스처 초기화
    //if (!serverlistTex.initialize(graphics, "pictures\\serverlist.png"))
    //    throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing serverlist texture"));

    // 메인 텍스처 초기화 (추후 map으로 변경)
    if (!mainTex.initialize(graphics, "pictures\\main.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing main texture"));

    // 게임 텍스처 초기화
    if (!gameTex.initialize(graphics, "pictures\\game.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing game texture"));

    // 로비 이미지 초기화
    if (!lobbyImg.initialize(graphics, 0, 0, 1, &lobbyTex))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing lobby image"));
    lobbyImg.setX(0.f); lobbyImg.setY(0.f);

    // 로딩 이미지 초기화
    if (!loadingImg.initialize(graphics, 0, 0, 1, &loadingTex))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing loading image"));
    loadingImg.setX(0.f); loadingImg.setY(0.f);

    //// 서버 리스트 이미지 초기화
    //if (!serverlistImg.initialize(graphics, 0, 0, 1, &serverlistTex))
    //    throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing serverlist image"));
    serverlistImg.setX(0.f); serverlistImg.setY(0.f);

    // 메인 이미지 초기화 (추후 map으로 변경)
    if (!mainImg.initialize(graphics, 0, 0, 1, &mainTex))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing main image"));
    mainImg.setX(0.f); mainImg.setY(0.f);

    // 게임 이미지 초기화
    if (!gameImg.initialize(graphics, 0, 0, 1, &gameTex))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing game image"));
    gameImg.setX(0.f); gameImg.setY(0.f);


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
    if (!dashboard.initialize(graphics, &gameTex, 0, 0, 10, D3DCOLOR_ARGB(255, 255, 0, 0)))
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
    if (scene) scene->update(frameTime);

    if (current == SceneList::Lobby) {
        if (auto* s = dynamic_cast<LobbyScene*>(scene.get())) {
            if (s->hasPending()) changeScene(SceneList::Loading);
        }
    }
    else if (current == SceneList::Loading) {
        if (auto* s = dynamic_cast<LoadingScene*>(scene.get())) {
            if (s->isDoneSuccess()) changeScene(SceneList::ServerList);   // ★ 반드시 ServerList
            else if (s->isFailed()) changeScene(SceneList::Lobby);
        }
    }
    else if (current == SceneList::ServerList) {
        auto* s = static_cast<ServerListScene*>(scene.get());
        if (s->consumeConfirmed()) {
            // 필요하다면 s->selectedServerId() / selectedIndex() 활용
            // TODO: 캐릭터 리스트 요청 or 월드 접속 시도
            changeScene(SceneList::Main);
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
    graphics->spriteBegin();

    // ▶ 공통 배경: 로비/로딩/서버리스트는 모두 같은 배경
    switch (current) {
    case SceneList::Lobby:
    case SceneList::Loading:
    case SceneList::ServerList:
        lobbyImg.draw();      // ← 여기만 그림
        break;

    case SceneList::Main:
        mainImg.draw();       // 메인은 기존대로
        break;
    }

    if (scene) scene->render();

    graphics->spriteEnd();
    graphics->endScene();
    graphics->showBackbuffer();
}

//=============================================================================
// 사용된 모든 리소스를 해제합니다.
//=============================================================================
void RPG::releaseAll()
{
    lobbyTex.onLostDevice();
    loadingTex.onLostDevice();
    serverlistTex.onLostDevice();
    mainTex.onLostDevice();
    gameTex.onLostDevice();
    Game::releaseAll();
    return;
}

//=============================================================================
// 디바이스 리셋 시 호출됩니다.
//=============================================================================
void RPG::resetAll()
{
    gameTex.onResetDevice();
    mainTex.onResetDevice();
    serverlistTex.onResetDevice();
    loadingTex.onResetDevice();
    lobbyTex.onResetDevice();
    Game::resetAll();
    return;
}

void RPG::changeScene(SceneList k)
{
    current = k;

    SceneDeps deps{};
    deps.g = graphics;
    deps.i = input;
    deps.atlas = &gameTex;
    deps.login = &login;
    deps.client = &client;

    switch (k) {
    case SceneList::Lobby:      scene = std::make_unique<LobbyScene>(deps);      break;
    case SceneList::Loading:    scene = std::make_unique<LoadingScene>(deps);    break;
    case SceneList::ServerList: scene = std::make_unique<ServerListScene>(deps); break;
    case SceneList::Main:       scene = std::make_unique<MainMapScene>(deps);    break;
    }
    scene->enter();
}