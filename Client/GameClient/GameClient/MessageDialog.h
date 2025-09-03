#ifndef _MESSAGEDIALOG_H                                                     // 이 파일이 두 번 이상 포함될 경우
#define _MESSAGEDIALOG_H                                                     // 다중 정의를 방지합니다.
#define WIN32_LEAN_AND_MEAN

#include <string>
#include "Constants.h"
#include "TextDX.h"
#include "Graphics.h"
#include "Input.h"

namespace messageDialogNS
{
    const UINT WIDTH = 400;                                                  // 대화 상자 기본 너비
    const UINT HEIGHT = 100;                                                 // 기본 높이
    const UINT BORDER = 5;
    const UINT MARGIN = 5;                                                   // 테두리로부터의 텍스트 여백
    const char FONT[] = "Arial";                                             // 글꼴
    const int FONT_HEIGHT = 18;                                              // 글꼴 높이
    const COLOR_ARGB FONT_COLOR = GraphicsNS::WHITE;                         // 텍스트 색상
    const COLOR_ARGB BORDER_COLOR = D3DCOLOR_ARGB(192, 192, 192, 192);       // 테두리 색상
    const COLOR_ARGB BACK_COLOR = SETCOLOR_ARGB(255, 100, 100, 192);         // 배경색
    const UINT X = GAME_WIDTH / 2 - WIDTH / 2;                               // 기본 위치
    const UINT Y = GAME_HEIGHT / 4 - HEIGHT / 2;
    const UINT BUTTON_WIDTH = (UINT)(FONT_HEIGHT * 4.5);
    const UINT BUTTON_HEIGHT = FONT_HEIGHT + 4;
    const int MAX_TYPE = 2;
    const int OK_CANCEL = 0;                                                 // 확인/취소 버튼 유형
    const int YES_NO = 1;                                                    // 예/아니오 버튼 유형
    static const char* BUTTON1_TEXT[MAX_TYPE] = { "OK", "YES" };
    static const char* BUTTON2_TEXT[MAX_TYPE] = { "CANCEL", "NO" };
    const byte DIALOG_CLOSE_KEY = VK_RETURN;                                 // Enter 키
    const COLOR_ARGB BUTTON_COLOR = GraphicsNS::GRAY;                        // 버튼 배경
    const COLOR_ARGB BUTTON_FONT_COLOR = GraphicsNS::WHITE;                  // 버튼 텍스트 색상
}

// 메시지 대화 상자
class MessageDialog
{
protected:
    Graphics* graphics;                                                      // 그래픽 시스템
    Input* input;                                                            // 입력 시스템
    TextDX      dxFont;                                                      // DirectX 글꼴
    float       x, y;                                                        // 화면 위치
    UINT        height;                                                      // 대화 상자 높이, print()에서 계산됨
    UINT        width;                                                       // 대화 상자 너비
    std::string text;                                                        // 대화 상자 텍스트
    RECT        textRect;                                                    // 텍스트 사각형
    RECT        buttonRect;                                                  // 버튼 사각형
    RECT        button2Rect;                                                 // 버튼2 사각형
    COLOR_ARGB  fontColor;                                                   // 글꼴 색상 (a,r,g,b)
    COLOR_ARGB  borderColor;                                                 // 테두리 색상 (a,r,g,b)
    COLOR_ARGB  backColor;                                                   // 배경 색상 (a,r,g,b)
    COLOR_ARGB  buttonColor;                                                 // 버튼 색상
    COLOR_ARGB  buttonFontColor;                                             // 버튼 글꼴 색상
    VertexC vtx[4];                                                          // 정점 데이터
    LP_VERTEXBUFFER dialogVerts;                                             // 대화 상자 정점 버퍼
    LP_VERTEXBUFFER borderVerts;                                             // 테두리 정점 버퍼
    LP_VERTEXBUFFER buttonVerts;                                             // 버튼 정점 버퍼
    LP_VERTEXBUFFER button2Verts;                                            // 버튼2 정점 버퍼
    int buttonClicked;                                                       // 클릭된 버튼 (1 또는 2)
    int buttonType;                                                          // 0 = 확인/취소, 1 = 예/아니오
    bool    initialized;                                                     // 성공적으로 초기화되면 true
    bool    visible;                                                         // 표시하려면 true
    HWND    hwnd;                                                            // 창 핸들
    float   screenRatioX, screenRatioY;

public:
    // 생성자
    MessageDialog();
    // 소멸자
    virtual ~MessageDialog();

    // MessageDialog를 초기화합니다.
    // 사전 조건: 
    //          *g는 Graphics 객체를 가리킵니다.
    //          *in은 Input 객체를 가리킵니다.
    //          hwnd = 창 핸들
    bool initialize(Graphics* g, Input* in, HWND hwnd);

    // 정점 버퍼 준비
    void prepareVerts();

    // MessageDialog를 표시합니다.
    const void draw();

    // 클릭된 버튼 반환
    // 0 = 클릭된 버튼 없음
    // 1은 왼쪽 버튼, 2는 오른쪽 버튼
    int getButtonClicked() { return buttonClicked; }

    // visible 반환
    bool getVisible() { return visible; }

    // 글꼴 색상 설정
    void setFontColor(COLOR_ARGB fc) { fontColor = fc; }

    // 테두리 색상 설정
    void setBorderColor(COLOR_ARGB bc) { borderColor = bc; }

    // 배경색 설정
    void setBackColor(COLOR_ARGB bc) { backColor = bc; }

    // 버튼 색상 설정
    void setButtonColor(COLOR_ARGB bc) { buttonColor = bc; }

    // 버튼 글꼴 색상 설정
    void setButtonFontColor(COLOR_ARGB bfc) { buttonFontColor = bfc; }

    // visible 설정;
    void setVisible(bool v) { visible = v; }

    // 버튼 유형 설정 0 = 확인/취소, 1 = 예/아니오
    void setButtonType(UINT t)
    {
        if (t < messageDialogNS::MAX_TYPE)
            buttonType = t;
    }

    // MessageDialog에 텍스트 str 표시
    void print(const std::string& str);

    // 닫기 이벤트 확인
    void update();

    // 그래픽 장치를 잃어버렸을 때 호출됩니다.
    void onLostDevice();

    // 그래픽 장치가 재설정될 때 호출됩니다.
    void onResetDevice();
};

#endif