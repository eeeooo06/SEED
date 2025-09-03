#define _CRTDBG_MAP_ALLOC                                                   // 메모리 누수 감지를 위해
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdlib.h>                                                         // 메모리 누수 감지를 위해
#include <crtdbg.h>                                                         // 메모리 누수 감지를 위해
#include "RPG.h"

// 함수 프로토타입
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool CreateMainWindow(HWND&, HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);

// 게임 포인터
RPG* rpgGame = NULL;
HWND hwnd = NULL;

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// Windows 애플리케이션의 시작점
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    // 디버그 빌드 시 메모리 누수 확인
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    MSG msg;

    // 게임을 생성하고 메시지 핸들러를 설정합니다.
    rpgGame = new RPG;

    // 윈도우를 생성합니다.
    if (!CreateMainWindow(hwnd, hInstance, nCmdShow))
        return 1;

    try {
        rpgGame->initialize(hwnd);                                             // GameError를 발생시킬 수 있음

        // 메인 메시지 루프
        int done = 0;
        while (!done)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                // 종료 메시지 확인
                if (msg.message == WM_QUIT)
                    done = 1;

                // 메시지를 디코딩하여 WinProc으로 전달
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
                rpgGame->run(hwnd);                                            // 게임 루프 실행
        }
        SAFE_DELETE(rpgGame);                                                  // 종료 전 메모리 해제
        return msg.wParam;
    }
    catch (const GameError& err)
    {
        rpgGame->deleteAll();
        DestroyWindow(hwnd);
        MessageBox(NULL, err.getMessage(), "Error", MB_OK);
    }
    catch (...)
    {
        rpgGame->deleteAll();
        DestroyWindow(hwnd);
        MessageBox(NULL, "Unknown error occured in game.", "Error", MB_OK);
    }

    SAFE_DELETE(rpgGame);                                                      // 종료 전 메모리 해제
    return 0;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 윈도우 이벤트 콜백 함수
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return (rpgGame->messageHandler(hwnd, msg, wParam, lParam));
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 윈도우 생성
// 반환: 
//      오류 시 false
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool CreateMainWindow(HWND& hwnd, HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wcx;

    // 메인 윈도우를 설명하는 매개변수로 윈도우 클래스 구조체를 채웁니다.
    wcx.cbSize = sizeof(wcx);                                               // 구조체 크기
    wcx.style = CS_HREDRAW | CS_VREDRAW;                                    // 크기가 변경되면 다시 그립니다.
    wcx.lpfnWndProc = WinProc;                                              // 윈도우 프로시저를 가리킵니다.
    wcx.cbClsExtra = 0;                                                     // 추가 클래스 메모리 없음
    wcx.cbWndExtra = 0;                                                     // 추가 윈도우 메모리 없음
    wcx.hInstance = hInstance;                                              // 인스턴스 핸들
    wcx.hIcon = NULL;
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);                              // 미리 정의된 화살표
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);                // 검은색 배경
    wcx.lpszMenuName = NULL;                                                // 메뉴 리소스 이름
    wcx.lpszClassName = CLASS_NAME;                                         // 윈도우 클래스 이름
    wcx.hIconSm = NULL;                                                     // 작은 클래스 아이콘

    // 윈도우 클래스를 등록합니다. RegisterClassEx는 오류 발생 시 0을 반환합니다.
    if (RegisterClassEx(&wcx) == 0)                                         // 오류 발생 시
        return false;

    // 화면을 창 모드 또는 전체 화면 모드로 설정하시겠습니까?
    DWORD style;
    if (FULLSCREEN)
        style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
    else
        style = WS_OVERLAPPEDWINDOW;

    // 윈도우 생성
    hwnd = CreateWindow(
        CLASS_NAME,                                                         // 윈도우 클래스의 이름
        GAME_TITLE,                                                         // 제목 표시줄 텍스트
        style,                                                              // 윈도우 스타일
        CW_USEDEFAULT,                                                      // 윈도우의 기본 가로 위치
        CW_USEDEFAULT,                                                      // 윈도우의 기본 세로 위치
        GAME_WIDTH,                                                         // 윈도우 너비
        GAME_HEIGHT,                                                        // 윈도우 높이
        (HWND)NULL,                                                         // 부모 윈도우 없음
        (HMENU)NULL,                                                        // 메뉴 없음
        hInstance,                                                          // 애플리케이션 인스턴스 핸들
        (LPVOID)NULL);                                                      // 윈도우 매개변수 없음

    // 윈도우 생성 중 오류가 발생한 경우
    if (!hwnd)
        return false;

    if (!FULLSCREEN)                                                        // 창 모드인 경우
    {
        // 클라이언트 영역이 GAME_WIDTH x GAME_HEIGHT가 되도록 윈도우 크기를 조정합니다.
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);                                   // 윈도우의 클라이언트 영역 크기를 가져옵니다.
        MoveWindow(hwnd,
            0,                                                              // 왼쪽
            0,                                                              // 위쪽
            GAME_WIDTH + (GAME_WIDTH - clientRect.right),                   // 오른쪽
            GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom),                // 아래쪽
            TRUE);                                                          // 윈도우를 다시 그립니다.
    }

    // 윈도우를 표시합니다.
    ShowWindow(hwnd, nCmdShow);

    return true;
}