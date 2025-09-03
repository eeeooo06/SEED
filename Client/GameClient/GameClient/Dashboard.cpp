#include "Dashboard.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 막대 초기화
// 사전 조건: 
//          *graphics = Graphics 객체에 대한 포인터
//          *textureM = TextureManager 객체에 대한 포인터
//          left, top = 화면 위치
//          scale = 스케일링 (확대/축소) 양
//          color = 막대 색상
// 사후 조건: 
//          성공 시 true, 오류 시 false 반환
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool Bar::initialize(Graphics* graphics, TextureManager* textureM, int left,
    int top, float scale, COLOR_ARGB color)
{
    try {
        Image::initialize(graphics, dashboardNS::IMAGE_SIZE, dashboardNS::IMAGE_SIZE,
            dashboardNS::TEXTURE_COLS, textureM);
        setCurrentFrame(dashboardNS::BAR_FRAME);
        spriteData.x = (float)left;
        spriteData.y = (float)top;
        spriteData.scale = scale;
        colorFilter = color;
    }
    catch (...)
    {
        return false;
    }
    // 성공 반환
    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 막대 크기 설정
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Bar::set(float p)
{
    if (p < 0)
        p = 0;
    else if (p > 100)
        p = 100;
    spriteData.rect.right = spriteData.rect.left + (LONG)(spriteData.width * p / 100);
}