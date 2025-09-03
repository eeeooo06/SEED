#define _CRTDBG_MAP_ALLOC                                                   // �޸� ���� ������ ����
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdlib.h>                                                         // �޸� ���� ������ ����
#include <crtdbg.h>                                                         // �޸� ���� ������ ����
#include "RPG.h"

// �Լ� ������Ÿ��
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool CreateMainWindow(HWND&, HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);

// ���� ������
RPG* rpgGame = NULL;
HWND hwnd = NULL;

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// Windows ���ø����̼��� ������
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    // ����� ���� �� �޸� ���� Ȯ��
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    MSG msg;

    // ������ �����ϰ� �޽��� �ڵ鷯�� �����մϴ�.
    rpgGame = new RPG;

    // �����츦 �����մϴ�.
    if (!CreateMainWindow(hwnd, hInstance, nCmdShow))
        return 1;

    try {
        rpgGame->initialize(hwnd);                                             // GameError�� �߻���ų �� ����

        // ���� �޽��� ����
        int done = 0;
        while (!done)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                // ���� �޽��� Ȯ��
                if (msg.message == WM_QUIT)
                    done = 1;

                // �޽����� ���ڵ��Ͽ� WinProc���� ����
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
                rpgGame->run(hwnd);                                            // ���� ���� ����
        }
        SAFE_DELETE(rpgGame);                                                  // ���� �� �޸� ����
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

    SAFE_DELETE(rpgGame);                                                      // ���� �� �޸� ����
    return 0;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ������ �̺�Ʈ �ݹ� �Լ�
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return (rpgGame->messageHandler(hwnd, msg, wParam, lParam));
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ������ ����
// ��ȯ: 
//      ���� �� false
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool CreateMainWindow(HWND& hwnd, HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wcx;

    // ���� �����츦 �����ϴ� �Ű������� ������ Ŭ���� ����ü�� ä��ϴ�.
    wcx.cbSize = sizeof(wcx);                                               // ����ü ũ��
    wcx.style = CS_HREDRAW | CS_VREDRAW;                                    // ũ�Ⱑ ����Ǹ� �ٽ� �׸��ϴ�.
    wcx.lpfnWndProc = WinProc;                                              // ������ ���ν����� ����ŵ�ϴ�.
    wcx.cbClsExtra = 0;                                                     // �߰� Ŭ���� �޸� ����
    wcx.cbWndExtra = 0;                                                     // �߰� ������ �޸� ����
    wcx.hInstance = hInstance;                                              // �ν��Ͻ� �ڵ�
    wcx.hIcon = NULL;
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);                              // �̸� ���ǵ� ȭ��ǥ
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);                // ������ ���
    wcx.lpszMenuName = NULL;                                                // �޴� ���ҽ� �̸�
    wcx.lpszClassName = CLASS_NAME;                                         // ������ Ŭ���� �̸�
    wcx.hIconSm = NULL;                                                     // ���� Ŭ���� ������

    // ������ Ŭ������ ����մϴ�. RegisterClassEx�� ���� �߻� �� 0�� ��ȯ�մϴ�.
    if (RegisterClassEx(&wcx) == 0)                                         // ���� �߻� ��
        return false;

    // ȭ���� â ��� �Ǵ� ��ü ȭ�� ���� �����Ͻðڽ��ϱ�?
    DWORD style;
    if (FULLSCREEN)
        style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
    else
        style = WS_OVERLAPPEDWINDOW;

    // ������ ����
    hwnd = CreateWindow(
        CLASS_NAME,                                                         // ������ Ŭ������ �̸�
        GAME_TITLE,                                                         // ���� ǥ���� �ؽ�Ʈ
        style,                                                              // ������ ��Ÿ��
        CW_USEDEFAULT,                                                      // �������� �⺻ ���� ��ġ
        CW_USEDEFAULT,                                                      // �������� �⺻ ���� ��ġ
        GAME_WIDTH,                                                         // ������ �ʺ�
        GAME_HEIGHT,                                                        // ������ ����
        (HWND)NULL,                                                         // �θ� ������ ����
        (HMENU)NULL,                                                        // �޴� ����
        hInstance,                                                          // ���ø����̼� �ν��Ͻ� �ڵ�
        (LPVOID)NULL);                                                      // ������ �Ű����� ����

    // ������ ���� �� ������ �߻��� ���
    if (!hwnd)
        return false;

    if (!FULLSCREEN)                                                        // â ����� ���
    {
        // Ŭ���̾�Ʈ ������ GAME_WIDTH x GAME_HEIGHT�� �ǵ��� ������ ũ�⸦ �����մϴ�.
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);                                   // �������� Ŭ���̾�Ʈ ���� ũ�⸦ �����ɴϴ�.
        MoveWindow(hwnd,
            0,                                                              // ����
            0,                                                              // ����
            GAME_WIDTH + (GAME_WIDTH - clientRect.right),                   // ������
            GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom),                // �Ʒ���
            TRUE);                                                          // �����츦 �ٽ� �׸��ϴ�.
    }

    // �����츦 ǥ���մϴ�.
    ShowWindow(hwnd, nCmdShow);

    return true;
}