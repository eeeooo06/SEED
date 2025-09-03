#ifndef _CONSOLE_H                                                           // 이 파일이 두 번 이상 포함될 경우 다중 정의를 방지합니다.
#define _CONSOLE_H              
#define WIN32_LEAN_AND_MEAN

#include <string>
#include <deque>
#include "Constants.h"
#include "TextDX.h"
#include "Graphics.h"
#include "Input.h"

namespace consoleNS
{
    const UINT WIDTH = 500;                                                  // 콘솔 너비
    const UINT HEIGHT = 400;                                                 // 콘솔 높이
    const UINT X = 5;                                                        // 콘솔 위치
    const UINT Y = 5;
    const UINT MARGIN = 4;                                                   // 콘솔 가장자리로부터의 텍스트 여백
    const char FONT[] = "Courier New";                                       // 콘솔 글꼴
    const int FONT_HEIGHT = 14;                                              // 글꼴 높이 (픽셀 단위)
    const COLOR_ARGB FONT_COLOR = GraphicsNS::WHITE;                         // 콘솔 텍스트 색상
    const COLOR_ARGB BACK_COLOR = SETCOLOR_ARGB(192, 128, 128, 128);         // 배경색
    const int MAX_LINES = 256;                                               // 텍스트 버퍼의 최대 줄 수
}

// 
class Console
{
private:
    Graphics* graphics;                                                      // 그래픽 시스템
    Input* input;                                                            // 입력 시스템
    TextDX      dxFont;                                                      // DirectX 글꼴
    float       x, y;                                                        // 콘솔 위치 (동적)
    int         rows;                                                        // 콘솔에 들어갈 텍스트 줄 수
    std::string commandStr;                                                  // 콘솔 명령어
    std::string inputStr;                                                    // 콘솔 텍스트 입력
    std::deque<std::string> text;                                            // 콘솔 텍스트
    RECT        textRect;                                                    // 텍스트 사각형
    COLOR_ARGB  fontColor;                                                   // 글꼴 색상 (a,r,g,b)
    COLOR_ARGB  backColor;                                                   // 배경 색상 (a,r,g,b)
    VertexC vtx[4];                                                          // 배경을 위한 정점 데이터
    LP_VERTEXBUFFER vertexBuffer;                                            // 정점 데이터를 담을 버퍼
    int         scrollAmount;                                                // 디스플레이를 위로 스크롤할 줄 수
    bool        initialized;                                                 // 성공적으로 초기화되면 true
    bool        visible;                                                     // 표시하려면 true

public:
    // 생성자
    Console();

    // 소멸자
    virtual ~Console();

    // 콘솔 초기화
    // 사전 조건: 
    //          *g는 Graphics를 가리킵니다.
    //          *in은 Input을 가리킵니다.
    bool initialize(Graphics* g, Input* in);

    // 콘솔을 표시합니다.
    const void draw();

    // 콘솔 보이기/숨기기
    void showHide();

    // visible을 반환합니다.
    bool getVisible() { return visible; }

    // visible = true로 설정;
    void show() { visible = true; }

    // visible = false로 설정;
    void hide() { visible = false; }

    // 콘솔 디스플레이에 텍스트 str을 추가합니다.
    // str의 첫 번째 줄만 표시됩니다.
    void print(const std::string& str);

    // 콘솔 명령어를 반환합니다.
    std::string getCommand();

    // 콘솔 입력 텍스트를 반환합니다.
    std::string getInput() { return inputStr; }

    // 입력 텍스트 지우기
    void clearInput() { inputStr = ""; }

    // 그래픽 장치를 잃어버렸을 때 호출됩니다.
    void onLostDevice();

    // 그래픽 장치가 재설정될 때 호출됩니다.
    void onResetDevice();
};

#endif