#ifndef _DASHBOARD_H                                                         // �� ������ �� �� �̻� ���Ե� ��� ���� ���Ǹ� �����մϴ�.
#define _DASHBOARD_H            
#define WIN32_LEAN_AND_MEAN

#include "Image.h"
#include "Constants.h"
#include "TextureManager.h"

namespace dashboardNS
{
    const int   IMAGE_SIZE = 32;                                             // �� �ؽ�ó ũ��
    const int   TEXTURE_COLS = 8;                                            // �ؽ�ó ��
    const int   BAR_FRAME = 44;                                              // ������ ������ ��ȣ
}

class Bar : public Image
{
public:
    // ���� �ʱ�ȭ
    // ���� ����: 
    //          *graphics = Graphics ��ü�� ���� ������
    //          *textureM = TextureManager ��ü�� ���� ������
    //          left, top = ȭ�� ��ġ
    //          scale = �����ϸ� (Ȯ��/���) ��
    //          color = ���� ����
    bool initialize(Graphics* graphics, TextureManager* textureM, int left, int top,
        float scale, COLOR_ARGB color);
    // ���� ũ�� ����
    void set(float percentOn);
    // setRect()�� ȣ����� �ʵ��� update�� �������մϴ�.
    virtual void update(float frameTime) {}
};

#endif