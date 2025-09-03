#include "InputDialog.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 생성자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
InputDialog::InputDialog()
{
    textBackColor = inputDialogNS::TEXT_BACK_COLOR;
    textFontColor = inputDialogNS::TEXT_COLOR;
    inTextVerts = NULL;
    inText = "";
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 소멸자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
InputDialog::~InputDialog()
{
    onLostDevice();                                                          // 모든 그래픽 항목에 대해 onLostDevice() 호출
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
// 대화 상자 배경 및 버튼 그리기를 위한 정점 버퍼 준비                             
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
void InputDialog::prepareVerts()
{
    MessageDialog::prepareVerts();                                           // 기본 클래스에서 prepareVerts 호출
    SAFE_RELEASE(inTextVerts);

    // inText 왼쪽 상단
    vtx[0].x = x + messageDialogNS::BORDER * 2;
    vtx[0].y = y + height - messageDialogNS::BORDER - messageDialogNS::MARGIN - messageDialogNS::BUTTON_HEIGHT * 2.5f;
    vtx[0].z = 0.0f;
    vtx[0].rhw = 1.0f;
    vtx[0].color = textBackColor;
    // inText 오른쪽 상단
    vtx[1].x = x + width - messageDialogNS::BORDER * 2;
    vtx[1].y = vtx[0].y;
    vtx[1].z = 0.0f;
    vtx[1].rhw = 1.0f;
    vtx[1].color = textBackColor;
    // inText 오른쪽 하단
    vtx[2].x = vtx[1].x;
    vtx[2].y = vtx[0].y + messageDialogNS::BUTTON_HEIGHT;
    vtx[2].z = 0.0f;
    vtx[2].rhw = 1.0f;
    vtx[2].color = textBackColor;
    // inText 왼쪽 하단
    vtx[3].x = vtx[0].x;
    vtx[3].y = vtx[2].y;
    vtx[3].z = 0.0f;
    vtx[3].rhw = 1.0f;
    vtx[3].color = textBackColor;
    graphics->createVertexBuffer(vtx, sizeof vtx, inTextVerts);

    // inTextRect 설정
    inTextRect.left = (long)vtx[0].x;
    inTextRect.right = (long)vtx[1].x;
    inTextRect.top = (long)vtx[0].y;
    inTextRect.bottom = (long)vtx[2].y;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// InputDialog 그리기
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
const void InputDialog::draw()
{
    if (!visible || graphics == NULL || !initialized)
        return;

    graphics->drawQuad(borderVerts);                                         // 테두리 그리기
    graphics->drawQuad(dialogVerts);                                         // 배경 그리기
    graphics->drawQuad(buttonVerts);                                         // 버튼 그리기
    graphics->drawQuad(button2Verts);                                        // 버튼2 그리기
    graphics->drawQuad(inTextVerts);                                         // 입력 텍스트 영역 그리기

    graphics->spriteBegin();                                                 // 스프라이트 그리기 시작

    if (text.size() == 0)
        return;
    // MessageDialog에 텍스트 표시
    dxFont.setFontColor(fontColor);
    dxFont.print(text, textRect, DT_CENTER | DT_WORDBREAK);

    // 버튼에 텍스트 표시
    dxFont.setFontColor(buttonFontColor);
    dxFont.print(messageDialogNS::BUTTON1_TEXT[buttonType], buttonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    dxFont.print(messageDialogNS::BUTTON2_TEXT[buttonType], button2Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // 입력 텍스트 표시
    dxFont.setFontColor(textFontColor);
    tempRect = inTextRect;                                                   // 저장
    // DT_CALDRECT 옵션으로는 텍스트가 출력되지 않습니다. RECT.right를 이동시킵니다.
    dxFont.print(inText, tempRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_CALCRECT);
    if (tempRect.right > inTextRect.right)                                   // 텍스트가 너무 길면 오른쪽 정렬
        dxFont.print(inText, inTextRect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
    else                                                                     // 그렇지 않으면 왼쪽 정렬
        dxFont.print(inText, inTextRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

    graphics->spriteEnd();                                                   // 스프라이트 그리기 종료
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
// DIALOG_CLOSE_KEY 및 버튼 클릭 확인                                          
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
void InputDialog::update()
{
    MessageDialog::update();                                                 // 기본 클래스에서 update 호출
    if (!initialized || !visible)
    {
        if (buttonClicked == 2)                                              // 취소 버튼인 경우
            inText = "";                                                     // 입력 텍스트 지우기
        return;
    }
    inText = input->getTextIn();                                             // 입력 텍스트 가져오기
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 텍스트 문자열을 설정하고, 텍스트에 맞게 대화 상자 하단 크기를 조정하고, visible = true로 설정합니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void InputDialog::print(const std::string& str)
{
    if (!initialized || visible)                                             // 초기화되지 않았거나 이미 사용 중인 경우
        return;
    text = str + "\n\n\n\n\n";                                               // 입력 텍스트와 버튼을 위한 공간 남기기

    // textRect를 대화 상자의 텍스트 영역으로 설정
    textRect.left = (long)(x + messageDialogNS::MARGIN);
    textRect.right = (long)(x + messageDialogNS::WIDTH - messageDialogNS::MARGIN);
    textRect.top = (long)(y + messageDialogNS::MARGIN);
    textRect.bottom = (long)(y + messageDialogNS::HEIGHT - messageDialogNS::MARGIN);

    // textRect.bottom을 텍스트에 필요한 정확한 높이로 설정
    // DT_CALDRECT 옵션으로는 텍스트가 출력되지 않습니다.
    dxFont.print(text, textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
    height = textRect.bottom - (int)y + messageDialogNS::BORDER + messageDialogNS::MARGIN;

    prepareVerts();                                                          // 정점 버퍼 준비
    inText = "";                                                             // 이전 입력 지우기
    input->clearTextIn();
    buttonClicked = 0;                                                       // buttonClicked 지우기
    visible = true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 그래픽 장치를 잃어버렸을 때 호출됨
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void InputDialog::onLostDevice()
{
    if (!initialized)
        return;
    MessageDialog::onLostDevice();
    SAFE_RELEASE(inTextVerts);
}