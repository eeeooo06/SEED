#ifndef _CONSOLE_H                                                           // �� ������ �� �� �̻� ���Ե� ��� ���� ���Ǹ� �����մϴ�.
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
    const UINT WIDTH = 500;                                                  // �ܼ� �ʺ�
    const UINT HEIGHT = 400;                                                 // �ܼ� ����
    const UINT X = 5;                                                        // �ܼ� ��ġ
    const UINT Y = 5;
    const UINT MARGIN = 4;                                                   // �ܼ� �����ڸ��κ����� �ؽ�Ʈ ����
    const char FONT[] = "Courier New";                                       // �ܼ� �۲�
    const int FONT_HEIGHT = 14;                                              // �۲� ���� (�ȼ� ����)
    const COLOR_ARGB FONT_COLOR = GraphicsNS::WHITE;                         // �ܼ� �ؽ�Ʈ ����
    const COLOR_ARGB BACK_COLOR = SETCOLOR_ARGB(192, 128, 128, 128);         // ����
    const int MAX_LINES = 256;                                               // �ؽ�Ʈ ������ �ִ� �� ��
}

// 
class Console
{
private:
    Graphics* graphics;                                                      // �׷��� �ý���
    Input* input;                                                            // �Է� �ý���
    TextDX      dxFont;                                                      // DirectX �۲�
    float       x, y;                                                        // �ܼ� ��ġ (����)
    int         rows;                                                        // �ֿܼ� �� �ؽ�Ʈ �� ��
    std::string commandStr;                                                  // �ܼ� ��ɾ�
    std::string inputStr;                                                    // �ܼ� �ؽ�Ʈ �Է�
    std::deque<std::string> text;                                            // �ܼ� �ؽ�Ʈ
    RECT        textRect;                                                    // �ؽ�Ʈ �簢��
    COLOR_ARGB  fontColor;                                                   // �۲� ���� (a,r,g,b)
    COLOR_ARGB  backColor;                                                   // ��� ���� (a,r,g,b)
    VertexC vtx[4];                                                          // ����� ���� ���� ������
    LP_VERTEXBUFFER vertexBuffer;                                            // ���� �����͸� ���� ����
    int         scrollAmount;                                                // ���÷��̸� ���� ��ũ���� �� ��
    bool        initialized;                                                 // ���������� �ʱ�ȭ�Ǹ� true
    bool        visible;                                                     // ǥ���Ϸ��� true

public:
    // ������
    Console();

    // �Ҹ���
    virtual ~Console();

    // �ܼ� �ʱ�ȭ
    // ���� ����: 
    //          *g�� Graphics�� ����ŵ�ϴ�.
    //          *in�� Input�� ����ŵ�ϴ�.
    bool initialize(Graphics* g, Input* in);

    // �ܼ��� ǥ���մϴ�.
    const void draw();

    // �ܼ� ���̱�/�����
    void showHide();

    // visible�� ��ȯ�մϴ�.
    bool getVisible() { return visible; }

    // visible = true�� ����;
    void show() { visible = true; }

    // visible = false�� ����;
    void hide() { visible = false; }

    // �ܼ� ���÷��̿� �ؽ�Ʈ str�� �߰��մϴ�.
    // str�� ù ��° �ٸ� ǥ�õ˴ϴ�.
    void print(const std::string& str);

    // �ܼ� ��ɾ ��ȯ�մϴ�.
    std::string getCommand();

    // �ܼ� �Է� �ؽ�Ʈ�� ��ȯ�մϴ�.
    std::string getInput() { return inputStr; }

    // �Է� �ؽ�Ʈ �����
    void clearInput() { inputStr = ""; }

    // �׷��� ��ġ�� �Ҿ������ �� ȣ��˴ϴ�.
    void onLostDevice();

    // �׷��� ��ġ�� �缳���� �� ȣ��˴ϴ�.
    void onResetDevice();
};

#endif