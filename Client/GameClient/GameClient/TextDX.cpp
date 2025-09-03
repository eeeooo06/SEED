#include "TextDX.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 생성자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
TextDX::TextDX()
{
    color = SETCOLOR_ARGB(255, 255, 255, 255);                               // 기본값은 흰색 글꼴

    // 글꼴 위치 설정
    fontRect.top = 0;
    fontRect.left = 0;
    fontRect.right = GAME_WIDTH;
    fontRect.bottom = GAME_HEIGHT;
    dxFont = NULL;
    angle = 0;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 소멸자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
TextDX::~TextDX()
{
    SAFE_RELEASE(dxFont);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// DirectX 글꼴 생성
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool TextDX::initialize(Graphics* g, int height, bool bold, bool italic,
    const std::string& fontName)
{
    graphics = g;                                                            // 그래픽 시스템

    UINT weight = FW_NORMAL;
    if (bold)
        weight = FW_BOLD;

    // DirectX 글꼴 생성
    if (FAILED(D3DXCreateFont(graphics->get3Ddevice(), height, 0, weight, 1, italic,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, fontName.c_str(),
        &dxFont))) return false;

    // 변환 행렬 생성
    D3DXMatrixTransformation2D(&matrix, NULL, 0.0f, NULL, NULL, 0.0f, NULL);

    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// x,y에 텍스트 출력
// 실패 시 0, 성공 시 텍스트 높이 반환
// 사전 조건: 
//          spriteBegin()
// 사후 조건: 
//          spriteEnd()
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
int TextDX::print(const std::string& str, int x, int y)
{
    if (dxFont == NULL)
        return 0;
    // 글꼴 위치 설정
    fontRect.top = y;
    fontRect.left = x;

    // 회전 중심
    D3DXVECTOR2 rCenter = D3DXVECTOR2((float)x, (float)y);
    // 각도만큼 텍스트를 회전시키도록 행렬 설정
    D3DXMatrixTransformation2D(&matrix, NULL, 0.0f, NULL, &rCenter, angle, NULL);
    // 스프라이트에게 행렬에 대해 알림 "Hello Neo"
    graphics->getSprite()->SetTransform(&matrix);
    return dxFont->DrawTextA(graphics->getSprite(), str.c_str(), -1, &fontRect, DT_LEFT, color);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// DirectX 텍스트 형식을 사용하여 RECT 내에 텍스트 출력
// 실패 시 0, 성공 시 텍스트 높이 반환
// 사전 조건: 
//          spriteBegin()
// 사후 조건: 
//          spriteEnd()
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
int TextDX::print(const std::string& str, RECT& rect, UINT format)
{
    if (dxFont == NULL)
        return 0;

    // 텍스트를 회전시키지 않도록 행렬 설정
    D3DXMatrixTransformation2D(&matrix, NULL, 0.0f, NULL, NULL, NULL, NULL);
    // 스프라이트에게 행렬에 대해 알림 "Hello Neo"
    graphics->getSprite()->SetTransform(&matrix);
    return dxFont->DrawTextA(graphics->getSprite(), str.c_str(), -1, &rect, format, color);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 그래픽 장치를 잃어버렸을 때 호출됨
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void TextDX::onLostDevice()
{
    if (dxFont == NULL)
        return;
    dxFont->OnLostDevice();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 그래픽 장치가 재설정될 때 호출됨
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void TextDX::onResetDevice()
{
    if (dxFont == NULL)
        return;
    dxFont->OnResetDevice();
}