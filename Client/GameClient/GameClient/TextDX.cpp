#include "TextDX.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ������
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
TextDX::TextDX()
{
    color = SETCOLOR_ARGB(255, 255, 255, 255);                               // �⺻���� ��� �۲�

    // �۲� ��ġ ����
    fontRect.top = 0;
    fontRect.left = 0;
    fontRect.right = GAME_WIDTH;
    fontRect.bottom = GAME_HEIGHT;
    dxFont = NULL;
    angle = 0;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �Ҹ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
TextDX::~TextDX()
{
    SAFE_RELEASE(dxFont);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// DirectX �۲� ����
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool TextDX::initialize(Graphics* g, int height, bool bold, bool italic,
    const std::string& fontName)
{
    graphics = g;                                                            // �׷��� �ý���

    UINT weight = FW_NORMAL;
    if (bold)
        weight = FW_BOLD;

    // DirectX �۲� ����
    if (FAILED(D3DXCreateFont(graphics->get3Ddevice(), height, 0, weight, 1, italic,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, fontName.c_str(),
        &dxFont))) return false;

    // ��ȯ ��� ����
    D3DXMatrixTransformation2D(&matrix, NULL, 0.0f, NULL, NULL, 0.0f, NULL);

    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// x,y�� �ؽ�Ʈ ���
// ���� �� 0, ���� �� �ؽ�Ʈ ���� ��ȯ
// ���� ����: 
//          spriteBegin()
// ���� ����: 
//          spriteEnd()
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
int TextDX::print(const std::string& str, int x, int y)
{
    if (dxFont == NULL)
        return 0;
    // �۲� ��ġ ����
    fontRect.top = y;
    fontRect.left = x;

    // ȸ�� �߽�
    D3DXVECTOR2 rCenter = D3DXVECTOR2((float)x, (float)y);
    // ������ŭ �ؽ�Ʈ�� ȸ����Ű���� ��� ����
    D3DXMatrixTransformation2D(&matrix, NULL, 0.0f, NULL, &rCenter, angle, NULL);
    // ��������Ʈ���� ��Ŀ� ���� �˸� "Hello Neo"
    graphics->getSprite()->SetTransform(&matrix);
    return dxFont->DrawTextA(graphics->getSprite(), str.c_str(), -1, &fontRect, DT_LEFT, color);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// DirectX �ؽ�Ʈ ������ ����Ͽ� RECT ���� �ؽ�Ʈ ���
// ���� �� 0, ���� �� �ؽ�Ʈ ���� ��ȯ
// ���� ����: 
//          spriteBegin()
// ���� ����: 
//          spriteEnd()
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
int TextDX::print(const std::string& str, RECT& rect, UINT format)
{
    if (dxFont == NULL)
        return 0;

    // �ؽ�Ʈ�� ȸ����Ű�� �ʵ��� ��� ����
    D3DXMatrixTransformation2D(&matrix, NULL, 0.0f, NULL, NULL, NULL, NULL);
    // ��������Ʈ���� ��Ŀ� ���� �˸� "Hello Neo"
    graphics->getSprite()->SetTransform(&matrix);
    return dxFont->DrawTextA(graphics->getSprite(), str.c_str(), -1, &rect, format, color);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �׷��� ��ġ�� �Ҿ������ �� ȣ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void TextDX::onLostDevice()
{
    if (dxFont == NULL)
        return;
    dxFont->OnLostDevice();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �׷��� ��ġ�� �缳���� �� ȣ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void TextDX::onResetDevice()
{
    if (dxFont == NULL)
        return;
    dxFont->OnResetDevice();
}