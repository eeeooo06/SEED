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

// DirectX 포인터 타입
#define LP_TEXTURE		LPDIRECT3DTEXTURE9
#define LP_SPRITE		LPD3DXSPRITE
#define LP_3DDEVICE		LPDIRECT3DDEVICE9
#define LP_3D			LPDIRECT3D9
#define VECTOR2			D3DXVECTOR2
#define LP_VERTEXBUFFER	LPDIRECT3DVERTEXBUFFER9
#define LP_DXFONT		LPD3DXFONT
#define LP_VERTEXBUFFER	LPDIRECT3DVERTEXBUFFER9

// 색상 정의
#define COLOR_ARGB DWORD
#define SETCOLOR_ARGB(a, r, g, b) ((COLOR_ARGB)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

namespace GraphicsNS
{
    // 자주 사용하는 색상들
    // ARGB 값은 0 ~ 255 범위를 가진다.
    // A = 알파 채널 (투명도, 255일 때 불투명)
    // R = 빨강, G = 초록, B = 파랑

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
    const COLOR_ARGB FILTER = D3DCOLOR_ARGB(0, 0, 0, 0);                    // 색상 필터와 함께 그릴 때 사용
    const COLOR_ARGB ALPHA25 = D3DCOLOR_ARGB(64, 255, 255, 255);            // 색상에 적용하면 25% 투명도
    const COLOR_ARGB ALPHA50 = D3DCOLOR_ARGB(128, 255, 255, 255);           // 색상에 적용하면 50% 투명도
    const COLOR_ARGB BACK_COLOR = NAVY;                                     // 게임의 배경색

    enum DISPLAY_MODE { TOGGLE, FULLSCREEN, WINDOW };
}

struct VertexC                                                              // 색상을 가진 정점
{
    float x, y, z;                                                          // 정점의 위치
    float rhw;                                                              // 동차 좌표의 W 값의 역수 (1로 설정)
    unsigned long color;                                                    // 정점의 색상
};

// 유연한 정점 형식 코드
// D3DFVF_XYZRHW = 정점들이 이미 변환된 상태이다.
// D3DFVF_DIFFUSE = 정점들이 디퓨즈(확산광) 색상 데이터를 포함한다.
#define D3DFVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

// SpriteData: 스프라이트를 그리기 위해 Graphics::drawSprite가 필요로 하는 속성들
struct SpriteData
{
    int         width;                                                      // 스프라이트의 너비 (픽셀 단위)
    int         height;                                                     // 스프라이트의 높이 (픽셀 단위)
    float       x;                                                          // 화면 위치 (스프라이트의 좌측 상단 좌표)
    float       y;
    float       scale;                                                      // 1보다 작으면 축소, 1보다 크면 확대
    float       angle;                                                      // 회전 각도 (라디안 단위)
    RECT        rect;                                                       // 더 큰 텍스처에서 이미지를 선택하는 데 사용됨
    LP_TEXTURE  texture;                                                    // 텍스처에 대한 포인터
    bool        flipHorizontal;                                             // true면 스프라이트를 수평으로 뒤집기 (좌우 반전)
    bool        flipVertical;                                               // true면 스프라이트를 수직으로 뒤집기 (상하 반전)
};

class Graphics
{
private:
    // DirectX 포인터 및 stuff
    LP_3D       direct3d;
    LP_3DDEVICE device3d;
    LP_SPRITE   sprite;
    D3DPRESENT_PARAMETERS d3dpp;
    D3DDISPLAYMODE pMode;

    // 다른 변수들
    HRESULT     result;                                                     // 표준 Windows 반환 코드들
    HWND        hwnd;
    bool        fullscreen;
    int         width;
    int         height;
    COLOR_ARGB  backColor;                                                  // 배경 색상

    // (엔진 내부 용도로만 사용. 사용자가 손댈 수 있는 부분은 없음.)
    // D3D 프레젠테이션 매개변수 초기화
    void    initD3Dpp();

public:
    // 생성자
    Graphics();

    // 소멸자
    virtual ~Graphics();

    // direct3d와 device3d를 해제한다.
    void    releaseAll();

    // DirectX 그래픽스를 초기화한다.
    // 오류 발생 시 GameError를 던진다.
    // 전제 조건: 
    //              hw = 윈도우 핸들
    //              width = 픽셀 단위의 너비
    //              height = 픽셀 단위의 높이
    //              fullscreen = 전체 화면이면 true, 창 모드면 false
    void    initialize(HWND hw, int width, int height, bool fullscreen);

    // 정점 버퍼를 생성한다.
    // 전제 조건(Pre): 
    //              verts[]에 정점 데이터가 들어 있다.
    //              size = verts[]의 크기이다.
    // 결과(Post): 
    //              성공 시 &vertexBuffer가 버퍼를 가리킨다.
    HRESULT createVertexBuffer(VertexC verts[], UINT size, LP_VERTEXBUFFER& vertexBuffer);

    // 알파 투명도를 가진 사각형(quad)을 표시한다.
    // 전제 조건(Pre): 
    //              createVertexBuffer를 사용하여 vertexBuffer를 생성했고,
    //              그 안에 시계 방향으로 정의된 4개의 정점이 들어 있어야 한다.
    //              g3ddev->BeginScene이 호출되어 있어야 한다.
    bool    drawQuad(LP_VERTEXBUFFER vertexBuffer);

    // 텍스처를 기본 D3D 메모리에 로드한다 (일반적인 텍스처 사용).
    // 엔진 내부 전용으로 사용된다. 게임 텍스처를 로드하려면 TextureManager 클래스를 사용하라.
    // 전제 조건(Pre): 
    //              filename = 텍스처 파일 이름
    //              transcolor = 투명 색상
    // 결과(Post): 
    //              width와 height = 텍스처의 크기
    //              texture가 텍스처를 가리키게 된다.
    HRESULT loadTexture(const char* filename, COLOR_ARGB transcolor, UINT& width, UINT& height, LP_TEXTURE& texture);

    // 텍스처를 시스템 메모리에 로드한다 (시스템 메모리는 잠금 가능하다).
    // 픽셀 데이터에 직접 접근할 수 있다. 표시용 텍스처를 로드하려면 TextureManager 클래스를 사용하라.
    // 전제 조건(Pre): 
    //              filename = 텍스처 파일 이름
    //              transcolor = 투명 색상
    // 결과(Post): 
    //              width와 height = 텍스처의 크기
    //              texture가 텍스처를 가리키게 된다.
    HRESULT loadTextureSystemMem(const char* filename, COLOR_ARGB transcolor, UINT& width, UINT& height, LP_TEXTURE& texture);

    // 화면에 오프스크린 백버퍼를 표시한다.
    HRESULT showBackbuffer();

    // 어댑터가 d3dpp에 지정된 백버퍼 높이, 너비, 새로 고침 빈도와 호환되는지 확인한다.
    // 호환되는 모드를 찾으면 pMode 구조체에 해당 모드의 포맷을 채운다.
    // 전제 조건(Pre): 
    //              d3dpp가 초기화되어 있어야 한다.
    // 결과(Post): 
    //              호환 가능한 모드를 찾으면 true를 반환하고 pMode 구조체가 채워진다.
    //              호환 가능한 모드를 찾지 못하면 false를 반환한다.
    bool    isAdapterCompatible();

    // SpriteData 구조체에 정의된 스프라이트를 그린다.
    // color는 선택 사항이며, 필터로 적용된다. WHITE가 기본값(변화 없음)이다.
    // 스프라이트 Begin/End 쌍을 생성한다.
    // 전제 조건(Pre): 
    //              spriteData.rect는 spriteData.texture에서 그릴 부분을 정의해야 한다.
//                  spriteData.rect.right는 오른쪽 경계 + 1 이어야 한다.
//                  spriteData.rect.bottom은 아래쪽 경계 + 1 이어야 한다.
    void    drawSprite(const SpriteData& spriteData,                        // 그릴 스프라이트
        COLOR_ARGB color = GraphicsNS::WHITE);                              // 기본값은 흰색 색상 필터 (변화 없음)

    // 그래픽스 디바이스를 리셋한다.
    HRESULT reset();

    // 전체 화면 모드와 창 모드를 전환한다.
    // 전제 조건(Pre): 
    //              사용자가 생성한 모든 D3DPOOL_DEFAULT 서피스가 해제되어야 한다.
    // 결과(Post): 
    //              모든 사용자 서피스가 다시 생성된다.
    void    changeDisplayMode(GraphicsNS::DISPLAY_MODE mode = GraphicsNS::TOGGLE);

    // 벡터 v의 길이를 반환한다.
    static float    Vector2Length(const VECTOR2* v) { return D3DXVec2Length(v); }

    // 벡터 v1과 v2의 내적(Dot product)을 반환한다.
    static float    Vector2Dot(const VECTOR2* v1, const VECTOR2* v2) { return D3DXVec2Dot(v1, v2); }

    // 벡터 v를 정규화한다.
    static void     Vector2Normalize(VECTOR2* v) { D3DXVec2Normalize(v, v); }

    // 벡터 v를 행렬 m으로 변환한다.
    static VECTOR2* Vector2Transform(VECTOR2* v, D3DXMATRIX* m) { return D3DXVec2TransformCoord(v, v, m); }

    // get 함수들
    // direct3d를 반환한다.
    LP_3D   get3D() { return direct3d; }

    // device3d를 반환한다.
    LP_3DDEVICE get3Ddevice() { return device3d; }

    // 스프라이트를 반환한다.
    LP_SPRITE   getSprite() { return sprite; }

    // 디바이스 컨텍스트(윈도우)에 대한 핸들을 반환한다.
    HDC     getDC() { return GetDC(hwnd); }

    // 디바이스 손실 여부를 검사한다.
    HRESULT getDeviceState();

    // 전체 화면 모드 여부를 반환한다.
    bool    getFullscreen() { return fullscreen; }

    // 화면을 지우는 데 사용할 색상을 설정한다.
    void setBackColor(COLOR_ARGB c) { backColor = c; }

    // ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
    // 백버퍼를 지우고 BeginScene()을 호출한다.
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
    // EndScene()을 호출한다.
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