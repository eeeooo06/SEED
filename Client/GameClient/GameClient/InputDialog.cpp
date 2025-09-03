#include "InputDialog.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ������
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
InputDialog::InputDialog()
{
    textBackColor = inputDialogNS::TEXT_BACK_COLOR;
    textFontColor = inputDialogNS::TEXT_COLOR;
    inTextVerts = NULL;
    inText = "";
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �Ҹ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
InputDialog::~InputDialog()
{
    onLostDevice();                                                          // ��� �׷��� �׸� ���� onLostDevice() ȣ��
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
// ��ȭ ���� ��� �� ��ư �׸��⸦ ���� ���� ���� �غ�                             
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
void InputDialog::prepareVerts()
{
    MessageDialog::prepareVerts();                                           // �⺻ Ŭ�������� prepareVerts ȣ��
    SAFE_RELEASE(inTextVerts);

    // inText ���� ���
    vtx[0].x = x + messageDialogNS::BORDER * 2;
    vtx[0].y = y + height - messageDialogNS::BORDER - messageDialogNS::MARGIN - messageDialogNS::BUTTON_HEIGHT * 2.5f;
    vtx[0].z = 0.0f;
    vtx[0].rhw = 1.0f;
    vtx[0].color = textBackColor;
    // inText ������ ���
    vtx[1].x = x + width - messageDialogNS::BORDER * 2;
    vtx[1].y = vtx[0].y;
    vtx[1].z = 0.0f;
    vtx[1].rhw = 1.0f;
    vtx[1].color = textBackColor;
    // inText ������ �ϴ�
    vtx[2].x = vtx[1].x;
    vtx[2].y = vtx[0].y + messageDialogNS::BUTTON_HEIGHT;
    vtx[2].z = 0.0f;
    vtx[2].rhw = 1.0f;
    vtx[2].color = textBackColor;
    // inText ���� �ϴ�
    vtx[3].x = vtx[0].x;
    vtx[3].y = vtx[2].y;
    vtx[3].z = 0.0f;
    vtx[3].rhw = 1.0f;
    vtx[3].color = textBackColor;
    graphics->createVertexBuffer(vtx, sizeof vtx, inTextVerts);

    // inTextRect ����
    inTextRect.left = (long)vtx[0].x;
    inTextRect.right = (long)vtx[1].x;
    inTextRect.top = (long)vtx[0].y;
    inTextRect.bottom = (long)vtx[2].y;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// InputDialog �׸���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
const void InputDialog::draw()
{
    if (!visible || graphics == NULL || !initialized)
        return;

    graphics->drawQuad(borderVerts);                                         // �׵θ� �׸���
    graphics->drawQuad(dialogVerts);                                         // ��� �׸���
    graphics->drawQuad(buttonVerts);                                         // ��ư �׸���
    graphics->drawQuad(button2Verts);                                        // ��ư2 �׸���
    graphics->drawQuad(inTextVerts);                                         // �Է� �ؽ�Ʈ ���� �׸���

    graphics->spriteBegin();                                                 // ��������Ʈ �׸��� ����

    if (text.size() == 0)
        return;
    // MessageDialog�� �ؽ�Ʈ ǥ��
    dxFont.setFontColor(fontColor);
    dxFont.print(text, textRect, DT_CENTER | DT_WORDBREAK);

    // ��ư�� �ؽ�Ʈ ǥ��
    dxFont.setFontColor(buttonFontColor);
    dxFont.print(messageDialogNS::BUTTON1_TEXT[buttonType], buttonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    dxFont.print(messageDialogNS::BUTTON2_TEXT[buttonType], button2Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // �Է� �ؽ�Ʈ ǥ��
    dxFont.setFontColor(textFontColor);
    tempRect = inTextRect;                                                   // ����
    // DT_CALDRECT �ɼ����δ� �ؽ�Ʈ�� ��µ��� �ʽ��ϴ�. RECT.right�� �̵���ŵ�ϴ�.
    dxFont.print(inText, tempRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_CALCRECT);
    if (tempRect.right > inTextRect.right)                                   // �ؽ�Ʈ�� �ʹ� ��� ������ ����
        dxFont.print(inText, inTextRect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
    else                                                                     // �׷��� ������ ���� ����
        dxFont.print(inText, inTextRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

    graphics->spriteEnd();                                                   // ��������Ʈ �׸��� ����
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
// DIALOG_CLOSE_KEY �� ��ư Ŭ�� Ȯ��                                          
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====               
void InputDialog::update()
{
    MessageDialog::update();                                                 // �⺻ Ŭ�������� update ȣ��
    if (!initialized || !visible)
    {
        if (buttonClicked == 2)                                              // ��� ��ư�� ���
            inText = "";                                                     // �Է� �ؽ�Ʈ �����
        return;
    }
    inText = input->getTextIn();                                             // �Է� �ؽ�Ʈ ��������
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �ؽ�Ʈ ���ڿ��� �����ϰ�, �ؽ�Ʈ�� �°� ��ȭ ���� �ϴ� ũ�⸦ �����ϰ�, visible = true�� �����մϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void InputDialog::print(const std::string& str)
{
    if (!initialized || visible)                                             // �ʱ�ȭ���� �ʾҰų� �̹� ��� ���� ���
        return;
    text = str + "\n\n\n\n\n";                                               // �Է� �ؽ�Ʈ�� ��ư�� ���� ���� �����

    // textRect�� ��ȭ ������ �ؽ�Ʈ �������� ����
    textRect.left = (long)(x + messageDialogNS::MARGIN);
    textRect.right = (long)(x + messageDialogNS::WIDTH - messageDialogNS::MARGIN);
    textRect.top = (long)(y + messageDialogNS::MARGIN);
    textRect.bottom = (long)(y + messageDialogNS::HEIGHT - messageDialogNS::MARGIN);

    // textRect.bottom�� �ؽ�Ʈ�� �ʿ��� ��Ȯ�� ���̷� ����
    // DT_CALDRECT �ɼ����δ� �ؽ�Ʈ�� ��µ��� �ʽ��ϴ�.
    dxFont.print(text, textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
    height = textRect.bottom - (int)y + messageDialogNS::BORDER + messageDialogNS::MARGIN;

    prepareVerts();                                                          // ���� ���� �غ�
    inText = "";                                                             // ���� �Է� �����
    input->clearTextIn();
    buttonClicked = 0;                                                       // buttonClicked �����
    visible = true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �׷��� ��ġ�� �Ҿ������ �� ȣ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void InputDialog::onLostDevice()
{
    if (!initialized)
        return;
    MessageDialog::onLostDevice();
    SAFE_RELEASE(inTextVerts);
}