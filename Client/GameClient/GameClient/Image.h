#ifndef _IMAGE_H                                                             // 이 파일이 두 번 이상 포함될 경우 다중 정의를 방지합니다.
#define _IMAGE_H                
#define WIN32_LEAN_AND_MEAN

#include "TextureManager.h"
#include "Constants.h"

class Image
{
    // 이미지 속성
protected:
    Graphics* graphics;                                                      // 그래픽에 대한 포인터
    TextureManager* textureManager;                                          // 텍스처 관리자에 대한 포인터
    // spriteData는 Graphics::drawSprite()로 이미지를 그리는 데 필요한 데이터를 포함합니다.
    SpriteData spriteData;                                                   // SpriteData는 "graphics.h"에 정의되어 있습니다.
    COLOR_ARGB colorFilter;                                                  // 색상 필터로 적용됨 (변경하지 않으려면 WHITE 사용)
    int     cols;                                                            // 다중 프레임 스프라이트의 열 수 (1에서 n까지)
    int     startFrame;                                                      // 현재 애니메이션의 첫 프레임
    int     endFrame;                                                        // 현재 애니메이션의 마지막 프레임
    int     currentFrame;                                                    // 현재 애니메이션 프레임
    float   frameDelay;                                                      // 애니메이션 프레임 사이의 시간
    float   animTimer;                                                       // 애니메이션 타이머
    HRESULT hr;                                                              // 표준 반환 유형
    bool    loop;                                                            // 프레임을 반복하려면 true
    bool    visible;                                                         // 보일 때 true
    bool    initialized;                                                     // 성공적으로 초기화되면 true
    bool    animComplete;                                                    // loop가 false이고 endFrame 표시가 완료되면 true

public:
    // 생성자
    Image();
    // 소멸자
    virtual ~Image();

    ////////////////////////////////////////
    //           Get 함수            //
    ////////////////////////////////////////

    // SpriteData 구조체에 대한 참조를 반환합니다.
    const virtual SpriteData& getSpriteInfo() { return spriteData; }

    // visible 매개변수를 반환합니다.
    virtual bool  getVisible() { return visible; }

    // X 위치를 반환합니다.
    virtual float getX() { return spriteData.x; }

    // Y 위치를 반환합니다.
    virtual float getY() { return spriteData.y; }

    // 스케일 팩터를 반환합니다.
    virtual float getScale() { return spriteData.scale; }

    // 너비를 반환합니다.
    virtual int   getWidth() { return spriteData.width; }

    // 높이를 반환합니다.
    virtual int   getHeight() { return spriteData.height; }

    // 중심 X를 반환합니다.
    virtual float getCenterX() { return spriteData.x + spriteData.width / 2 * getScale(); }

    // 중심 Y를 반환합니다.
    virtual float getCenterY() { return spriteData.y + spriteData.height / 2 * getScale(); }

    // 회전 각도를 도 단위로 반환합니다.
    virtual float getDegrees() { return spriteData.angle * (180.0f / (float)PI); }

    // 회전 각도를 라디안 단위로 반환합니다.
    virtual float getRadians() { return spriteData.angle; }

    // 애니메이션 프레임 사이의 지연 시간을 반환합니다.
    virtual float getFrameDelay() { return frameDelay; }

    // 시작 프레임 번호를 반환합니다.
    virtual int   getStartFrame() { return startFrame; }

    // 종료 프레임 번호를 반환합니다.
    virtual int   getEndFrame() { return endFrame; }

    // 현재 프레임 번호를 반환합니다.
    virtual int   getCurrentFrame() { return currentFrame; }

    // 이미지의 RECT 구조체를 반환합니다.
    virtual RECT  getSpriteDataRect() { return spriteData.rect; }

    // 애니메이션 완료 상태를 반환합니다.
    virtual bool  getAnimationComplete() { return animComplete; }

    // colorFilter를 반환합니다.
    virtual COLOR_ARGB getColorFilter() { return colorFilter; }

    ////////////////////////////////////////
    //           Set 함수            //
    ////////////////////////////////////////

    // X 위치를 설정합니다.
    virtual void setX(float newX) { spriteData.x = newX; }

    // Y 위치를 설정합니다.
    virtual void setY(float newY) { spriteData.y = newY; }

    // 스케일을 설정합니다.
    virtual void setScale(float s) { spriteData.scale = s; }

    // 회전 각도를 도 단위로 설정합니다.
    // 0도는 위쪽입니다. 각도는 시계 방향으로 진행됩니다.
    virtual void setDegrees(float deg) { spriteData.angle = deg * ((float)PI / 180.0f); }

    // 회전 각도를 라디안 단위로 설정합니다.
    // 0 라디안은 위쪽입니다. 각도는 시계 방향으로 진행됩니다.
    virtual void setRadians(float rad) { spriteData.angle = rad; }

    // visible을 설정합니다.
    virtual void setVisible(bool v) { visible = v; }

    // 애니메이션 프레임 사이의 지연 시간을 설정합니다.
    virtual void setFrameDelay(float d) { frameDelay = d; }

    // 애니메이션의 시작 및 종료 프레임을 설정합니다.
    virtual void setFrames(int s, int e) { startFrame = s; endFrame = e; }

    // 현재 애니메이션 프레임을 설정합니다.
    virtual void setCurrentFrame(int c);

    // currentFrame을 그리도록 spriteData.rect를 설정합니다.
    virtual void setRect();

    // spriteData.rect를 r로 설정합니다.
    virtual void setSpriteDataRect(RECT r) { spriteData.rect = r; }

    // 애니메이션 루프를 설정합니다. lp = true이면 루프합니다.
    virtual void setLoop(bool lp) { loop = lp; }

    // 애니메이션 완료 부울을 설정합니다.
    virtual void setAnimationComplete(bool a) { animComplete = a; };

    // 색상 필터를 설정합니다. (변경하지 않으려면 WHITE 사용)
    virtual void setColorFilter(COLOR_ARGB color) { colorFilter = color; }

    // TextureManager 설정
    virtual void setTextureManager(TextureManager* textureM)
    {
        textureManager = textureM;
    }

    ////////////////////////////////////////
    //         기타 함수            //
    ////////////////////////////////////////

    // 이미지 초기화
    // 사전 조건: 
    //          *g = Graphics 객체에 대한 포인터
    //          width = 이미지의 너비 (픽셀 단위) (0 = 전체 텍스처 너비 사용)
    //          height = 이미지의 높이 (픽셀 단위) (0 = 전체 텍스처 높이 사용)
    //          ncols = 텍스처의 열 수 (1에서 n까지) (0은 1과 동일)
    //          *textureM = TextureManager 객체에 대한 포인터
    virtual bool initialize(Graphics* g, int width, int height,
        int ncols, TextureManager* textureM);

    // 이미지를 수평으로 뒤집기 (미러)
    virtual void flipHorizontal(bool flip) { spriteData.flipHorizontal = flip; }

    // 이미지를 수직으로 뒤집기
    virtual void flipVertical(bool flip) { spriteData.flipVertical = flip; }

    // 색상을 필터로 사용하여 이미지를 그립니다. 기본 색상은 WHITE입니다.
    virtual void draw(COLOR_ARGB color = GraphicsNS::WHITE);

    // 지정된 SpriteData를 사용하여 이 이미지를 그립니다.
    // 현재 SpriteData.rect는 텍스처를 선택하는 데 사용됩니다.
    virtual void draw(SpriteData sd, COLOR_ARGB color = GraphicsNS::WHITE);  // 색상을 필터로 사용하여 SpriteData로 그립니다.

    // 애니메이션을 업데이트합니다. frameTime은 속도를 조절하는 데 사용됩니다.
    virtual void update(float frameTime);
};

#endif