#ifndef _MESSAGEDIALOG_H                                                     // �� ������ �� �� �̻� ���Ե� ���
#define _MESSAGEDIALOG_H                                                     // ���� ���Ǹ� �����մϴ�.
#define WIN32_LEAN_AND_MEAN

#include <string>
#include "Constants.h"
#include "TextDX.h"
#include "Graphics.h"
#include "Input.h"

namespace messageDialogNS
{
    const UINT WIDTH = 400;                                                  // ��ȭ ���� �⺻ �ʺ�
    const UINT HEIGHT = 100;                                                 // �⺻ ����
    const UINT BORDER = 5;
    const UINT MARGIN = 5;                                                   // �׵θ��κ����� �ؽ�Ʈ ����
    const char FONT[] = "Arial";                                             // �۲�
    const int FONT_HEIGHT = 18;                                              // �۲� ����
    const COLOR_ARGB FONT_COLOR = GraphicsNS::WHITE;                         // �ؽ�Ʈ ����
    const COLOR_ARGB BORDER_COLOR = D3DCOLOR_ARGB(192, 192, 192, 192);       // �׵θ� ����
    const COLOR_ARGB BACK_COLOR = SETCOLOR_ARGB(255, 100, 100, 192);         // ����
    const UINT X = GAME_WIDTH / 2 - WIDTH / 2;                               // �⺻ ��ġ
    const UINT Y = GAME_HEIGHT / 4 - HEIGHT / 2;
    const UINT BUTTON_WIDTH = (UINT)(FONT_HEIGHT * 4.5);
    const UINT BUTTON_HEIGHT = FONT_HEIGHT + 4;
    const int MAX_TYPE = 2;
    const int OK_CANCEL = 0;                                                 // Ȯ��/��� ��ư ����
    const int YES_NO = 1;                                                    // ��/�ƴϿ� ��ư ����
    static const char* BUTTON1_TEXT[MAX_TYPE] = { "OK", "YES" };
    static const char* BUTTON2_TEXT[MAX_TYPE] = { "CANCEL", "NO" };
    const byte DIALOG_CLOSE_KEY = VK_RETURN;                                 // Enter Ű
    const COLOR_ARGB BUTTON_COLOR = GraphicsNS::GRAY;                        // ��ư ���
    const COLOR_ARGB BUTTON_FONT_COLOR = GraphicsNS::WHITE;                  // ��ư �ؽ�Ʈ ����
}

// �޽��� ��ȭ ����
class MessageDialog
{
protected:
    Graphics* graphics;                                                      // �׷��� �ý���
    Input* input;                                                            // �Է� �ý���
    TextDX      dxFont;                                                      // DirectX �۲�
    float       x, y;                                                        // ȭ�� ��ġ
    UINT        height;                                                      // ��ȭ ���� ����, print()���� ����
    UINT        width;                                                       // ��ȭ ���� �ʺ�
    std::string text;                                                        // ��ȭ ���� �ؽ�Ʈ
    RECT        textRect;                                                    // �ؽ�Ʈ �簢��
    RECT        buttonRect;                                                  // ��ư �簢��
    RECT        button2Rect;                                                 // ��ư2 �簢��
    COLOR_ARGB  fontColor;                                                   // �۲� ���� (a,r,g,b)
    COLOR_ARGB  borderColor;                                                 // �׵θ� ���� (a,r,g,b)
    COLOR_ARGB  backColor;                                                   // ��� ���� (a,r,g,b)
    COLOR_ARGB  buttonColor;                                                 // ��ư ����
    COLOR_ARGB  buttonFontColor;                                             // ��ư �۲� ����
    VertexC vtx[4];                                                          // ���� ������
    LP_VERTEXBUFFER dialogVerts;                                             // ��ȭ ���� ���� ����
    LP_VERTEXBUFFER borderVerts;                                             // �׵θ� ���� ����
    LP_VERTEXBUFFER buttonVerts;                                             // ��ư ���� ����
    LP_VERTEXBUFFER button2Verts;                                            // ��ư2 ���� ����
    int buttonClicked;                                                       // Ŭ���� ��ư (1 �Ǵ� 2)
    int buttonType;                                                          // 0 = Ȯ��/���, 1 = ��/�ƴϿ�
    bool    initialized;                                                     // ���������� �ʱ�ȭ�Ǹ� true
    bool    visible;                                                         // ǥ���Ϸ��� true
    HWND    hwnd;                                                            // â �ڵ�
    float   screenRatioX, screenRatioY;

public:
    // ������
    MessageDialog();
    // �Ҹ���
    virtual ~MessageDialog();

    // MessageDialog�� �ʱ�ȭ�մϴ�.
    // ���� ����: 
    //          *g�� Graphics ��ü�� ����ŵ�ϴ�.
    //          *in�� Input ��ü�� ����ŵ�ϴ�.
    //          hwnd = â �ڵ�
    bool initialize(Graphics* g, Input* in, HWND hwnd);

    // ���� ���� �غ�
    void prepareVerts();

    // MessageDialog�� ǥ���մϴ�.
    const void draw();

    // Ŭ���� ��ư ��ȯ
    // 0 = Ŭ���� ��ư ����
    // 1�� ���� ��ư, 2�� ������ ��ư
    int getButtonClicked() { return buttonClicked; }

    // visible ��ȯ
    bool getVisible() { return visible; }

    // �۲� ���� ����
    void setFontColor(COLOR_ARGB fc) { fontColor = fc; }

    // �׵θ� ���� ����
    void setBorderColor(COLOR_ARGB bc) { borderColor = bc; }

    // ���� ����
    void setBackColor(COLOR_ARGB bc) { backColor = bc; }

    // ��ư ���� ����
    void setButtonColor(COLOR_ARGB bc) { buttonColor = bc; }

    // ��ư �۲� ���� ����
    void setButtonFontColor(COLOR_ARGB bfc) { buttonFontColor = bfc; }

    // visible ����;
    void setVisible(bool v) { visible = v; }

    // ��ư ���� ���� 0 = Ȯ��/���, 1 = ��/�ƴϿ�
    void setButtonType(UINT t)
    {
        if (t < messageDialogNS::MAX_TYPE)
            buttonType = t;
    }

    // MessageDialog�� �ؽ�Ʈ str ǥ��
    void print(const std::string& str);

    // �ݱ� �̺�Ʈ Ȯ��
    void update();

    // �׷��� ��ġ�� �Ҿ������ �� ȣ��˴ϴ�.
    void onLostDevice();

    // �׷��� ��ġ�� �缳���� �� ȣ��˴ϴ�.
    void onResetDevice();
};

#endif