#ifndef _TEXTUREMANAGER_H                                                    // �� ������ ���� ��ġ�� ���Ե� ���
#define _TEXTUREMANAGER_H                                                    // �ߺ� ���� ����
#define WIN32_LEAN_AND_MEAN                                                  

#include "Graphics.h"                                                        
#include "Constants.h"                                                       

class TextureManager
{
    // TextureManager �Ӽ�                                                    
private:
    UINT       width;                                                        // �ؽ�ó�� �ʺ� (�ȼ� ����)
    UINT       height;                                                       // �ؽ�ó�� ���� (�ȼ� ����)
    LP_TEXTURE texture;                                                      // �ؽ�ó ������
    const char* file;                                                        // ���� �̸�
    Graphics* graphics;                                                      // �׷��� ��ü ������ ����
    bool    initialized;                                                     // ���������� �ʱ�ȭ�Ǹ� true
    HRESULT hr;                                                              // ǥ�� ��ȯ Ÿ��

public:
    // ������
    TextureManager();

    // �Ҹ���
    virtual ~TextureManager();

    // �ؽ�ó�� ���� �����͸� ��ȯ�մϴ�
    LP_TEXTURE getTexture() const { return texture; }

    // �ؽ�ó �ʺ� ��ȯ�մϴ�
    UINT getWidth() const { return width; }

    // �ؽ�ó ���̸� ��ȯ�մϴ�
    UINT getHeight() const { return height; }

    // textureManager�� �ʱ�ȭ�մϴ�
    // ��������: 
    //          *g�� Graphics ��ü�� ����ŵ�ϴ�
    //          *file�� �ε��� �ؽ�ó ������ �̸��� ����ŵ�ϴ�
    // ��������: 
    //          �ؽ�ó ������ �ε�˴ϴ�
    virtual bool initialize(Graphics* g, const char* file);

    // ���ҽ� ����
    virtual void onLostDevice();

    // ���ҽ� ����
    virtual void onResetDevice();
};

#endif