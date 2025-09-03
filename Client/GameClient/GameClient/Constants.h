#pragma once

#ifndef _CONSTANTS_H
#define _CONSTANTS_H
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
//                  매크로
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// 포인터가 가리키는 메모리를 안전하게 해제
#define SAFE_DELETE(ptr)       { if (ptr) { delete (ptr); (ptr)=NULL; } }
// COM 객체를 안전하게 해제
#define SAFE_RELEASE(ptr)      { if(ptr) { (ptr)->Release(); (ptr)=NULL; } }
// 포인터가 가리키는 배열을 안전하게 해제
#define SAFE_DELETE_ARRAY(ptr) { if(ptr) { delete [](ptr); (ptr)=NULL; } }
// onLostDevice를 안전하게 호출
#define SAFE_ON_LOST_DEVICE(ptr)    { if(ptr) { ptr->onLostDevice(); } }
// onResetDevice를 안전하게 호출
#define SAFE_ON_RESET_DEVICE(ptr)   { if(ptr) { ptr->onResetDevice(); } }
#define TRANSCOLOR  SETCOLOR_ARGB(0,255,0,255) // 투명 색상 (마젠타)

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
//                  
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// 
const char CLASS_NAME[] = "RPG";
const char GAME_TITLE[] = "RPG Client v1.0";
const bool FULLSCREEN = false;
const UINT GAME_WIDTH = 1280;
const UINT GAME_HEIGHT = 1024;

// 
const double PI = 3.14159265358979;
const double PIx2 = PI * 2.0;
const float FRAME_RATE = 200.0f;
const float MIN_FRAME_RATE = 10.0f;
const float MIN_FRAME_TIME = 1.0f / FRAME_RATE;
const float MAX_FRAME_TIME = 1.0f / MIN_FRAME_RATE;
const float FULL_HEALTH = 100;

// Path
const char NEBULA_IMAGE[] = "pictures\\orion.jpg";
const char TEXTURES_IMAGE[] = "pictures\\textures.png";
const char MENU_IMAGE[] = "pictures\\menu.png";
const char LoginBox_IMAGE[] = "pictures\\loginbox.png";

// audio.cpp 
// WAVE_BANK .xwb  
const char WAVE_BANK[] = "";
// SOUND_BANK .xsb  
const char SOUND_BANK[] = "";

// 

// Ű 
//  ӿ Ű ο   մϴ.  ߴٸ
//  Ͽ Ű  ϰ   ֽϴ.
const UCHAR CONSOLE_KEY = VK_OEM_3;
const UCHAR ESC_KEY = VK_ESCAPE;
const UCHAR ALT_KEY = VK_MENU;
const UCHAR ENTER_KEY = VK_RETURN;
const UCHAR BACKSPACE_KEY = VK_BACK;


// 
enum WEAPON { SWORD, STAFF, SHURIKEN };
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
//                  Player
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
namespace playerNS
{
    const int WIDTH = 64;
    const int HEIGHT = 64;
    const int TEXTURE_COLS = 8;
    const int START_FRAME = 0;
    const int END_FRAME = 7;
}

#endif