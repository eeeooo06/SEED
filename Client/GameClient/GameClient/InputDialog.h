#ifndef _INPUTDIALOG_H                                                       // 이 파일이 두 번 이상 포함될 경우 다중 정의를 방지합니다.
#define _INPUTDIALOG_H          
#define WIN32_LEAN_AND_MEAN

#include <string>
#include "Constants.h"
#include "TextDX.h"
#include "Graphics.h"
#include "Input.h"
#include "MessageDialog.h"

namespace inputDialogNS
{
    const COLOR_ARGB TEXT_BACK_COLOR = GraphicsNS::WHITE;                    // 입력 텍스트 배경
    const COLOR_ARGB TEXT_COLOR = GraphicsNS::BLACK;                         // 입력 텍스트 색상
}

// 입력 대화상자, MessageDialog로부터 상속받음
class InputDialog : public MessageDialog
{
private:
    std::string inText;                                                      // 입력 텍스트
    RECT        inTextRect;
    RECT        tempRect;
    COLOR_ARGB  textBackColor;                                               // 텍스트 영역 배경색
    COLOR_ARGB  textFontColor;                                               // 텍스트 영역 글꼴색
    LP_VERTEXBUFFER inTextVerts;                                             // 텍스트 영역 정점 버퍼

public:
    // 생성자
    InputDialog();
    // 소멸자
    virtual ~InputDialog();

    // 정점 버퍼 준비
    void prepareVerts();

    // InputDialog를 표시합니다.
    const void draw();

    // 입력 텍스트를 반환합니다.
    std::string getText()
    {
        if (!visible)
            return inText;
        else
            return "";
    }

    // 입력 텍스트 글꼴색 설정
    void setTextFontColor(COLOR_ARGB fc) { textFontColor = fc; }

    // 입력 텍스트 배경색 설정
    void setTextBackColor(COLOR_ARGB bc) { textBackColor = bc; }

    // InputDialog에 텍스트 str 표시
    void print(const std::string& str);

    // 닫기 이벤트 확인
    void update();

    // 그래픽 장치를 잃어버렸을 때 호출됩니다.
    void onLostDevice();
};

#endif