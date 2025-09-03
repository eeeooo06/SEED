#ifndef _TEXTDX_H                                                            // 이 파일이 두 번 이상 포함될 경우
#define _TEXTDX_H                                                            // 다중 정의를 방지합니다.
#define WIN32_LEAN_AND_MEAN

#include <string>
#include "Constants.h"
#include "Graphics.h"

class TextDX
{
private:
    Graphics* graphics;
    COLOR_ARGB  color;                                                       // 글꼴 색상 (a,r,g,b)
    LP_DXFONT   dxFont;
    RECT        fontRect;                                                    // 텍스트 사각형
    // 텍스트를 회전시키기 위한 행렬                                             
    D3DXMATRIX  matrix;
    float       angle;                                                       // 텍스트의 회전 각도 (라디안)

public:
    // 생성자 (스프라이트 텍스트)
    TextDX();

    // 소멸자
    virtual ~TextDX();

    // 글꼴 초기화
    // 사전 조건: 
    //          *g는 Graphics 객체를 가리킵니다.
    //          height = 높이 (픽셀 단위)
    //          bold = 굵게 (true/false)
    //          italic = 기울임꼴 (true/false)
    //          &fontName = 사용할 글꼴 이름
    virtual bool initialize(Graphics* g, int height, bool bold, bool italic, const std::string& fontName);

    // x,y 위치에 출력합니다. spriteBegin()/spriteEnd() 사이에서 호출하세요.
    // 실패 시 0, 성공 시 텍스트 높이를 반환합니다.
    // 사전 조건: 
    //          &str은 표시할 텍스트를 포함합니다.
    //          x, y = 화면 위치
    virtual int print(const std::string& str, int x, int y);

    // 형식을 사용하여 사각형 내부에 출력합니다. spriteBegin()/spriteEnd() 사이에서 호출하세요.
    // 실패 시 0, 성공 시 텍스트 높이를 반환합니다.
    // 사전 조건:   
    //          &str = 표시할 텍스트
    //          &rect = 사각형 영역
    //          format = 형식 지정자
    virtual int print(const std::string& str, RECT& rect, UINT format);

    // 회전 각도를 도 단위로 반환합니다.
    virtual float getDegrees() { return angle * (180.0f / (float)PI); }

    // 회전 각도를 라디안 단위로 반환합니다.
    virtual float getRadians() { return angle; }

    // 글꼴 색상을 반환합니다.
    virtual COLOR_ARGB getFontColor() { return color; }

    // 회전 각도를 도 단위로 설정합니다.
    // 0도는 위쪽입니다. 각도는 시계 방향으로 진행됩니다.
    virtual void setDegrees(float deg) { angle = deg * ((float)PI / 180.0f); }

    // 회전 각도를 라디안 단위로 설정합니다.
    // 0 라디안은 위쪽입니다. 각도는 시계 방향으로 진행됩니다.
    virtual void setRadians(float rad) { angle = rad; }

    // 글꼴 색상을 설정합니다. SETCOLOR_ARGB 매크로 또는 graphicsNS::의 색상을 사용하세요.
    virtual void setFontColor(COLOR_ARGB c) { color = c; }

    // 리소스 해제
    virtual void onLostDevice();

    // 리소스 복원
    virtual void onResetDevice();
};

#endif