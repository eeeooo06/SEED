#pragma once

#ifndef _GRAPHICS_H
#define _GRAPHICS_H
#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include "Constants.h"
#include "GameError.h"

// DirectX ������ Ÿ��
#define LP_TEXTURE		LPDIRECT3DTEXTURE9
#define LP_SPRITE		LPD3DXSPRITE
#define LP_3DDEVICE		LPDIRECT3DDEVICE9
#define LP_3D			LPDIRECT3D9
#define VECTOR2			D3DXVECTOR2
#define LP_VERTEXBUFFER	LPDIRECT3DVERTEXBUFFER9
#define LP_DXFONT		LPD3DXFONT
#define LP_VERTEXBUFFER	LPDIRECT3DVERTEXBUFFER9

// ���� ����
#define COLOR_ARGB DWORD
#define SETCOLOR_ARGB(a, r, g, b) ((COLOR_ARGB)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

namespace GraphicsNS
{
    // ���� ����ϴ� �����
    // ARGB ���� 0 ~ 255 ������ ������.
    // A = ���� ä�� (����, 255�� �� ������)
    // R = ����, G = �ʷ�, B = �Ķ�

    const COLOR_ARGB ORANGE = D3DCOLOR_ARGB(255, 255, 165, 0);
    const COLOR_ARGB BROWN = D3DCOLOR_ARGB(255, 139, 69, 19);
    const COLOR_ARGB LTGRAY = D3DCOLOR_ARGB(255, 192, 192, 192);
    const COLOR_ARGB GRAY = D3DCOLOR_ARGB(255, 128, 128, 128);
    const COLOR_ARGB OLIVE = D3DCOLOR_ARGB(255, 128, 128, 0);
    const COLOR_ARGB PURPLE = D3DCOLOR_ARGB(255, 128, 0, 128);
    const COLOR_ARGB MAROON = D3DCOLOR_ARGB(255, 128, 0, 0);
    const COLOR_ARGB TEAL = D3DCOLOR_ARGB(255, 0, 128, 128);
    const COLOR_ARGB GREEN = D3DCOLOR_ARGB(255, 0, 128, 0);
    const COLOR_ARGB NAVY = D3DCOLOR_ARGB(255, 0, 0, 128);
    const COLOR_ARGB WHITE = D3DCOLOR_ARGB(255, 255, 255, 255);
    const COLOR_ARGB YELLOW = D3DCOLOR_ARGB(255, 255, 255, 0);
    const COLOR_ARGB MAGENTA = D3DCOLOR_ARGB(255, 255, 0, 255);
    const COLOR_ARGB RED = D3DCOLOR_ARGB(255, 255, 0, 0);
    const COLOR_ARGB CYAN = D3DCOLOR_ARGB(255, 0, 255, 255);
    const COLOR_ARGB LIME = D3DCOLOR_ARGB(255, 0, 255, 0);
    const COLOR_ARGB BLUE = D3DCOLOR_ARGB(255, 0, 0, 255);
    const COLOR_ARGB BLACK = D3DCOLOR_ARGB(255, 0, 0, 0);
    const COLOR_ARGB FILTER = D3DCOLOR_ARGB(0, 0, 0, 0);                    // ���� ���Ϳ� �Բ� �׸� �� ���
    const COLOR_ARGB ALPHA25 = D3DCOLOR_ARGB(64, 255, 255, 255);            // ���� �����ϸ� 25% ����
    const COLOR_ARGB ALPHA50 = D3DCOLOR_ARGB(128, 255, 255, 255);           // ���� �����ϸ� 50% ����
    const COLOR_ARGB BACK_COLOR = NAVY;                                     // ������ ����

    enum DISPLAY_MODE { TOGGLE, FULLSCREEN, WINDOW };
}

struct VertexC                                                              // ������ ���� ����
{
    float x, y, z;                                                          // ������ ��ġ
    float rhw;                                                              // ���� ��ǥ�� W ���� ���� (1�� ����)
    unsigned long color;                                                    // ������ ����
};

// ������ ���� ���� �ڵ�
// D3DFVF_XYZRHW = �������� �̹� ��ȯ�� �����̴�.
// D3DFVF_DIFFUSE = �������� ��ǻ��(Ȯ�걤) ���� �����͸� �����Ѵ�.
#define D3DFVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

// SpriteData: ��������Ʈ�� �׸��� ���� Graphics::drawSprite�� �ʿ�� �ϴ� �Ӽ���
struct SpriteData
{
    int         width;                                                      // ��������Ʈ�� �ʺ� (�ȼ� ����)
    int         height;                                                     // ��������Ʈ�� ���� (�ȼ� ����)
    float       x;                                                          // ȭ�� ��ġ (��������Ʈ�� ���� ��� ��ǥ)
    float       y;
    float       scale;                                                      // 1���� ������ ���, 1���� ũ�� Ȯ��
    float       angle;                                                      // ȸ�� ���� (���� ����)
    RECT        rect;                                                       // �� ū �ؽ�ó���� �̹����� �����ϴ� �� ����
    LP_TEXTURE  texture;                                                    // �ؽ�ó�� ���� ������
    bool        flipHorizontal;                                             // true�� ��������Ʈ�� �������� ������ (�¿� ����)
    bool        flipVertical;                                               // true�� ��������Ʈ�� �������� ������ (���� ����)
};

class Graphics
{
private:
    // DirectX ������ �� stuff
    LP_3D       direct3d;
    LP_3DDEVICE device3d;
    LP_SPRITE   sprite;
    D3DPRESENT_PARAMETERS d3dpp;
    D3DDISPLAYMODE pMode;

    // �ٸ� ������
    HRESULT     result;                                                     // ǥ�� Windows ��ȯ �ڵ��
    HWND        hwnd;
    bool        fullscreen;
    int         width;
    int         height;
    COLOR_ARGB  backColor;                                                  // ��� ����

    // (���� ���� �뵵�θ� ���. ����ڰ� �մ� �� �ִ� �κ��� ����.)
    // D3D ���������̼� �Ű����� �ʱ�ȭ
    void    initD3Dpp();

public:
    // ������
    Graphics();

    // �Ҹ���
    virtual ~Graphics();

    // direct3d�� device3d�� �����Ѵ�.
    void    releaseAll();

    // DirectX �׷��Ƚ��� �ʱ�ȭ�Ѵ�.
    // ���� �߻� �� GameError�� ������.
    // ���� ����: 
    //              hw = ������ �ڵ�
    //              width = �ȼ� ������ �ʺ�
    //              height = �ȼ� ������ ����
    //              fullscreen = ��ü ȭ���̸� true, â ���� false
    void    initialize(HWND hw, int width, int height, bool fullscreen);

    // ���� ���۸� �����Ѵ�.
    // ���� ����(Pre): 
    //              verts[]�� ���� �����Ͱ� ��� �ִ�.
    //              size = verts[]�� ũ���̴�.
    // ���(Post): 
    //              ���� �� &vertexBuffer�� ���۸� ����Ų��.
    HRESULT createVertexBuffer(VertexC verts[], UINT size, LP_VERTEXBUFFER& vertexBuffer);

    // ���� ������ ���� �簢��(quad)�� ǥ���Ѵ�.
    // ���� ����(Pre): 
    //              createVertexBuffer�� ����Ͽ� vertexBuffer�� �����߰�,
    //              �� �ȿ� �ð� �������� ���ǵ� 4���� ������ ��� �־�� �Ѵ�.
    //              g3ddev->BeginScene�� ȣ��Ǿ� �־�� �Ѵ�.
    bool    drawQuad(LP_VERTEXBUFFER vertexBuffer);

    // �ؽ�ó�� �⺻ D3D �޸𸮿� �ε��Ѵ� (�Ϲ����� �ؽ�ó ���).
    // ���� ���� �������� ���ȴ�. ���� �ؽ�ó�� �ε��Ϸ��� TextureManager Ŭ������ ����϶�.
    // ���� ����(Pre): 
    //              filename = �ؽ�ó ���� �̸�
    //              transcolor = ���� ����
    // ���(Post): 
    //              width�� height = �ؽ�ó�� ũ��
    //              texture�� �ؽ�ó�� ����Ű�� �ȴ�.
    HRESULT loadTexture(const char* filename, COLOR_ARGB transcolor, UINT& width, UINT& height, LP_TEXTURE& texture);

    // �ؽ�ó�� �ý��� �޸𸮿� �ε��Ѵ� (�ý��� �޸𸮴� ��� �����ϴ�).
    // �ȼ� �����Ϳ� ���� ������ �� �ִ�. ǥ�ÿ� �ؽ�ó�� �ε��Ϸ��� TextureManager Ŭ������ ����϶�.
    // ���� ����(Pre): 
    //              filename = �ؽ�ó ���� �̸�
    //              transcolor = ���� ����
    // ���(Post): 
    //              width�� height = �ؽ�ó�� ũ��
    //              texture�� �ؽ�ó�� ����Ű�� �ȴ�.
    HRESULT loadTextureSystemMem(const char* filename, COLOR_ARGB transcolor, UINT& width, UINT& height, LP_TEXTURE& texture);

    // ȭ�鿡 ������ũ�� ����۸� ǥ���Ѵ�.
    HRESULT showBackbuffer();

    // ����Ͱ� d3dpp�� ������ ����� ����, �ʺ�, ���� ��ħ �󵵿� ȣȯ�Ǵ��� Ȯ���Ѵ�.
    // ȣȯ�Ǵ� ��带 ã���� pMode ����ü�� �ش� ����� ������ ä���.
    // ���� ����(Pre): 
    //              d3dpp�� �ʱ�ȭ�Ǿ� �־�� �Ѵ�.
    // ���(Post): 
    //              ȣȯ ������ ��带 ã���� true�� ��ȯ�ϰ� pMode ����ü�� ä������.
    //              ȣȯ ������ ��带 ã�� ���ϸ� false�� ��ȯ�Ѵ�.
    bool    isAdapterCompatible();

    // SpriteData ����ü�� ���ǵ� ��������Ʈ�� �׸���.
    // color�� ���� �����̸�, ���ͷ� ����ȴ�. WHITE�� �⺻��(��ȭ ����)�̴�.
    // ��������Ʈ Begin/End ���� �����Ѵ�.
    // ���� ����(Pre): 
    //              spriteData.rect�� spriteData.texture���� �׸� �κ��� �����ؾ� �Ѵ�.
//                  spriteData.rect.right�� ������ ��� + 1 �̾�� �Ѵ�.
//                  spriteData.rect.bottom�� �Ʒ��� ��� + 1 �̾�� �Ѵ�.
    void    drawSprite(const SpriteData& spriteData,                        // �׸� ��������Ʈ
        COLOR_ARGB color = GraphicsNS::WHITE);                              // �⺻���� ��� ���� ���� (��ȭ ����)

    // �׷��Ƚ� ����̽��� �����Ѵ�.
    HRESULT reset();

    // ��ü ȭ�� ���� â ��带 ��ȯ�Ѵ�.
    // ���� ����(Pre): 
    //              ����ڰ� ������ ��� D3DPOOL_DEFAULT ���ǽ��� �����Ǿ�� �Ѵ�.
    // ���(Post): 
    //              ��� ����� ���ǽ��� �ٽ� �����ȴ�.
    void    changeDisplayMode(GraphicsNS::DISPLAY_MODE mode = GraphicsNS::TOGGLE);

    // ���� v�� ���̸� ��ȯ�Ѵ�.
    static float    Vector2Length(const VECTOR2* v) { return D3DXVec2Length(v); }

    // ���� v1�� v2�� ����(Dot product)�� ��ȯ�Ѵ�.
    static float    Vector2Dot(const VECTOR2* v1, const VECTOR2* v2) { return D3DXVec2Dot(v1, v2); }

    // ���� v�� ����ȭ�Ѵ�.
    static void     Vector2Normalize(VECTOR2* v) { D3DXVec2Normalize(v, v); }

    // ���� v�� ��� m���� ��ȯ�Ѵ�.
    static VECTOR2* Vector2Transform(VECTOR2* v, D3DXMATRIX* m) { return D3DXVec2TransformCoord(v, v, m); }

    // get �Լ���
    // direct3d�� ��ȯ�Ѵ�.
    LP_3D   get3D() { return direct3d; }

    // device3d�� ��ȯ�Ѵ�.
    LP_3DDEVICE get3Ddevice() { return device3d; }

    // ��������Ʈ�� ��ȯ�Ѵ�.
    LP_SPRITE   getSprite() { return sprite; }

    // ����̽� ���ؽ�Ʈ(������)�� ���� �ڵ��� ��ȯ�Ѵ�.
    HDC     getDC() { return GetDC(hwnd); }

    // ����̽� �ս� ���θ� �˻��Ѵ�.
    HRESULT getDeviceState();

    // ��ü ȭ�� ��� ���θ� ��ȯ�Ѵ�.
    bool    getFullscreen() { return fullscreen; }

    // ȭ���� ����� �� ����� ������ �����Ѵ�.
    void setBackColor(COLOR_ARGB c) { backColor = c; }

    // ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
    // ����۸� ����� BeginScene()�� ȣ���Ѵ�.
    // ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
    HRESULT beginScene()
    {
        result = E_FAIL;
        if (device3d == NULL)
            return result;
        // clear backbuffer to backColor
        device3d->Clear(0, NULL, D3DCLEAR_TARGET, backColor, 1.0F, 0);
        result = device3d->BeginScene();                                    // begin scene for drawing
        return result;
    }

    // ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
    // EndScene()�� ȣ���Ѵ�.
    // ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
    HRESULT endScene()
    {
        result = E_FAIL;
        if (device3d)
            result = device3d->EndScene();
        return result;
    }

    // ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
    // Sprite Begin
    // ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
    void spriteBegin()
    {
        sprite->Begin(D3DXSPRITE_ALPHABLEND);
    }

    // ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
    // Sprite End
    // ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
    void spriteEnd()
    {
        sprite->End();
    }
};
#endif