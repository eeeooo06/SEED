#ifndef _INPUT_H                                                                // 이 파일이 두 번 이상 포함될 경우 다중 정의를 방지합니다.
#define _INPUT_H                
#define WIN32_LEAN_AND_MEAN

class Input;

#include <windows.h>
#include <WindowsX.h>
#include <string>
#include <XInput.h>
#include "Constants.h"
#include "GameError.h"


// 고해상도 마우스용
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC      ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE     ((USHORT) 0x02)
#endif
//--------------------------

namespace inputNS
{
    const int KEYS_ARRAY_LEN = 256;                                             // 키 배열의 크기

    // clear()를 위한 값, 비트 플래그
    const UCHAR KEYS_DOWN = 1;
    const UCHAR KEYS_PRESSED = 2;
    const UCHAR MOUSE = 4;
    const UCHAR TEXT_IN = 8;
    const UCHAR KEYS_MOUSE_TEXT = KEYS_DOWN + KEYS_PRESSED + MOUSE + TEXT_IN;
}

const DWORD GAMEPAD_THUMBSTICK_DEADZONE = (DWORD)(0.20f * 0X7FFF);              // 기본적으로 범위의 20%를 데드존으로 설정
const DWORD GAMEPAD_TRIGGER_DEADZONE = 30;                                      // 트리거 범위 0-255
const DWORD MAX_CONTROLLERS = 4;                                                // XInput에서 지원하는 최대 컨트롤러 수

// state.Gamepad.wButtons의 게임패드 버튼에 해당하는 비트
const DWORD GAMEPAD_DPAD_UP = 0x0001;
const DWORD GAMEPAD_DPAD_DOWN = 0x0002;
const DWORD GAMEPAD_DPAD_LEFT = 0x0004;
const DWORD GAMEPAD_DPAD_RIGHT = 0x0008;
const DWORD GAMEPAD_START_BUTTON = 0x0010;
const DWORD GAMEPAD_BACK_BUTTON = 0x0020;
const DWORD GAMEPAD_LEFT_THUMB = 0x0040;
const DWORD GAMEPAD_RIGHT_THUMB = 0x0080;
const DWORD GAMEPAD_LEFT_SHOULDER = 0x0100;
const DWORD GAMEPAD_RIGHT_SHOULDER = 0x0200;
const DWORD GAMEPAD_A = 0x1000;
const DWORD GAMEPAD_B = 0x2000;
const DWORD GAMEPAD_X = 0x4000;
const DWORD GAMEPAD_Y = 0x8000;

struct ControllerState
{
    XINPUT_STATE        state;
    XINPUT_VIBRATION    vibration;
    float               vibrateTimeLeft;                                        // mSec
    float               vibrateTimeRight;                                       // mSec
    bool                connected;
};

class Input
{
private:
    bool keysDown[inputNS::KEYS_ARRAY_LEN];                                     // 지정된 키가 눌려 있으면 true
    bool keysPressed[inputNS::KEYS_ARRAY_LEN];                                  // 지정된 키가 눌렸었으면 true
    std::string textIn;                                                         // 사용자가 입력한 텍스트
    char charIn;                                                                // 마지막으로 입력된 문자
    bool newLine;                                                               // 새 줄이 시작되면 true
    int  mouseX, mouseY;                                                        // 마우스 화면 좌표
    int  mouseRawX, mouseRawY;                                                  // 고해상도 마우스 데이터
    RAWINPUTDEVICE Rid[1];                                                      // 고해상도 마우스용
    bool mouseCaptured;                                                         // 마우스가 캡처되었으면 true
    bool mouseLButton;                                                          // 마우스 왼쪽 버튼이 눌려 있으면 true
    bool mouseMButton;                                                          // 마우스 가운데 버튼이 눌려 있으면 true
    bool mouseRButton;                                                          // 마우스 오른쪽 버튼이 눌려 있으면 true
    bool mouseX1Button;                                                         // X1 마우스 버튼이 눌려 있으면 true
    bool mouseX2Button;                                                         // X2 마우스 버튼이 눌려 있으면 true
    ControllerState controllers[MAX_CONTROLLERS];                               // 컨트롤러 상태

public:
    // 생성자
    Input();

    // 소멸자
    virtual ~Input();

    // 마우스 및 컨트롤러 입력을 초기화합니다.
    // GameError를 발생시킬 수 있음
    // 사전 조건: 
    //          hwnd = 윈도우 핸들
    //          capture = true이면 마우스를 캡처합니다.
    void initialize(HWND hwnd, bool capture);

    // 키 눌림 상태 저장
    void keyDown(WPARAM);

    // 키 떼임 상태 저장
    void keyUp(WPARAM);

    // textIn 문자열에 방금 입력된 문자 저장
    void keyIn(WPARAM);

    // 지정된 가상 키가 눌려 있으면 true를, 그렇지 않으면 false를 반환합니다.
    bool isKeyDown(UCHAR vkey) const;

    // 가장 최근 프레임에서 지정된 가상 키가 눌렸으면 true를 반환합니다.
    // 키 입력은 매 프레임 끝에 지워집니다.
    bool wasKeyPressed(UCHAR vkey) const;

    // 가장 최근 프레임에서 아무 키나 눌렸으면 true를 반환합니다.
    // 키 입력은 매 프레임 끝에 지워집니다.
    bool anyKeyPressed() const;

    // 지정된 키 입력 지우기
    void clearKeyPress(UCHAR vkey);

    // 지정된 입력 버퍼를 지웁니다. 여기서 what은 다음의 조합입니다.
    // KEYS_DOWN, KEYS_PRESSED, MOUSE, TEXT_IN 또는 KEYS_MOUSE_TEXT.
    // OR '|' 연산자를 사용하여 매개변수를 조합하세요.
    void clear(UCHAR what);

    // 키, 마우스 및 텍스트 입력 데이터 지우기
    void clearAll() { clear(inputNS::KEYS_MOUSE_TEXT); }

    // 텍스트 입력 버퍼 지우기
    void clearTextIn() { textIn.clear(); }

    // 텍스트 입력을 문자열로 반환
    std::string getTextIn() { return textIn; }

    // 마지막으로 입력된 문자 반환
    char getCharIn() { char c = charIn; charIn = 0; return c; }

    // 마우스 화면 위치를 mouseX, mouseY로 읽어들입니다.
    void mouseIn(LPARAM);

    // 원시 마우스 데이터를 mouseRawX, mouseRawY로 읽어들입니다.
    // 이 루틴은 고해상도 마우스를 지원합니다.
    void mouseRawIn(LPARAM);

    // 마우스 버튼 상태 저장
    void setMouseLButton(bool b) { mouseLButton = b; }

    // 마우스 버튼 상태 저장
    void setMouseMButton(bool b) { mouseMButton = b; }

    // 마우스 버튼 상태 저장
    void setMouseRButton(bool b) { mouseRButton = b; }

    // 마우스 버튼 상태 저장
    void setMouseXButton(WPARAM wParam) {
        mouseX1Button = (wParam & MK_XBUTTON1) ? true : false;
        mouseX2Button = (wParam & MK_XBUTTON2) ? true : false;
    }
    // 마우스 X 위치 반환
    int  getMouseX()        const { return mouseX; }

    // 마우스 Y 위치 반환
    int  getMouseY()        const { return mouseY; }

    // 원시 마우스 X 움직임 반환. 왼쪽 < 0, 오른쪽 > 0
    // 고해상도 마우스를 지원합니다.
    int  getMouseRawX()     const { return mouseRawX; }

    // 원시 마우스 Y 움직임 반환. 위쪽 < 0, 아래쪽 > 0
    // 고해상도 마우스를 지원합니다.
    int  getMouseRawY()     const { return mouseRawY; }

    // 마우스 왼쪽 버튼 상태 반환
    bool getMouseLButton()  const { return mouseLButton; }

    // 마우스 가운데 버튼 상태 반환
    bool getMouseMButton()  const { return mouseMButton; }

    // 마우스 오른쪽 버튼 상태 반환
    bool getMouseRButton()  const { return mouseRButton; }

    // X1 마우스 버튼 상태 반환
    bool getMouseX1Button() const { return mouseX1Button; }

    // X2 마우스 버튼 상태 반환
    bool getMouseX2Button() const { return mouseX2Button; }

    // 게임 컨트롤러의 연결 상태를 업데이트합니다.
    void checkControllers();

    // 연결된 게임 컨트롤러로부터 입력을 저장합니다.
    void readControllers();

    // 지정된 게임 컨트롤러의 상태를 반환합니다.
    const ControllerState* getControllerState(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return &controllers[n];
    }

    // 컨트롤러 n의 버튼 상태를 반환합니다.
    const WORD getGamepadButtons(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return controllers[n].state.Gamepad.wButtons;
    }

    // 컨트롤러 n의 D-패드 위쪽 상태를 반환합니다.
    bool getGamepadDPadUp(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_DPAD_UP) != 0);
    }

    // 컨트롤러 n의 D-패드 아래쪽 상태를 반환합니다.
    bool getGamepadDPadDown(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_DPAD_DOWN) != 0);
    }

    // 컨트롤러 n의 D-패드 왼쪽 상태를 반환합니다.
    bool getGamepadDPadLeft(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_DPAD_LEFT) != 0);
    }

    // 컨트롤러 n의 D-패드 오른쪽 상태를 반환합니다.
    bool getGamepadDPadRight(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return bool((controllers[n].state.Gamepad.wButtons & GAMEPAD_DPAD_RIGHT) != 0);
    }

    // 컨트롤러 n의 시작 버튼 상태를 반환합니다.
    bool getGamepadStart(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return bool((controllers[n].state.Gamepad.wButtons & GAMEPAD_START_BUTTON) != 0);
    }

    // 컨트롤러 n의 뒤로가기 버튼 상태를 반환합니다.
    bool getGamepadBack(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return bool((controllers[n].state.Gamepad.wButtons & GAMEPAD_BACK_BUTTON) != 0);
    }

    // 컨트롤러 n의 왼쪽 스틱 버튼 상태를 반환합니다.
    bool getGamepadLeftThumb(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return bool((controllers[n].state.Gamepad.wButtons & GAMEPAD_LEFT_THUMB) != 0);
    }

    // 컨트롤러 n의 오른쪽 스틱 버튼 상태를 반환합니다.
    bool getGamepadRightThumb(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return bool((controllers[n].state.Gamepad.wButtons & GAMEPAD_RIGHT_THUMB) != 0);
    }

    // 컨트롤러 n의 왼쪽 숄더 버튼 상태를 반환합니다.
    bool getGamepadLeftShoulder(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return bool((controllers[n].state.Gamepad.wButtons & GAMEPAD_LEFT_SHOULDER) != 0);
    }

    // 컨트롤러 n의 오른쪽 숄더 버튼 상태를 반환합니다.
    bool getGamepadRightShoulder(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return bool((controllers[n].state.Gamepad.wButtons & GAMEPAD_RIGHT_SHOULDER) != 0);
    }

    // 컨트롤러 n의 A 버튼 상태를 반환합니다.
    bool getGamepadA(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return bool((controllers[n].state.Gamepad.wButtons & GAMEPAD_A) != 0);
    }

    // 컨트롤러 n의 B 버튼 상태를 반환합니다.
    bool getGamepadB(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return bool((controllers[n].state.Gamepad.wButtons & GAMEPAD_B) != 0);
    }

    // 컨트롤러 n의 X 버튼 상태를 반환합니다.
    bool getGamepadX(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return bool((controllers[n].state.Gamepad.wButtons & GAMEPAD_X) != 0);
    }

    // 컨트롤러 n의 Y 버튼 상태를 반환합니다.
    bool getGamepadY(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return bool((controllers[n].state.Gamepad.wButtons & GAMEPAD_Y) != 0);
    }

    // 컨트롤러 n의 왼쪽 트리거 값을 반환합니다.
    BYTE getGamepadLeftTrigger(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return (controllers[n].state.Gamepad.bLeftTrigger);
    }

    // 컨트롤러 n의 오른쪽 트리거 값을 반환합니다.
    BYTE getGamepadRightTrigger(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return (controllers[n].state.Gamepad.bRightTrigger);
    }

    // 컨트롤러 n의 왼쪽 스틱 X축 값을 반환합니다.
    SHORT getGamepadThumbLX(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return (controllers[n].state.Gamepad.sThumbLX);
    }

    // 컨트롤러 n의 왼쪽 스틱 Y축 값을 반환합니다.
    SHORT getGamepadThumbLY(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return (controllers[n].state.Gamepad.sThumbLY);
    }

    // 컨트롤러 n의 오른쪽 스틱 X축 값을 반환합니다.
    SHORT getGamepadThumbRX(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return (controllers[n].state.Gamepad.sThumbRX);
    }

    // 컨트롤러 n의 오른쪽 스틱 Y축 값을 반환합니다.
    SHORT getGamepadThumbRY(UINT n)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        return (controllers[n].state.Gamepad.sThumbRY);
    }

    // 컨트롤러 n의 왼쪽 모터를 진동시킵니다.
    // 왼쪽은 저주파 진동입니다.
    // 속도 0=꺼짐, 65536=100%
    // sec는 진동 시간(초)입니다.
    void gamePadVibrateLeft(UINT n, WORD speed, float sec)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        controllers[n].vibration.wLeftMotorSpeed = speed;
        controllers[n].vibrateTimeLeft = sec;
    }

    // 컨트롤러 n의 오른쪽 모터를 진동시킵니다.
    // 오른쪽은 고주파 진동입니다.
    // 속도 0=꺼짐, 65536=100%
    // sec는 진동 시간(초)입니다.
    void gamePadVibrateRight(UINT n, WORD speed, float sec)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        controllers[n].vibration.wRightMotorSpeed = speed;
        controllers[n].vibrateTimeRight = sec;
    }

    // 연결된 컨트롤러를 원하는 시간 동안 진동시킵니다.
    void vibrateControllers(float frameTime);
};

#endif