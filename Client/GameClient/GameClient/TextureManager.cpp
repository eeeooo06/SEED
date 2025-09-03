#include "TextureManager.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 생성자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
TextureManager::TextureManager()
{
    texture = NULL;
    width = 0;
    height = 0;
    file = NULL;
    graphics = NULL;
    initialized = false;                                                     // 성공적으로 초기화되면 true로 설정
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 소멸자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
TextureManager::~TextureManager()
{
    SAFE_RELEASE(texture);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 디스크에서 텍스처 파일을 로드합니다.
// 사후조건: 
//          성공하면 true, 실패하면 false를 반환합니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool TextureManager::initialize(Graphics* g, const char* f)
{
    try {
        graphics = g;                                                        // 그래픽 객체
        file = f;                                                            // 텍스처 파일

        hr = graphics->loadTexture(file, TRANSCOLOR, width, height, texture);
        if (FAILED(hr))
        {
            SAFE_RELEASE(texture);
            return false;
        }
    }
    catch (...) { return false; }
    initialized = true;                                                      // 성공적으로 초기화되면 true로 설정
    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 그래픽 장치를 잃어버렸을 때 호출됩니다
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void TextureManager::onLostDevice()
{
    if (!initialized)
        return;
    SAFE_RELEASE(texture);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 그래픽 장치가 재설정될 때 호출됩니다
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void TextureManager::onResetDevice()
{
    if (!initialized)
        return;
    graphics->loadTexture(file, TRANSCOLOR, width, height, texture);
}