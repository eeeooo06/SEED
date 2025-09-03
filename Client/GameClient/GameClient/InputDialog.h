#ifndef _INPUTDIALOG_H                                                       // �� ������ �� �� �̻� ���Ե� ��� ���� ���Ǹ� �����մϴ�.
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
    const COLOR_ARGB TEXT_BACK_COLOR = GraphicsNS::WHITE;                    // �Է� �ؽ�Ʈ ���
    const COLOR_ARGB TEXT_COLOR = GraphicsNS::BLACK;                         // �Է� �ؽ�Ʈ ����
}

// �Է� ��ȭ����, MessageDialog�κ��� ��ӹ���
class InputDialog : public MessageDialog
{
private:
    std::string inText;                                                      // �Է� �ؽ�Ʈ
    RECT        inTextRect;
    RECT        tempRect;
    COLOR_ARGB  textBackColor;                                               // �ؽ�Ʈ ���� ����
    COLOR_ARGB  textFontColor;                                               // �ؽ�Ʈ ���� �۲û�
    LP_VERTEXBUFFER inTextVerts;                                             // �ؽ�Ʈ ���� ���� ����

public:
    // ������
    InputDialog();
    // �Ҹ���
    virtual ~InputDialog();

    // ���� ���� �غ�
    void prepareVerts();

    // InputDialog�� ǥ���մϴ�.
    const void draw();

    // �Է� �ؽ�Ʈ�� ��ȯ�մϴ�.
    std::string getText()
    {
        if (!visible)
            return inText;
        else
            return "";
    }

    // �Է� �ؽ�Ʈ �۲û� ����
    void setTextFontColor(COLOR_ARGB fc) { textFontColor = fc; }

    // �Է� �ؽ�Ʈ ���� ����
    void setTextBackColor(COLOR_ARGB bc) { textBackColor = bc; }

    // InputDialog�� �ؽ�Ʈ str ǥ��
    void print(const std::string& str);

    // �ݱ� �̺�Ʈ Ȯ��
    void update();

    // �׷��� ��ġ�� �Ҿ������ �� ȣ��˴ϴ�.
    void onLostDevice();
};

#endif