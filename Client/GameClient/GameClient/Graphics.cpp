#include "Graphics.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 생성자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Graphics::Graphics()
{
    direct3d = NULL;
    device3d = NULL;
    sprite = NULL;
    fullscreen = false;
    width = GAME_WIDTH;                                                      // 너비와 높이는 initialize()에서 대체됩니다.
    height = GAME_HEIGHT;
    backColor = GraphicsNS::BACK_COLOR;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 모든 surfaces를 해제하고 모든 포인터를 정리한다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Graphics::~Graphics()
{
    releaseAll();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 해제
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Graphics::releaseAll()
{
    SAFE_RELEASE(sprite);
    SAFE_RELEASE(device3d);
    SAFE_RELEASE(direct3d);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// DirectX 그래픽스를 초기화한다
// 예외가 발생하면 GameError를 던진다
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Graphics::initialize(HWND hw, int w, int h, bool full)
{
    hwnd = hw;
    width = w;
    height = h;
    fullscreen = full;

    // Direct3D 초기화
    direct3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (direct3d == NULL)
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing Direct3D"));

    initD3Dpp();                                                             // D3D 프리젠테이션 매개변수 초기화
    if (fullscreen)                                                          // 전체 화면 모드인 경우
    {
        if (isAdapterCompatible())                                           // 어댑터가 호환되는지 확인
            // 호환되는 새로 고침 빈도로 설정
            d3dpp.FullScreen_RefreshRateInHz = pMode.RefreshRate;
        else
            throw(GameError(gameErrorNS::FATAL_ERROR,
                "The graphics device does not support the specified resolution and/or format."));
    }

    // 그래픽 카드가 하드웨어 텍스처링, 조명 및 정점 셰이더를 지원하는지 확인
    D3DCAPS9 caps;
    DWORD behavior;
    result = direct3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    // 장치가 HW T&L을 지원하지 않거나 1.1 정점 셰이더를 지원하지 않는 경우
    // 하드웨어에서 셰이더를 사용하는 경우 소프트웨어 정점 처리로 전환합니다.
    if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
        caps.VertexShaderVersion < D3DVS_VERSION(1, 1))
        behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;                      // 소프트웨어 전용 처리 사용
    else                                                                     
        behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;                      // 하드웨어 전용 처리 사용

    // Direct3D 장치 생성
    result = direct3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hwnd,
        behavior,
        &d3dpp,
        &device3d);

    if (FAILED(result))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error creating Direct3D device"));

    result = D3DXCreateSprite(device3d, &sprite);
    if (FAILED(result))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error creating Direct3D sprite"));

    // 기본 형식의 알파 혼합 구성
    device3d->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    device3d->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// D3D 표현(프레젠테이션) 파라미터를 초기화한다
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Graphics::initD3Dpp()
{
    try {
        ZeroMemory(&d3dpp, sizeof(d3dpp));                                   // 구조체를 0으로 채웁니다.
        // 필요한 매개변수 채우기                                               
        d3dpp.BackBufferWidth = width;                                       
        d3dpp.BackBufferHeight = height;                                     
        if (fullscreen)                                                      // 전체 화면인 경우
            d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;                        // 24비트 색상
        else                                                                 
            d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;                         // 바탕화면 설정 사용
        d3dpp.BackBufferCount = 1;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = hwnd;
        d3dpp.Windowed = (!fullscreen);
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    catch (...)
    {
        throw(GameError(gameErrorNS::FATAL_ERROR,
            "Error initializing D3D presentation parameters"));
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 텍스처를 기본 D3D 메모리에 로드한다 (일반적인 텍스처 사용 경우)
// 파일 정보를 반환한다. 모든 텍스처를 로드할 때는 TextureManager 클래스를 사용한다.
// 사전 조건(Pre):
//              filename은 텍스처 파일의 이름이다.
//              transcolor는 투명 색상이다.
// 사후 조건(Post):
//              width와 height는 텍스처의 크기가 된다.
//              texture는 텍스처를 가리킨다.
// 반환값: 
//              HRESULT
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Graphics::loadTexture(const char* filename, COLOR_ARGB transcolor,
    UINT& width, UINT& height, LP_TEXTURE& texture)
{
    // 파일 정보 읽기용 구조체
    D3DXIMAGE_INFO info;
    result = E_FAIL;

    try {
        if (filename == NULL)
        {
            texture = NULL;
            return D3DERR_INVALIDCALL;
        }

        // 파일에서 너비와 높이 가져오기
        result = D3DXGetImageInfoFromFile(filename, &info);
        if (result != D3D_OK)
            return result;
        width = info.Width;
        height = info.Height;

        // 파일에서 로드하여 새 텍스처 만들기
        result = D3DXCreateTextureFromFileEx(
            device3d,                                                        // 3D 장치
            filename,                                                        // 이미지 파일 이름
            info.Width,                                                      // 텍스처 너비
            info.Height,                                                     // 텍스처 높이
            1,                                                               // 밉맵 수준 (1은 체인 없음)
            0,                                                               // 사용법
            D3DFMT_UNKNOWN,                                                  // 표면 형식 (기본값)
            D3DPOOL_DEFAULT,                                                 // 텍스처용 메모리 클래스
            D3DX_DEFAULT,                                                    // 이미지 필터
            D3DX_DEFAULT,                                                    // 밉 필터
            transcolor,                                                      // 투명도를 위한 색상 키
            &info,                                                           // 비트맵 파일 정보 (로드된 파일에서)
            NULL,                                                            // 색상 팔레트
            &texture);                                                       // 대상 텍스처

    }
    catch (...)
    {
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error in Graphics::loadTexture"));
    }
    return result;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 텍스처를 시스템 메모리에 로드한다 (시스템 메모리는 락 걸 수 있다)
// 빠른 접근을 위해 사용될 수 있다. 화면 표시에 텍스처를 TextureManager 클래스를 통해 로드한다.
// 사전 조건(Pre):
//              filename은 텍스처 파일의 이름이다.
//              transcolor는 투명 색상이다.
// 사후 조건(Post):
//              width와 height는 텍스처의 크기가 된다.
//              texture는 텍스처를 가리킨다.
// 반환값: 
//              HRESULT를 반환하며 TextureData 구조체를 채운다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Graphics::loadTextureSystemMem(const char* filename, COLOR_ARGB transcolor,
    UINT& width, UINT& height, LP_TEXTURE& texture)
{
    // 비트맵 파일 정보를 읽기 위한 구조체
    D3DXIMAGE_INFO info;
    result = E_FAIL;        // 표준 Windows 반환 코드

    try {
        if (filename == NULL)
        {
            texture = NULL;
            return D3DERR_INVALIDCALL;
        }

        // 비트맵 파일에서 너비와 높이를 얻는다.
        result = D3DXGetImageInfoFromFile(filename, &info);
        if (result != D3D_OK)
            return result;
        width = info.Width;
        height = info.Height;

        // 비트맵 이미지 파일를 로드하여 새로운 텍스처를 생성한다.
        result = D3DXCreateTextureFromFileEx(
            device3d,                                                        // 3D device
            filename,                                                        // 비트맵 파일 이름
            info.Width,                                                      // 비트맵 이미지 너비
            info.Height,                                                     // 비트맵 이미지 높이
            1,                                                               // 밉맵 레벨 (1이면 체인 없음)
            0,                                                               // 사용 용도
            D3DFMT_UNKNOWN,                                                  // surface 형식 (기본값)
            D3DPOOL_SYSTEMMEM,                                               // 시스템 메모리는 락 걸 수 있다.
            D3DX_DEFAULT,                                                    // 이미지 필터
            D3DX_DEFAULT,                                                    // 밉맵 필터
            transcolor,                                                      // 투명 처리를 위한 컬러 키
            &info,                                                           // (로드한 파일에서 얻은) 비트맵 파일 정보
            NULL,                                                            // 컬러 팔레트
            &texture);                                                       // 목적 텍스처

    }
    catch (...)
    {
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error in Graphics::loadTexture"));
    }
    return result;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 버텍스 버퍼를 생성한다.
// 사전 조건(Pre):
//              verts[]에 버텍스 데이터가 들어 있다.
//              size = verts[]의 크기이다.
// 사후 조건(Post):
//              생성 후 &vertexBuffer는 버퍼를 가리킨다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Graphics::createVertexBuffer(VertexC verts[], UINT size, LP_VERTEXBUFFER& vertexBuffer)
{
    // 표준 Windows 반환 코드
    HRESULT result = E_FAIL;

    // 버텍스 버퍼를 생성한다.
    result = device3d->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX,
        D3DPOOL_DEFAULT, &vertexBuffer, NULL);
    if (FAILED(result))
        return result;

    void* ptr;
    // 데이터를 넣기 위해 버퍼를 반드시 잠가야 한다.
    result = vertexBuffer->Lock(0, size, (void**)&ptr, 0);
    if (FAILED(result))
        return result;
    memcpy(ptr, verts, size);                                                // 버텍스 데이터를 버퍼에 복사한다.
    vertexBuffer->Unlock();                                                  // 잠금 해제를 잊지마라.

    return result;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// Triangle Fan을 이용하여 색이 채워진 사각형(Quad)을 표시한다.
// 사전 조건(Pre):
//              createVertexBuffer를 호출할 때와 동일하게 준비된
//              4개의 정점을 가진 vertexBuffer를 전달해야 한다.
//              g3ddev->BeginScene()이 호출되었어야 한다.
// 사후 조건(Post):
//              사각형(Quad)이 그려진다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool Graphics::drawQuad(LP_VERTEXBUFFER vertexBuffer)
{
    HRESULT result = E_FAIL;                                                 // 표준 Windows 반환 코드

    if (vertexBuffer == NULL)
        return false;

    device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, true);                  // 알파 블렌딩을 활성화한다.

    device3d->SetStreamSource(0, vertexBuffer, 0, sizeof(VertexC));
    device3d->SetFVF(D3DFVF_VERTEX);
    result = device3d->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, false);                 // 알파 블렌딩을 비활성화한다.

    if (FAILED(result))
        return false;

    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 백버퍼를 화면에 표시한다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Graphics::showBackbuffer()
{
    result = E_FAIL;                                                         // 기본으로 실패이면, 성공 시 대체된다.
    // 백버퍼를 화면에 표시한다.
    result = device3d->Present(NULL, NULL, NULL, NULL);
    return result;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 어댑터가 d3dpp에 지정된 형식, 너비, 높이 및 재생 빈도 모드와 호환되는지 확인한다.
// 호환되는 모드를 찾으면 pMode 구조체에 해당 설정을 채운다.
// 사전 조건(Pre):
//              d3dpp가 초기화되어 있어야 한다.
// 사후 조건(Post):
//              호환되는 모드를 찾으면 true를 반환하고 pMode 구조체를 채워넣는다.
//              호환되는 모드를 찾지 못하면 false를 반환한다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool Graphics::isAdapterCompatible()
{
    UINT modes = direct3d->GetAdapterModeCount(D3DADAPTER_DEFAULT,
        d3dpp.BackBufferFormat);
    for (UINT i = 0; i < modes; i++)
    {
        result = direct3d->EnumAdapterModes(D3DADAPTER_DEFAULT,
            d3dpp.BackBufferFormat,
            i, &pMode);
        if (pMode.Height == d3dpp.BackBufferHeight &&
            pMode.Width == d3dpp.BackBufferWidth &&
            pMode.RefreshRate >= d3dpp.FullScreen_RefreshRateInHz)
            return true;
    }
    return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// SpriteData 구조체를 사용하여 스프라이트를 그린다.
// Color가 지정된 색상이면, 색조로 사용된다. 기본은 WHITE(색조 없음)이다.
// 사전 조건(Pre):
//              sprite->Begin()이 호출되었어야 한다.
// 사후 조건(Post):
//              sprite->End()가 호출된다.
// spriteData.rect는 spriteData.texture에서 그릴 부분을 지정한다.
// spriteData.rect.right는 오른쪽 면 + 1이어야 한다.
// spriteData.rect.bottom은 아래쪽 면 + 1이어야 한다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Graphics::drawSprite(const SpriteData& spriteData, COLOR_ARGB color)
{
    if (spriteData.texture == NULL)                                          // 텍스처가 없으면  
        return;

    // 스프라이트의 중심을 찾는다.
    D3DXVECTOR2 spriteCenter = D3DXVECTOR2((float)(spriteData.width / 2 * spriteData.scale),
        (float)(spriteData.height / 2 * spriteData.scale));
    // 스프라이트의 화면 위치
    D3DXVECTOR2 translate = D3DXVECTOR2((float)spriteData.x, (float)spriteData.y);
    // X, Y 스케일링 행렬
    D3DXVECTOR2 scaling(spriteData.scale, spriteData.scale);
    if (spriteData.flipHorizontal)                                           // 수평으로 뒤집힌 경우
    {                                                                        
        scaling.x *= -1;                                                     // 음수인 스케일 X 계수로 뒤집는다.
        // 뒤집힌 이미지의 중심을 구한다.
        spriteCenter.x -= (float)(spriteData.width * spriteData.scale);
        // 왼쪽으로 스케일링 행렬이 이동하면,
        // 오른쪽으로 이동시켜서 원래 위치를 찾는다.
        translate.x += (float)(spriteData.width * spriteData.scale);
    }
    if (spriteData.flipVertical)                                             // 수직으로 뒤집힌 경우
    {                                                                        
        scaling.y *= -1;                                                     // 음수인 스케일 Y 계수로 뒤집는다.
        // 뒤집힌 이미지의 중심을 구한다.
        spriteCenter.y -= (float)(spriteData.height * spriteData.scale);
        // 위쪽으로 스케일링 행렬이 이동하면,
        // 아래쪽으로 이동시켜서 원래 위치를 찾는다.
        translate.y += (float)(spriteData.height * spriteData.scale);
    }
    // 스프라이트를 회전, 스케일, 위치시키는 변환 행렬을 생성한다.
    D3DXMATRIX matrix;
    D3DXMatrixTransformation2D(
        &matrix,                                                             // 결과
        NULL,                                                                // 스케일링 전 중심에 대한 변환에 사용한다.
        0.0f,                                                                // 스케일링 회전 계수
        &scaling,                                                            // 스케일링 크기
        &spriteCenter,                                                       // 회전 중심
        (float)(spriteData.angle),                                           // 회전 각도
        &translate);                                                         // X, Y 좌표 위치

    // 스프라이트의 변환을 설정한다. ("Hello Neo")
    sprite->SetTransform(&matrix);

    // 스프라이트를 그린다.
    sprite->Draw(spriteData.texture, &spriteData.rect, NULL, NULL, color);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 손실된 장치 테스트
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Graphics::getDeviceState()
{
    result = E_FAIL;                                                         // 기본적으로 실패, 성공 시 교체
    if (device3d == NULL)
        return  result;
    result = device3d->TestCooperativeLevel();
    return result;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 그래픽 장치 재설정
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Graphics::reset()
{
    result = E_FAIL;                                                         // 기본적으로 실패, 성공 시 교체
    initD3Dpp();                                                             // D3D 프리젠테이션 매개변수 초기화
    sprite->OnLostDevice();
    result = device3d->Reset(&d3dpp);                                        // 그래픽 장치 재설정 시도

    // 기본 형식의 알파 혼합 구성
    device3d->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    device3d->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    sprite->OnResetDevice();
    return result;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 창 모드 또는 전체 화면 모드 전환
// 사전 조건: 
//          모든 사용자가 생성한 D3DPOOL_DEFAULT 표면이 해제됩니다.
// 사후 조건: 
//          모든 사용자 표면이 다시 생성됩니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Graphics::changeDisplayMode(GraphicsNS::DISPLAY_MODE mode)
{
    try {
        switch (mode)
        {
        case GraphicsNS::FULLSCREEN:
            if (fullscreen)                                                  // 이미 전체 화면 모드인 경우
                return;
            fullscreen = true; break;
        case GraphicsNS::WINDOW:
            if (fullscreen == false)                                         // 이미 창 모드인 경우
                return;
            fullscreen = false; break;
        default:                                                             // 기본적으로 창/전체 화면 전환
            fullscreen = !fullscreen;
        }
        reset();
        if (fullscreen)                                                      // 전체 화면
        {
            SetWindowLong(hwnd, GWL_STYLE, WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP);
        }
        else                                                                 // 창 모드
        {
            SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
            SetWindowPos(hwnd, HWND_TOP, 0, 0, GAME_WIDTH, GAME_HEIGHT,
                SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

            // 클라이언트 영역이 GAME_WIDTH x GAME_HEIGHT가 되도록 창 크기 조정
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);                                // 창의 클라이언트 영역 크기 가져오기
            MoveWindow(hwnd,
                0,                                                           // 왼쪽
                0,                                                           // 위쪽
                GAME_WIDTH + (GAME_WIDTH - clientRect.right),                // 오른쪽
                GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom),             // 아래쪽
                TRUE);                                                       // 창 다시 그리기
        }

    }
    catch (...)
    {
        // 오류가 발생했습니다. 창 모드를 시도하세요. 
        SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowPos(hwnd, HWND_TOP, 0, 0, GAME_WIDTH, GAME_HEIGHT,
            SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

        // 클라이언트 영역이 GAME_WIDTH x GAME_HEIGHT가 되도록 창 크기 조정
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);                                    // 창의 클라이언트 영역 크기 가져오기
        MoveWindow(hwnd,
            0,                                                               // 왼쪽
            0,                                                               // 위쪽
            GAME_WIDTH + (GAME_WIDTH - clientRect.right),                    // 오른쪽
            GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom),                 // 아래쪽
            TRUE);                                                           // 창 다시 그리기
    }
}