#include "MessageDialog.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 생성자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
MessageDialog::MessageDialog()
{
    initialized = false;                                                     // 성공적으로 초기화되면 true로 설정
    graphics = NULL;
    visible = false;                                                         // 보이지 않음
    fontColor = messageDialogNS::FONT_COLOR;
    borderColor = messageDialogNS::BORDER_COLOR;
    backColor = messageDialogNS::BACK_COLOR;
    buttonColor = messageDialogNS::BUTTON_COLOR;
    buttonFontColor = messageDialogNS::BUTTON_FONT_COLOR;
    x = messageDialogNS::X;                                                  // 시작 위치
    y = messageDialogNS::Y;
    height = messageDialogNS::HEIGHT;
    width = messageDialogNS::WIDTH;
    textRect.bottom = messageDialogNS::Y + messageDialogNS::HEIGHT - messageDialogNS::MARGIN;
    textRect.left = messageDialogNS::X + messageDialogNS::MARGIN;
    textRect.right = messageDialogNS::X + messageDialogNS::WIDTH - messageDialogNS::MARGIN;
    textRect.top = messageDialogNS::Y + messageDialogNS::MARGIN;
    dialogVerts = NULL;
    borderVerts = NULL;
    buttonVerts = NULL;
    button2Verts = NULL;
    buttonType = 0;                                                          // 확인/취소
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 소멸자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
MessageDialog::~MessageDialog()
{
    onLostDevice();                                                          // 모든 그래픽 항목에 대해 onLostDevice() 호출
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// MessageDialog 초기화
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool MessageDialog::initialize(Graphics* g, Input* in, HWND h)
{
    try {
        graphics = g;                                                        // 그래픽 객체
        input = in;                                                          // 입력 객체
        hwnd = h;

        // DirectX 글꼴 초기화
        if (dxFont.initialize(graphics, messageDialogNS::FONT_HEIGHT, false,
            false, messageDialogNS::FONT) == false)
            return false;                                                    // 실패한 경우
        dxFont.setFontColor(fontColor);
    }
    catch (...) {
        return false;
    }

    initialized = true;
    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 대화 상자 배경 및 버튼을 그리기 위한 정점 버퍼 준비
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void MessageDialog::prepareVerts()
{
    SAFE_RELEASE(dialogVerts);
    SAFE_RELEASE(borderVerts);
    SAFE_RELEASE(buttonVerts);
    SAFE_RELEASE(button2Verts);

    // 테두리 왼쪽 상단
    vtx[0].x = x;
    vtx[0].y = y;
    vtx[0].z = 0.0f;
    vtx[0].rhw = 1.0f;
    vtx[0].color = borderColor;

    // 테두리 오른쪽 상단
    vtx[1].x = x + width;
    vtx[1].y = y;
    vtx[1].z = 0.0f;
    vtx[1].rhw = 1.0f;
    vtx[1].color = borderColor;

    // 테두리 오른쪽 하단
    vtx[2].x = x + width;
    vtx[2].y = y + height;
    vtx[2].z = 0.0f;
    vtx[2].rhw = 1.0f;
    vtx[2].color = borderColor;

    // 테두리 왼쪽 하단
    vtx[3].x = x;
    vtx[3].y = y + height;
    vtx[3].z = 0.0f;
    vtx[3].rhw = 1.0f;
    vtx[3].color = borderColor;

    graphics->createVertexBuffer(vtx, sizeof vtx, borderVerts);

    // 배경 왼쪽 상단
    vtx[0].x = x + messageDialogNS::BORDER;
    vtx[0].y = y + messageDialogNS::BORDER;
    vtx[0].z = 0.0f;
    vtx[0].rhw = 1.0f;
    vtx[0].color = backColor;

    // 배경 오른쪽 상단
    vtx[1].x = x + width - messageDialogNS::BORDER;
    vtx[1].y = y + messageDialogNS::BORDER;
    vtx[1].z = 0.0f;
    vtx[1].rhw = 1.0f;
    vtx[1].color = backColor;

    // 배경 오른쪽 하단
    vtx[2].x = x + width - messageDialogNS::BORDER;
    vtx[2].y = y + height - messageDialogNS::BORDER;
    vtx[2].z = 0.0f;
    vtx[2].rhw = 1.0f;
    vtx[2].color = backColor;

    // 배경 왼쪽 하단
    vtx[3].x = x + messageDialogNS::BORDER;
    vtx[3].y = y + height - messageDialogNS::BORDER;
    vtx[3].z = 0.0f;
    vtx[3].rhw = 1.0f;
    vtx[3].color = backColor;

    graphics->createVertexBuffer(vtx, sizeof vtx, dialogVerts);

    // 버튼 왼쪽 상단
    vtx[0].x = x + width / 2.0f - messageDialogNS::BUTTON_WIDTH / 2.0f;
    vtx[0].y = y + height - messageDialogNS::BORDER - messageDialogNS::MARGIN - messageDialogNS::BUTTON_HEIGHT;
    vtx[0].z = 0.0f;
    vtx[0].rhw = 1.0f;
    vtx[0].color = buttonColor;

    // 버튼 오른쪽 상단
    vtx[1].x = x + width / 2.0f + messageDialogNS::BUTTON_WIDTH / 2.0f;
    vtx[1].y = vtx[0].y;
    vtx[1].z = 0.0f;
    vtx[1].rhw = 1.0f;
    vtx[1].color = buttonColor;

    // 버튼 오른쪽 하단
    vtx[2].x = vtx[1].x;
    vtx[2].y = vtx[0].y + messageDialogNS::BUTTON_HEIGHT;
    vtx[2].z = 0.0f;
    vtx[2].rhw = 1.0f;
    vtx[2].color = buttonColor;

    // 버튼 왼쪽 하단
    vtx[3].x = vtx[0].x;
    vtx[3].y = vtx[2].y;
    vtx[3].z = 0.0f;
    vtx[3].rhw = 1.0f;
    vtx[3].color = buttonColor;

    graphics->createVertexBuffer(vtx, sizeof vtx, buttonVerts);

    // buttonRect 설정
    buttonRect.left = (long)vtx[0].x;
    buttonRect.right = (long)vtx[1].x;
    buttonRect.top = (long)vtx[0].y;
    buttonRect.bottom = (long)vtx[2].y;

    // 버튼2 왼쪽 상단
    vtx[0].x = x + width - messageDialogNS::BUTTON_WIDTH * 1.2f;
    vtx[0].y = y + height - messageDialogNS::BORDER - messageDialogNS::MARGIN - messageDialogNS::BUTTON_HEIGHT;
    vtx[0].z = 0.0f;
    vtx[0].rhw = 1.0f;
    vtx[0].color = buttonColor;
    // 버튼2 오른쪽 상단
    vtx[1].x = vtx[0].x + messageDialogNS::BUTTON_WIDTH;
    vtx[1].y = vtx[0].y;
    vtx[1].z = 0.0f;
    vtx[1].rhw = 1.0f;
    vtx[1].color = buttonColor;
    // 버튼2 오른쪽 하단
    vtx[2].x = vtx[1].x;
    vtx[2].y = vtx[0].y + messageDialogNS::BUTTON_HEIGHT;
    vtx[2].z = 0.0f;
    vtx[2].rhw = 1.0f;
    vtx[2].color = buttonColor;
    // 버튼2 왼쪽 하단
    vtx[3].x = vtx[0].x;
    vtx[3].y = vtx[2].y;
    vtx[3].z = 0.0f;
    vtx[3].rhw = 1.0f;
    vtx[3].color = buttonColor;
    graphics->createVertexBuffer(vtx, sizeof vtx, button2Verts);

    // button2Rect 설정
    button2Rect.left = (long)vtx[0].x;
    button2Rect.right = (long)vtx[1].x;
    button2Rect.top = (long)vtx[0].y;
    button2Rect.bottom = (long)vtx[2].y;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// MessageDialog 그리기
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
const void MessageDialog::draw()
{
    if (!visible || graphics == NULL || !initialized)
        return;

    graphics->drawQuad(borderVerts);                                         // 테두리 그리기
    graphics->drawQuad(dialogVerts);                                         // 배경 그리기
    graphics->drawQuad(buttonVerts);                                         // 버튼 그리기
    graphics->drawQuad(button2Verts);                                        // 버튼2 그리기

    graphics->spriteBegin();                                                 // 스프라이트 그리기 시작

    if (text.size() == 0)
        return;
    // MessageDialog에 텍스트 표시
    dxFont.setFontColor(fontColor);
    dxFont.print(text, textRect, DT_CENTER | DT_WORDBREAK);

    // 버튼에 텍스트 표시
    dxFont.setFontColor(buttonFontColor);
    dxFont.print(messageDialogNS::BUTTON1_TEXT[buttonType], buttonRect,
        DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    dxFont.print(messageDialogNS::BUTTON2_TEXT[buttonType], button2Rect,
        DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    graphics->spriteEnd();                                                   // 스프라이트 그리기 종료
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// DIALOG_CLOSE_KEY 및 확인 버튼 클릭 확인
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void MessageDialog::update()
{
    if (!initialized || !visible)
        return;
    if (input->wasKeyPressed(messageDialogNS::DIALOG_CLOSE_KEY))
    {
        visible = false;
        buttonClicked = 1;                                                   // button1이 클릭됨
        return;
    }

    if (graphics->getFullscreen() == false)                                  // 창 모드인 경우
    {
        // 창 크기가 조절된 경우 화면 비율 계산
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        screenRatioX = (float)GAME_WIDTH / clientRect.right;
        screenRatioY = (float)GAME_HEIGHT / clientRect.bottom;
    }

    if (input->getMouseLButton())                                            // 마우스 왼쪽 버튼인 경우
    {
        // 마우스가 버튼1(확인) 내부를 클릭한 경우
        if (input->getMouseX() * screenRatioX >= buttonRect.left &&
            input->getMouseX() * screenRatioX <= buttonRect.right &&
            input->getMouseY() * screenRatioY >= buttonRect.top &&
            input->getMouseY() * screenRatioY <= buttonRect.bottom)
        {
            visible = false;                                                 // 메시지 대화 상자 숨기기
            buttonClicked = 1;                                               // button1이 클릭됨
            return;
        }

        // 마우스가 버튼2(취소) 내부를 클릭한 경우
        if (input->getMouseX() * screenRatioX >= button2Rect.left &&
            input->getMouseX() * screenRatioX <= button2Rect.right &&
            input->getMouseY() * screenRatioY >= button2Rect.top &&
            input->getMouseY() * screenRatioY <= button2Rect.bottom)
        {
            visible = false;                                                 // 메시지 대화 상자 숨기기
            buttonClicked = 2;                                               // button2가 클릭됨
        }
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 텍스트 문자열을 설정하고, 텍스트에 맞게 대화 상자 하단 크기를 조정하고, visible = true로 설정
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void MessageDialog::print(const std::string& str)
{
    if (!initialized || visible)                                             // 초기화되지 않았거나 이미 사용 중인 경우
        return;
    text = str + "\n\n\n\n";                                                 // 버튼을 위한 공간 남기기

    // textRect를 대화 상자의 텍스트 영역으로 설정
    textRect.left = (long)(x + messageDialogNS::MARGIN);
    textRect.right = (long)(x + messageDialogNS::WIDTH - messageDialogNS::MARGIN);
    textRect.top = (long)(y + messageDialogNS::MARGIN);
    textRect.bottom = (long)(y + messageDialogNS::HEIGHT - messageDialogNS::MARGIN);

    // 텍스트에 필요한 정확한 높이로 textRect.bottom 설정
    // DT_CALDRECT 옵션으로는 텍스트가 출력되지 않음.
    dxFont.print(text, textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
    height = textRect.bottom - (int)y + messageDialogNS::BORDER + messageDialogNS::MARGIN;

    prepareVerts();                                                          // 정점 버퍼 준비
    buttonClicked = 0;                                                       // buttonClicked 지우기
    visible = true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 그래픽 장치를 잃어버렸을 때 호출됨
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void MessageDialog::onLostDevice()
{
    if (!initialized)
        return;
    dxFont.onLostDevice();
    SAFE_RELEASE(dialogVerts);
    SAFE_RELEASE(borderVerts);
    SAFE_RELEASE(buttonVerts);
    SAFE_RELEASE(button2Verts);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 그래픽 장치가 재설정될 때 호출됨
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void MessageDialog::onResetDevice()
{
    if (!initialized)
        return;
    prepareVerts();
    dxFont.onResetDevice();
}
