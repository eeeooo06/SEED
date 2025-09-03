#include "Image.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 생성자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Image::Image()
{
    initialized = false;                                                     // 성공적으로 초기화되면 true로 설정
    spriteData.width = 2;
    spriteData.height = 2;
    spriteData.x = 0.0;
    spriteData.y = 0.0;
    spriteData.scale = 1.0;
    spriteData.angle = 0.0;
    spriteData.rect.left = 0;                                                // 다중 프레임 이미지에서 한 프레임을 선택하는 데 사용됨
    spriteData.rect.top = 0;
    spriteData.rect.right = spriteData.width;
    spriteData.rect.bottom = spriteData.height;
    spriteData.texture = NULL;                                               // 스프라이트 텍스처 (그림)
    spriteData.flipHorizontal = false;
    spriteData.flipVertical = false;
    cols = 1;
    textureManager = NULL;
    startFrame = 0;
    endFrame = 0;
    currentFrame = 0;
    frameDelay = 1.0;                                                        // 기본적으로 애니메이션 프레임당 1초
    animTimer = 0.0;
    visible = true;                                                          // 이미지가 보임
    loop = true;                                                             // 프레임 반복
    animComplete = false;
    graphics = NULL;                                                         // 그래픽 시스템에 대한 링크
    colorFilter = GraphicsNS::WHITE;                                         // 변경 없음은 WHITE
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 소멸자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Image::~Image()
{
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 이미지를 초기화합니다.
// 사후 조건: 
//          성공하면 true, 실패하면 false를 반환합니다.
// Graphics에 대한 포인터
// 이미지의 너비 (픽셀 단위) (0 = 전체 텍스처 너비 사용)
// 이미지의 높이 (픽셀 단위) (0 = 전체 텍스처 높이 사용)
// 텍스처의 열 수 (1에서 n까지) (0은 1과 동일)
// TextureManager에 대한 포인터
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool Image::initialize(Graphics* g, int width, int height, int ncols,
    TextureManager* textureM)
{
    try {
        graphics = g;                                                        // 그래픽 객체
        textureManager = textureM;                                           // 텍스처 객체에 대한 포인터

        spriteData.texture = textureManager->getTexture();
        if (width == 0)
            width = textureManager->getWidth();                              // 텍스처의 전체 너비 사용
        spriteData.width = width;
        if (height == 0)
            height = textureManager->getHeight();                            // 텍스처의 전체 높이 사용
        spriteData.height = height;
        cols = ncols;
        if (cols == 0)
            cols = 1;                                                        // 열이 0이면 1을 사용

        // currentFrame을 그리도록 spriteData.rect 구성
        spriteData.rect.left = (currentFrame % cols) * spriteData.width;
        // 오른쪽 가장자리 + 1
        spriteData.rect.right = spriteData.rect.left + spriteData.width;
        spriteData.rect.top = (currentFrame / cols) * spriteData.height;
        // 아래쪽 가장자리 + 1
        spriteData.rect.bottom = spriteData.rect.top + spriteData.height;
    }
    catch (...) { return false; }
    initialized = true;                                                      // 성공적으로 초기화됨
    return true;
}


// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 색상을 필터로 사용하여 이미지 그리기
// color 매개변수는 선택 사항이며, image.h에서 기본값으로 WHITE가 할당됩니다.
// 사전 조건: 
//          spriteBegin()이 호출됨
// 사후 조건: 
//          spriteEnd()가 호출됨
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Image::draw(COLOR_ARGB color)
{
    if (!visible || graphics == NULL)
        return;
    // onReset()이 호출된 경우를 대비하여 새로운 텍스처 가져오기
    spriteData.texture = textureManager->getTexture();
    if (color == GraphicsNS::FILTER)                                         // 필터로 그리는 경우
        graphics->drawSprite(spriteData, colorFilter);                       // colorFilter 사용
    else
        graphics->drawSprite(spriteData, color);                             // color를 필터로 사용
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 지정된 SpriteData를 사용하여 이 이미지 그리기
// 현재 SpriteData.rect는 텍스처를 선택하는 데 사용됩니다.
// 사전 조건: 
//          spriteBegin()이 호출됨
// 사후 조건: 
//          spriteEnd()가 호출됨
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Image::draw(SpriteData sd, COLOR_ARGB color)
{
    if (!visible || graphics == NULL)
        return;
    sd.rect = spriteData.rect;                                               // 이 이미지의 rect를 사용하여 텍스처 선택
    sd.texture = textureManager->getTexture();                               // onReset()이 호출된 경우를 대비하여 새로운 텍스처 가져오기

    if (color == GraphicsNS::FILTER)                                         // 필터로 그리는 경우
        graphics->drawSprite(sd, colorFilter);                               // colorFilter 사용
    else
        graphics->drawSprite(sd, color);                                     // color를 필터로 사용
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 업데이트
// 일반적으로 프레임당 한 번 호출됩니다.
// frameTime은 움직임과 애니메이션의 속도를 조절하는 데 사용됩니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Image::update(float frameTime)
{
    if (endFrame - startFrame > 0)                                           // 애니메이션 스프라이트인 경우
    {
        animTimer += frameTime;                                              // 총 경과 시간
        if (animTimer > frameDelay)
        {
            animTimer -= frameDelay;
            currentFrame++;
            if (currentFrame < startFrame || currentFrame > endFrame)
            {
                if (loop == true)                                            // 애니메이션을 반복하는 경우
                    currentFrame = startFrame;
                else                                                         // 애니메이션을 반복하지 않음
                {
                    currentFrame = endFrame;
                    animComplete = true;                                     // 애니메이션 완료
                }
            }
            setRect();                                                       // spriteData.rect 설정
        }
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 이미지의 현재 프레임 설정
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Image::setCurrentFrame(int c)
{
    if (c >= 0)
    {
        currentFrame = c;
        animComplete = false;
        setRect();                                                           // spriteData.rect 설정
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
//  currentFrame을 그리도록 spriteData.rect 설정
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
inline void Image::setRect()
{
    // currentFrame을 그리도록 spriteData.rect 구성
    spriteData.rect.left = (currentFrame % cols) * spriteData.width;
    // 오른쪽 가장자리 + 1
    spriteData.rect.right = spriteData.rect.left + spriteData.width;
    spriteData.rect.top = (currentFrame / cols) * spriteData.height;
    // 아래쪽 가장자리 + 1
    spriteData.rect.bottom = spriteData.rect.top + spriteData.height;
}