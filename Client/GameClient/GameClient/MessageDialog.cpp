#include "MessageDialog.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ������
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
MessageDialog::MessageDialog()
{
    initialized = false;                                                     // ���������� �ʱ�ȭ�Ǹ� true�� ����
    graphics = NULL;
    visible = false;                                                         // ������ ����
    fontColor = messageDialogNS::FONT_COLOR;
    borderColor = messageDialogNS::BORDER_COLOR;
    backColor = messageDialogNS::BACK_COLOR;
    buttonColor = messageDialogNS::BUTTON_COLOR;
    buttonFontColor = messageDialogNS::BUTTON_FONT_COLOR;
    x = messageDialogNS::X;                                                  // ���� ��ġ
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
    buttonType = 0;                                                          // Ȯ��/���
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �Ҹ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
MessageDialog::~MessageDialog()
{
    onLostDevice();                                                          // ��� �׷��� �׸� ���� onLostDevice() ȣ��
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// MessageDialog �ʱ�ȭ
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool MessageDialog::initialize(Graphics* g, Input* in, HWND h)
{
    try {
        graphics = g;                                                        // �׷��� ��ü
        input = in;                                                          // �Է� ��ü
        hwnd = h;

        // DirectX �۲� �ʱ�ȭ
        if (dxFont.initialize(graphics, messageDialogNS::FONT_HEIGHT, false,
            false, messageDialogNS::FONT) == false)
            return false;                                                    // ������ ���
        dxFont.setFontColor(fontColor);
    }
    catch (...) {
        return false;
    }

    initialized = true;
    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ��ȭ ���� ��� �� ��ư�� �׸��� ���� ���� ���� �غ�
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void MessageDialog::prepareVerts()
{
    SAFE_RELEASE(dialogVerts);
    SAFE_RELEASE(borderVerts);
    SAFE_RELEASE(buttonVerts);
    SAFE_RELEASE(button2Verts);

    // �׵θ� ���� ���
    vtx[0].x = x;
    vtx[0].y = y;
    vtx[0].z = 0.0f;
    vtx[0].rhw = 1.0f;
    vtx[0].color = borderColor;

    // �׵θ� ������ ���
    vtx[1].x = x + width;
    vtx[1].y = y;
    vtx[1].z = 0.0f;
    vtx[1].rhw = 1.0f;
    vtx[1].color = borderColor;

    // �׵θ� ������ �ϴ�
    vtx[2].x = x + width;
    vtx[2].y = y + height;
    vtx[2].z = 0.0f;
    vtx[2].rhw = 1.0f;
    vtx[2].color = borderColor;

    // �׵θ� ���� �ϴ�
    vtx[3].x = x;
    vtx[3].y = y + height;
    vtx[3].z = 0.0f;
    vtx[3].rhw = 1.0f;
    vtx[3].color = borderColor;

    graphics->createVertexBuffer(vtx, sizeof vtx, borderVerts);

    // ��� ���� ���
    vtx[0].x = x + messageDialogNS::BORDER;
    vtx[0].y = y + messageDialogNS::BORDER;
    vtx[0].z = 0.0f;
    vtx[0].rhw = 1.0f;
    vtx[0].color = backColor;

    // ��� ������ ���
    vtx[1].x = x + width - messageDialogNS::BORDER;
    vtx[1].y = y + messageDialogNS::BORDER;
    vtx[1].z = 0.0f;
    vtx[1].rhw = 1.0f;
    vtx[1].color = backColor;

    // ��� ������ �ϴ�
    vtx[2].x = x + width - messageDialogNS::BORDER;
    vtx[2].y = y + height - messageDialogNS::BORDER;
    vtx[2].z = 0.0f;
    vtx[2].rhw = 1.0f;
    vtx[2].color = backColor;

    // ��� ���� �ϴ�
    vtx[3].x = x + messageDialogNS::BORDER;
    vtx[3].y = y + height - messageDialogNS::BORDER;
    vtx[3].z = 0.0f;
    vtx[3].rhw = 1.0f;
    vtx[3].color = backColor;

    graphics->createVertexBuffer(vtx, sizeof vtx, dialogVerts);

    // ��ư ���� ���
    vtx[0].x = x + width / 2.0f - messageDialogNS::BUTTON_WIDTH / 2.0f;
    vtx[0].y = y + height - messageDialogNS::BORDER - messageDialogNS::MARGIN - messageDialogNS::BUTTON_HEIGHT;
    vtx[0].z = 0.0f;
    vtx[0].rhw = 1.0f;
    vtx[0].color = buttonColor;

    // ��ư ������ ���
    vtx[1].x = x + width / 2.0f + messageDialogNS::BUTTON_WIDTH / 2.0f;
    vtx[1].y = vtx[0].y;
    vtx[1].z = 0.0f;
    vtx[1].rhw = 1.0f;
    vtx[1].color = buttonColor;

    // ��ư ������ �ϴ�
    vtx[2].x = vtx[1].x;
    vtx[2].y = vtx[0].y + messageDialogNS::BUTTON_HEIGHT;
    vtx[2].z = 0.0f;
    vtx[2].rhw = 1.0f;
    vtx[2].color = buttonColor;

    // ��ư ���� �ϴ�
    vtx[3].x = vtx[0].x;
    vtx[3].y = vtx[2].y;
    vtx[3].z = 0.0f;
    vtx[3].rhw = 1.0f;
    vtx[3].color = buttonColor;

    graphics->createVertexBuffer(vtx, sizeof vtx, buttonVerts);

    // buttonRect ����
    buttonRect.left = (long)vtx[0].x;
    buttonRect.right = (long)vtx[1].x;
    buttonRect.top = (long)vtx[0].y;
    buttonRect.bottom = (long)vtx[2].y;

    // ��ư2 ���� ���
    vtx[0].x = x + width - messageDialogNS::BUTTON_WIDTH * 1.2f;
    vtx[0].y = y + height - messageDialogNS::BORDER - messageDialogNS::MARGIN - messageDialogNS::BUTTON_HEIGHT;
    vtx[0].z = 0.0f;
    vtx[0].rhw = 1.0f;
    vtx[0].color = buttonColor;
    // ��ư2 ������ ���
    vtx[1].x = vtx[0].x + messageDialogNS::BUTTON_WIDTH;
    vtx[1].y = vtx[0].y;
    vtx[1].z = 0.0f;
    vtx[1].rhw = 1.0f;
    vtx[1].color = buttonColor;
    // ��ư2 ������ �ϴ�
    vtx[2].x = vtx[1].x;
    vtx[2].y = vtx[0].y + messageDialogNS::BUTTON_HEIGHT;
    vtx[2].z = 0.0f;
    vtx[2].rhw = 1.0f;
    vtx[2].color = buttonColor;
    // ��ư2 ���� �ϴ�
    vtx[3].x = vtx[0].x;
    vtx[3].y = vtx[2].y;
    vtx[3].z = 0.0f;
    vtx[3].rhw = 1.0f;
    vtx[3].color = buttonColor;
    graphics->createVertexBuffer(vtx, sizeof vtx, button2Verts);

    // button2Rect ����
    button2Rect.left = (long)vtx[0].x;
    button2Rect.right = (long)vtx[1].x;
    button2Rect.top = (long)vtx[0].y;
    button2Rect.bottom = (long)vtx[2].y;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// MessageDialog �׸���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
const void MessageDialog::draw()
{
    if (!visible || graphics == NULL || !initialized)
        return;

    graphics->drawQuad(borderVerts);                                         // �׵θ� �׸���
    graphics->drawQuad(dialogVerts);                                         // ��� �׸���
    graphics->drawQuad(buttonVerts);                                         // ��ư �׸���
    graphics->drawQuad(button2Verts);                                        // ��ư2 �׸���

    graphics->spriteBegin();                                                 // ��������Ʈ �׸��� ����

    if (text.size() == 0)
        return;
    // MessageDialog�� �ؽ�Ʈ ǥ��
    dxFont.setFontColor(fontColor);
    dxFont.print(text, textRect, DT_CENTER | DT_WORDBREAK);

    // ��ư�� �ؽ�Ʈ ǥ��
    dxFont.setFontColor(buttonFontColor);
    dxFont.print(messageDialogNS::BUTTON1_TEXT[buttonType], buttonRect,
        DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    dxFont.print(messageDialogNS::BUTTON2_TEXT[buttonType], button2Rect,
        DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    graphics->spriteEnd();                                                   // ��������Ʈ �׸��� ����
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// DIALOG_CLOSE_KEY �� Ȯ�� ��ư Ŭ�� Ȯ��
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void MessageDialog::update()
{
    if (!initialized || !visible)
        return;
    if (input->wasKeyPressed(messageDialogNS::DIALOG_CLOSE_KEY))
    {
        visible = false;
        buttonClicked = 1;                                                   // button1�� Ŭ����
        return;
    }

    if (graphics->getFullscreen() == false)                                  // â ����� ���
    {
        // â ũ�Ⱑ ������ ��� ȭ�� ���� ���
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        screenRatioX = (float)GAME_WIDTH / clientRect.right;
        screenRatioY = (float)GAME_HEIGHT / clientRect.bottom;
    }

    if (input->getMouseLButton())                                            // ���콺 ���� ��ư�� ���
    {
        // ���콺�� ��ư1(Ȯ��) ���θ� Ŭ���� ���
        if (input->getMouseX() * screenRatioX >= buttonRect.left &&
            input->getMouseX() * screenRatioX <= buttonRect.right &&
            input->getMouseY() * screenRatioY >= buttonRect.top &&
            input->getMouseY() * screenRatioY <= buttonRect.bottom)
        {
            visible = false;                                                 // �޽��� ��ȭ ���� �����
            buttonClicked = 1;                                               // button1�� Ŭ����
            return;
        }

        // ���콺�� ��ư2(���) ���θ� Ŭ���� ���
        if (input->getMouseX() * screenRatioX >= button2Rect.left &&
            input->getMouseX() * screenRatioX <= button2Rect.right &&
            input->getMouseY() * screenRatioY >= button2Rect.top &&
            input->getMouseY() * screenRatioY <= button2Rect.bottom)
        {
            visible = false;                                                 // �޽��� ��ȭ ���� �����
            buttonClicked = 2;                                               // button2�� Ŭ����
        }
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �ؽ�Ʈ ���ڿ��� �����ϰ�, �ؽ�Ʈ�� �°� ��ȭ ���� �ϴ� ũ�⸦ �����ϰ�, visible = true�� ����
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void MessageDialog::print(const std::string& str)
{
    if (!initialized || visible)                                             // �ʱ�ȭ���� �ʾҰų� �̹� ��� ���� ���
        return;
    text = str + "\n\n\n\n";                                                 // ��ư�� ���� ���� �����

    // textRect�� ��ȭ ������ �ؽ�Ʈ �������� ����
    textRect.left = (long)(x + messageDialogNS::MARGIN);
    textRect.right = (long)(x + messageDialogNS::WIDTH - messageDialogNS::MARGIN);
    textRect.top = (long)(y + messageDialogNS::MARGIN);
    textRect.bottom = (long)(y + messageDialogNS::HEIGHT - messageDialogNS::MARGIN);

    // �ؽ�Ʈ�� �ʿ��� ��Ȯ�� ���̷� textRect.bottom ����
    // DT_CALDRECT �ɼ����δ� �ؽ�Ʈ�� ��µ��� ����.
    dxFont.print(text, textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
    height = textRect.bottom - (int)y + messageDialogNS::BORDER + messageDialogNS::MARGIN;

    prepareVerts();                                                          // ���� ���� �غ�
    buttonClicked = 0;                                                       // buttonClicked �����
    visible = true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �׷��� ��ġ�� �Ҿ������ �� ȣ���
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
// �׷��� ��ġ�� �缳���� �� ȣ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void MessageDialog::onResetDevice()
{
    if (!initialized)
        return;
    prepareVerts();
    dxFont.onResetDevice();
}
