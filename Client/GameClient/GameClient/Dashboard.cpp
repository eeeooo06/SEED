#include "Dashboard.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ���� �ʱ�ȭ
// ���� ����: 
//          *graphics = Graphics ��ü�� ���� ������
//          *textureM = TextureManager ��ü�� ���� ������
//          left, top = ȭ�� ��ġ
//          scale = �����ϸ� (Ȯ��/���) ��
//          color = ���� ����
// ���� ����: 
//          ���� �� true, ���� �� false ��ȯ
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool Bar::initialize(Graphics* graphics, TextureManager* textureM, int left,
    int top, float scale, COLOR_ARGB color)
{
    try {
        Image::initialize(graphics, dashboardNS::IMAGE_SIZE, dashboardNS::IMAGE_SIZE,
            dashboardNS::TEXTURE_COLS, textureM);
        setCurrentFrame(dashboardNS::BAR_FRAME);
        spriteData.x = (float)left;
        spriteData.y = (float)top;
        spriteData.scale = scale;
        colorFilter = color;
    }
    catch (...)
    {
        return false;
    }
    // ���� ��ȯ
    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ���� ũ�� ����
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Bar::set(float p)
{
    if (p < 0)
        p = 0;
    else if (p > 100)
        p = 100;
    spriteData.rect.right = spriteData.rect.left + (LONG)(spriteData.width * p / 100);
}