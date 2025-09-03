#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Xinput9_1_0.lib")

#include "Game.h"
#include <Xinput.h>

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 생성자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Game::Game()
{
    input = new Input();													// 키보드 입력을 즉시 초기화한다.
    // 추가 초기화는 나중에 input->initialize() 호출에서 처리
    paused = false;															// 게임이 일시 정지되지 않았다.
    audio = NULL;
    console = NULL;
    graphics = NULL;
    messageDialog = NULL;
    inputDialog = NULL;

    fps = 100;
    fpsOn = false;															// 기본값을 FPS 표시 꺼짐으로 설정한다.

    initialized = false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 소멸자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Game::~Game()
{
    deleteAll();															// 예약된 메모리를 모두 해제한다.
    ShowCursor(true);														// 마우스 커서를 화면에 보이게 한다.
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 윈도우 메시지 핸들러
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
LRESULT Game::messageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (initialized)     // 초기화되지 않았다면 메시지를 처리하지 않는다.
    {
        switch (msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);        // Windows에 이 프로그램을 종료하라고 알린다.
            return 0;
        case WM_KEYDOWN: case WM_SYSKEYDOWN:                                // 키 다운
            input->keyDown(wParam);
            return 0;
        case WM_KEYUP: case WM_SYSKEYUP:                                    // 키 업
            input->keyUp(wParam);
            return 0;
        case WM_CHAR:                                                       // 문자열 입력
            input->keyIn(wParam);
            return 0;
        case WM_MOUSEMOVE:                                                  // 마우스 이동
            input->mouseIn(lParam);
            return 0;
        case WM_INPUT:                                                      // 마우스 원시 입력
            input->mouseRawIn(lParam);
            return 0;
        case WM_LBUTTONDOWN:                                                // 왼쪽 마우스 버튼 눌림
            input->setMouseLButton(true);
            input->mouseIn(lParam);                                         // 마우스 좌표
            return 0;
        case WM_LBUTTONUP:                                                  // 왼쪽 마우스 버튼 뗌
            input->setMouseLButton(false);
            input->mouseIn(lParam);                                         // 마우스 좌표
            return 0;
        case WM_MBUTTONDOWN:                                                // 가운데 마우스 버튼 눌림
            input->setMouseMButton(true);
            input->mouseIn(lParam);                                         // 마우스 좌표
            return 0;
        case WM_MBUTTONUP:                                                  // 가운데 마우스 버튼 뗌
            input->setMouseMButton(false);
            input->mouseIn(lParam);                                         // 마우스 좌표
            return 0;
        case WM_RBUTTONDOWN:                                                // 오른쪽 마우스 버튼 눌림
            input->setMouseRButton(true);
            input->mouseIn(lParam);                                         // 마우스 좌표
            return 0;
        case WM_RBUTTONUP:                                                  // 오른쪽 마우스 버튼 뗌
            input->setMouseRButton(false);
            input->mouseIn(lParam);                                         // 마우스 좌표
            return 0;
        case WM_XBUTTONDOWN: case WM_XBUTTONUP:                             // 마우스 X버트 눌림/뗌
            input->setMouseXButton(wParam);
            input->mouseIn(lParam);                                         // 마우스 좌표
            return 0;
        case WM_DEVICECHANGE:                                               // 컨트롤러가 연결되었는지 확인
            input->checkControllers();
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);    // Windows에 처리 맡기기
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 게임을 초기화한다.
// 에러 발생 시 GameError를 던진다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Game::initialize(HWND hw)
{
    hwnd = hw;                                                              // 윈도우 핸들을 저장한다.

    // 그래픽 초기화한다.
    graphics = new Graphics();
    // GameError를 던진다.
    graphics->initialize(hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN);

    // 입력을 초기화하고, 마우스를 캡처하지 않는다
    input->initialize(hwnd, false);                                         // GameError를 던진다.


    // 콘솔 초기화한다.
    console = new Console();
    console->initialize(graphics, input);                                   // 콘솔을 준비한다.
    console->print("---Console---");

    // 메시지 다이얼로그를 초기화한다.
    messageDialog = new MessageDialog();
    messageDialog->initialize(graphics, input, hwnd);

    // 입력 다이얼로그를 초기화한다.
    inputDialog = new InputDialog();
    inputDialog->initialize(graphics, input, hwnd);

    // DirectX Font를 초기화한다.
    if (dxFont.initialize(graphics, GameNS::FONT_SIZE, false, false, GameNS::FONT) == false)
        throw (GameError(gameErrorNS::FATAL_ERROR, "Failed to initialize DirectX Font."));

    dxFont.setFontColor(GameNS::FONT_COLOR);

    // 사운드 시스템을 초기화한다.
    audio = new Audio();
    if (*WAVE_BANK != '\0' && *SOUND_BANK != '\0')                          // 사운드 파일이 정의되어 있다면
    {
        if (FAILED(hr = audio->initialize()))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                throw (GameError(gameErrorNS::FATAL_ERROR, "Failed to initialize sound system because media file not found."));
            else
                throw (GameError(gameErrorNS::FATAL_ERROR, "Failed to initialize sound system."));
        }
    }

    // 고해상도 타이머 설정을 시도한다.
    if (QueryPerformanceFrequency(&timerFreq) == false)
        throw (GameError(gameErrorNS::FATAL_ERROR, "Error initializing high resolution timer"));

    QueryPerformanceCounter(&timeStart);                                    // 시작 시간을 가져온다.

    initialized = true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 게임 아이템을 렌더링한다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Game::renderGame()
{
    const int BUF_SIZE = 20;
    static char buffer[BUF_SIZE];

    // 렌더링을 시작한다.
    if (SUCCEEDED(graphics->beginScene()))
    {
        render();                                                           // 파생 객체에서 render()를 호출한다.

        graphics->spriteBegin();                                            // 스프라이트 그리기 시작.
        if (fpsOn)                                                          // FPS 표시가 요청되었을 경우
        {
            // FPS를 문자열로 변환한다.
            _snprintf_s(buffer, BUF_SIZE, "fps %d", (int)fps);
            dxFont.print(buffer, GAME_WIDTH - 100, GAME_HEIGHT - 28);
        }
        graphics->spriteEnd();                                              // 스프라이트 그리기 종료.

        console->draw();                                                    // 콘솔을 여기에서 그려서 게임 위에 표시되도록 한다.

        messageDialog->draw();                                              // Dialog가 맨 위에 그려진다.
        inputDialog->draw();                                                // Dialog가 맨 위에 그려진다.

        // 렌더링을 종료한다.
        graphics->endScene();
    }
    handleLostGraphicsDevice();

    // 백 버퍼를 화면에 표시한다.
    graphics->showBackbuffer();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 손실된 그래픽 디바이스를 처리한다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Game::handleLostGraphicsDevice()
{
    // 손실된 디바이스를 검사하고 처리한다.
    hr = graphics->getDeviceState();
    if (FAILED(hr))                                                         // 그래픽 디바이스가 유효한 상태가 아니라면
    {
        // 디바이스가 손실되어 리셋할 수 없는 경우
        if (hr == D3DERR_DEVICELOST)
        {
            Sleep(100);                                                     // CPU 시간을 100밀리초 양보한다.
            return;
        }
        // 디바이스가 손실되었지만 이제 리셋 가능하다.
        else if (hr == D3DERR_DEVICENOTRESET)
        {
            releaseAll();
            hr = graphics->reset();                                         // 그래픽 디바이스 리셋을 시도한다.
            if (FAILED(hr))                                                 // 리셋이 실패했다면
                return;
            resetAll();
        }
        else
            return;                                                         // 기타 디바이스 오류
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 윈도우/전체 화면 전환
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Game::setDisplayMode(GraphicsNS::DISPLAY_MODE mode)
{
    releaseAll();                                                           // 사용자가 생성한 모든 surfaces를 해제한다.
    graphics->changeDisplayMode(mode);
    resetAll();                                                             // surfaces를 다시 생성한다.
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 윈도우/전체 화면 전환
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Game::run(HWND hwnd)
{
    if (graphics == NULL)                                                   // 그래픽이 초기화되지 않았다면
        return;

    // 마지막 프레임의 경과 시간을 계산하여 frameTime에 저장한다.
    QueryPerformanceCounter(&timeEnd);
    frameTime = (float)(timeEnd.QuadPart - timeStart.QuadPart) / (float)timerFreq.QuadPart;

    // 전력 절감 코드, winmm.lib가 필요하다.
    // 목표 프레임 속도를 유지하기에 충분한 시간이 경과하지 않았다면
    if (frameTime < MIN_FRAME_TIME)
    {
        sleepTime = (DWORD)((MIN_FRAME_TIME - frameTime) * 1000);
        timeBeginPeriod(1);                                                 // Windows 타이머에 1ms 해상도를 요청한다.
        Sleep(sleepTime);                                                   // CPU를 sleepTime 동안 해제한다.
        timeEndPeriod(1);                                                   // 1ms 타이머 해상도를 종료한다.
        return;
    }

    if (frameTime > 0.0)
        fps = (fps * 0.99f) + (0.01f / frameTime);                          // 평균 FPS
    if (frameTime > MAX_FRAME_TIME)                                         // 프레임 속도가 매우 느릴 경우
        frameTime = MAX_FRAME_TIME;                                         // 최대 frameTime을 제한한다.
    timeStart = timeEnd;

    // update(), ai(), collisions() 함수는 순수 가상 함수이다.
    // 이 함수들은 Game 클래스를 상속받는 클래스에서 반드시 구현해야 한다.
    if (!paused)                                                            // 일시 정지되지 않았다면
    {
        update();                                                           // 모든 게임 아이템을 업데이트한다.
        ai();                                                               // 인공지능
        collisions();                                                       // 충돌을 처리한다.
        input->vibrateControllers(frameTime);                               // 컨트롤러 진동을 처리한다.
    }
    renderGame();                                                           // 모든 게임 아이템을 그린다.

    //communicate(frameTime);                                                 // 네트워크 통신을 수행한다.

    // 콘솔 키 입력을 확인한다.
    if (input->wasKeyPressed(CONSOLE_KEY))
    {
        console->showHide();
        // paused = console->getVisible();                                  // 콘솔이 표시되면 게임을 일시 정지한다.
    }
    consoleCommand();                                                       // 사용자가 입력한 콘솔 명령을 처리한다.

    input->readControllers();                                               // 컨트롤러의 상태를 읽는다.

    messageDialog->update();
    inputDialog->update();

    audio->run();                                                           // 주기적인 사운드 엔진 작업을 수행한다.

    // Alt+Enter 입력 시 전체화면/창 모드를 전환한다.
    if (input->isKeyDown(ALT_KEY) && input->wasKeyPressed(ENTER_KEY))
        setDisplayMode(GraphicsNS::TOGGLE);                                 // 전체화면과 창 모드를 전환한다.

    // Esc 키를 누르면 창 모드로 설정한다.
    if (input->isKeyDown(ESC_KEY))
        setDisplayMode(GraphicsNS::WINDOW);                                 // 창 모드로 설정한다.

    // 입력을 초기화한다.
    // 모든 키 체크가 끝난 후에 이 함수를 호출한다.
    input->clear(inputNS::KEYS_PRESSED);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 콘솔 명령을 처리한다.
// 새로운 콘솔 명령을 추가한다면, 파생 클래스에서 이 함수를 오버라이드한다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Game::consoleCommand()
{
    command = console->getCommand();                                        // 콘솔에서 명령을 가져온다.
    if (command == "")                                                      // 명령이 없을 경우
        return;

    if (command == "help")                                                  // "help" 명령일 경우
    {
        console->print("Console Commands : ");
        console->print("fps - toggle display of frames per seconds");
        return;
    }

    if (command == "fps")
    {
        fpsOn = !fpsOn;                                                     // fps 표시를 전환한다.
        if (fpsOn)
            console->print("fpsOn");
        else
            console->print("fpsOff");
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 그래픽 디바이스가 손실되었다.
// 그래픽 디바이스를 리셋할 수 있도록 예약된 모든 비디오 메모리를 해제한다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Game::releaseAll()
{
    SAFE_ON_LOST_DEVICE(inputDialog);
    SAFE_ON_LOST_DEVICE(messageDialog);
    SAFE_ON_LOST_DEVICE(console);
    dxFont.onLostDevice();
    return;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 모든 surfaces를 재생성하고 모든 엔티티를 리셋한다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Game::resetAll()
{
    dxFont.onResetDevice();
    SAFE_ON_RESET_DEVICE(console);
    SAFE_ON_RESET_DEVICE(messageDialog);
    SAFE_ON_RESET_DEVICE(inputDialog);
    return;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 예약된 모든 메모리를 삭제한다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Game::deleteAll()
{
    releaseAll();                                                           // 모든 그래픽 아이템에 대해 onLostDevice()를 호출한다.
    SAFE_DELETE(audio);
    SAFE_DELETE(graphics);
    SAFE_DELETE(input);
    SAFE_DELETE(console);
    SAFE_DELETE(messageDialog);
    SAFE_DELETE(inputDialog);
    initialized = false;
}