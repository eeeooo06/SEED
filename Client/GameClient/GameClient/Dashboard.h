#ifndef _DASHBOARD_H                                                         // 이 파일이 두 번 이상 포함될 경우 다중 정의를 방지합니다.
#define _DASHBOARD_H            
#define WIN32_LEAN_AND_MEAN

#include "Image.h"
#include "Constants.h"
#include "TextureManager.h"

namespace dashboardNS
{
    const int   IMAGE_SIZE = 32;                                             // 각 텍스처 크기
    const int   TEXTURE_COLS = 8;                                            // 텍스처 열
    const int   BAR_FRAME = 44;                                              // 막대의 프레임 번호
}

class Bar : public Image
{
public:
    // 막대 초기화
    // 사전 조건: 
    //          *graphics = Graphics 객체에 대한 포인터
    //          *textureM = TextureManager 객체에 대한 포인터
    //          left, top = 화면 위치
    //          scale = 스케일링 (확대/축소) 양
    //          color = 막대 색상
    bool initialize(Graphics* graphics, TextureManager* textureM, int left, int top,
        float scale, COLOR_ARGB color);
    // 막대 크기 설정
    void set(float percentOn);
    // setRect()가 호출되지 않도록 update를 재정의합니다.
    virtual void update(float frameTime) {}
};

#endif