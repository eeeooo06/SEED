#ifndef _TEXTDX_H                                                            // �� ������ �� �� �̻� ���Ե� ���
#define _TEXTDX_H                                                            // ���� ���Ǹ� �����մϴ�.
#define WIN32_LEAN_AND_MEAN

#include <string>
#include "Constants.h"
#include "Graphics.h"

class TextDX
{
private:
    Graphics* graphics;
    COLOR_ARGB  color;                                                       // �۲� ���� (a,r,g,b)
    LP_DXFONT   dxFont;
    RECT        fontRect;                                                    // �ؽ�Ʈ �簢��
    // �ؽ�Ʈ�� ȸ����Ű�� ���� ���                                             
    D3DXMATRIX  matrix;
    float       angle;                                                       // �ؽ�Ʈ�� ȸ�� ���� (����)

public:
    // ������ (��������Ʈ �ؽ�Ʈ)
    TextDX();

    // �Ҹ���
    virtual ~TextDX();

    // �۲� �ʱ�ȭ
    // ���� ����: 
    //          *g�� Graphics ��ü�� ����ŵ�ϴ�.
    //          height = ���� (�ȼ� ����)
    //          bold = ���� (true/false)
    //          italic = ����Ӳ� (true/false)
    //          &fontName = ����� �۲� �̸�
    virtual bool initialize(Graphics* g, int height, bool bold, bool italic, const std::string& fontName);

    // x,y ��ġ�� ����մϴ�. spriteBegin()/spriteEnd() ���̿��� ȣ���ϼ���.
    // ���� �� 0, ���� �� �ؽ�Ʈ ���̸� ��ȯ�մϴ�.
    // ���� ����: 
    //          &str�� ǥ���� �ؽ�Ʈ�� �����մϴ�.
    //          x, y = ȭ�� ��ġ
    virtual int print(const std::string& str, int x, int y);

    // ������ ����Ͽ� �簢�� ���ο� ����մϴ�. spriteBegin()/spriteEnd() ���̿��� ȣ���ϼ���.
    // ���� �� 0, ���� �� �ؽ�Ʈ ���̸� ��ȯ�մϴ�.
    // ���� ����:   
    //          &str = ǥ���� �ؽ�Ʈ
    //          &rect = �簢�� ����
    //          format = ���� ������
    virtual int print(const std::string& str, RECT& rect, UINT format);

    // ȸ�� ������ �� ������ ��ȯ�մϴ�.
    virtual float getDegrees() { return angle * (180.0f / (float)PI); }

    // ȸ�� ������ ���� ������ ��ȯ�մϴ�.
    virtual float getRadians() { return angle; }

    // �۲� ������ ��ȯ�մϴ�.
    virtual COLOR_ARGB getFontColor() { return color; }

    // ȸ�� ������ �� ������ �����մϴ�.
    // 0���� �����Դϴ�. ������ �ð� �������� ����˴ϴ�.
    virtual void setDegrees(float deg) { angle = deg * ((float)PI / 180.0f); }

    // ȸ�� ������ ���� ������ �����մϴ�.
    // 0 ������ �����Դϴ�. ������ �ð� �������� ����˴ϴ�.
    virtual void setRadians(float rad) { angle = rad; }

    // �۲� ������ �����մϴ�. SETCOLOR_ARGB ��ũ�� �Ǵ� graphicsNS::�� ������ ����ϼ���.
    virtual void setFontColor(COLOR_ARGB c) { color = c; }

    // ���ҽ� ����
    virtual void onLostDevice();

    // ���ҽ� ����
    virtual void onResetDevice();
};

#endif