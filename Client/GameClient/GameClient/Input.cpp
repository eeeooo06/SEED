#include "Input.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Input::Input()
{
    // Ű  迭 ʱȭ
    for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
        keysDown[i] = false;
    // Ű Է 迭 ʱȭ
    for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
        keysPressed[i] = false;
    newLine = true;                                                          //   
    textIn = "";                                                             // textIn ʱȭ
    charIn = 0;                                                              // charIn ʱȭ

    // 콺                                                            
    mouseX = 0;                                                              // ȭ X
    mouseY = 0;                                                              // ȭ Y
    mouseRawX = 0;                                                           // ػ X
    mouseRawY = 0;                                                           // ػ Y
    mouseLButton = false;                                                    // 콺  ư  true
    mouseMButton = false;                                                    // 콺  ư  true
    mouseRButton = false;                                                    // 콺  ư  true
    mouseX1Button = false;                                                   // X1 콺 ư  true
    mouseX2Button = false;                                                   // X2 콺 ư  true

    for (int i = 0; i < MAX_CONTROLLERS; i++)
    {
        controllers[i].vibrateTimeLeft = 0;
        controllers[i].vibrateTimeRight = 0;
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
// Ҹ                                                                     
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
Input::~Input()
{
    if (mouseCaptured)
        ReleaseCapture();                                                    // 콺 
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 콺  Ʈѷ Է ʱȭ
// 콺 ĸóϷ capture=true 
// GameError ߻ų  
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Input::initialize(HWND hwnd, bool capture)
{
    try {
        mouseCaptured = capture;

        // ػ 콺 
        Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
        Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
        Rid[0].dwFlags = RIDEV_INPUTSINK;
        Rid[0].hwndTarget = hwnd;
        RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

        if (mouseCaptured)
            SetCapture(hwnd);                                                // 콺 ĸó

        // Ʈѷ  ʱȭ
        ZeroMemory(controllers, sizeof(ControllerState) * MAX_CONTROLLERS);

        checkControllers();                                                  //  Ʈѷ Ȯ
    }
    catch (...)
    {
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing input system"));
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
//  Ű  keysDown  keysPressed 迭 true 
//  : 
//          wParam  Ű ڵ(0-255) մϴ.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Input::keyDown(WPARAM wParam)
{
    // Ű ڵ尡   ִ Ȯ
    if (wParam < inputNS::KEYS_ARRAY_LEN)
    {
        keysDown[wParam] = true;                                             // keysDown 迭 Ʈ
        // Ű Ƚϴ, clear()  ϴ.                           
        keysPressed[wParam] = true;                                          // keysPressed 迭 Ʈ
    }

    if (wParam == BACKSPACE_KEY)                                             // 齺̽ 
    {
        if (textIn.length() > 0)
            textIn.erase(textIn.size() - 1);
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
//  Ű  keysDown 迭 false                                     
//  : 
//          wParam  Ű ڵ(0-255) մϴ.                          
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
void Input::keyUp(WPARAM wParam)
{
    // Ű ڵ尡    ִ Ȯ                                      
    if (wParam < inputNS::KEYS_ARRAY_LEN)
        //  ̺ Ʈ                                                 
        keysDown[wParam] = false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
// textIn ڿ  Էµ                                           
//  : 
//          wParam ڸ մϴ.                                        
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
void Input::keyIn(WPARAM wParam)
{
    if (newLine)                                                             //    
    {
        textIn.clear();
        newLine = false;
    }

    textIn += wParam;                                                    // textIn  ߰
    charIn = wParam;                                                     //  Էµ  

    if ((char)wParam == '\r')                                                //  Ű 
        newLine = true;                                                      //   
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
//  Ű   true, ׷  false ȯմϴ.           
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
bool Input::isKeyDown(UCHAR vkey) const
{
    if (vkey < inputNS::KEYS_ARRAY_LEN)
        return keysDown[vkey];
    else
        return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
//  ֱ ӿ   Ű  true ȯմϴ.                 
// . Ű Է    ϴ.                                   
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
bool Input::wasKeyPressed(UCHAR vkey) const
{
    if (vkey < inputNS::KEYS_ARRAY_LEN)
        return keysPressed[vkey];
    else
        return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
//  ֱ ӿ ƹ Ű  true ȯմϴ.                       
// Ű Է    ϴ.                                          
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
bool Input::anyKeyPressed() const
{
    for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
        if (keysPressed[i] == true)
            return true;
    return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
//  Ű Է                                                         
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
void Input::clearKeyPress(UCHAR vkey)
{
    if (vkey < inputNS::KEYS_ARRAY_LEN)
        keysPressed[vkey] = false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
//  Է                                                        
//   input.h                                                   
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
void Input::clear(UCHAR what)
{
    if (what & inputNS::KEYS_DOWN)                                           // Ű  ¸  
    {
        for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
            keysDown[i] = false;
    }
    if (what & inputNS::KEYS_PRESSED)                                        // Ű Է ¸  
    {
        for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
            keysPressed[i] = false;
    }
    if (what & inputNS::MOUSE)                                               // 콺 ¸  
    {
        mouseX = 0;
        mouseY = 0;
        mouseRawX = 0;
        mouseRawY = 0;
    }
    if (what & inputNS::TEXT_IN)
        clearTextIn();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 콺 ȭ ġ mouseX, mouseY оԴϴ.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Input::mouseIn(LPARAM lParam)
{
    mouseX = GET_X_LPARAM(lParam);
    mouseY = GET_Y_LPARAM(lParam);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
//  콺 ͸ mouseRawX, mouseRawY оԴϴ.
//  ƾ ػ 콺 մϴ.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Input::mouseRawIn(LPARAM lParam)
{
    UINT dwSize = 40;
    static BYTE lpb[40];

    GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
        lpb, &dwSize, sizeof(RAWINPUTHEADER));

    RAWINPUT* raw = (RAWINPUT*)lpb;

    if (raw->header.dwType == RIM_TYPEMOUSE)
    {
        mouseRawX = raw->data.mouse.lLastX;
        mouseRawY = raw->data.mouse.lLastY;
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
//  Ʈѷ Ȯ
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Input::checkControllers()
{
    DWORD result;
    for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
    {
        result = XInputGetState(i, &controllers[i].state);
        if (result == ERROR_SUCCESS)
            controllers[i].connected = true;
        else
            controllers[i].connected = false;
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
//  Ʈѷ  б
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Input::readControllers()
{
    DWORD result;
    for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
    {
        if (controllers[i].connected)
        {
            result = XInputGetState(i, &controllers[i].state);
            if (result == ERROR_DEVICE_NOT_CONNECTED)                        // Ʈѷ   
                controllers[i].connected = false;
        }
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
//  Ʈѷ 
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Input::vibrateControllers(float frameTime)
{
    for (int i = 0; i < MAX_CONTROLLERS; i++)
    {
        if (controllers[i].connected)
        {
            controllers[i].vibrateTimeLeft -= frameTime;
            if (controllers[i].vibrateTimeLeft < 0)
            {
                controllers[i].vibrateTimeLeft = 0;
                controllers[i].vibration.wLeftMotorSpeed = 0;
            }
            controllers[i].vibrateTimeRight -= frameTime;
            if (controllers[i].vibrateTimeRight < 0)
            {
                controllers[i].vibrateTimeRight = 0;
                controllers[i].vibration.wRightMotorSpeed = 0;
            }
            XInputSetState(i, &controllers[i].vibration);
        }
    }
}
