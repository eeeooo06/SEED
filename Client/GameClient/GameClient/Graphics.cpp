#include "Graphics.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ������
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Graphics::Graphics()
{
    direct3d = NULL;
    device3d = NULL;
    sprite = NULL;
    fullscreen = false;
    width = GAME_WIDTH;                                                      // �ʺ�� ���̴� initialize()���� ��ü�˴ϴ�.
    height = GAME_HEIGHT;
    backColor = GraphicsNS::BACK_COLOR;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ��� surfaces�� �����ϰ� ��� �����͸� �����Ѵ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Graphics::~Graphics()
{
    releaseAll();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ����
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Graphics::releaseAll()
{
    SAFE_RELEASE(sprite);
    SAFE_RELEASE(device3d);
    SAFE_RELEASE(direct3d);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// DirectX �׷��Ƚ��� �ʱ�ȭ�Ѵ�
// ���ܰ� �߻��ϸ� GameError�� ������
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Graphics::initialize(HWND hw, int w, int h, bool full)
{
    hwnd = hw;
    width = w;
    height = h;
    fullscreen = full;

    // Direct3D �ʱ�ȭ
    direct3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (direct3d == NULL)
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing Direct3D"));

    initD3Dpp();                                                             // D3D ���������̼� �Ű����� �ʱ�ȭ
    if (fullscreen)                                                          // ��ü ȭ�� ����� ���
    {
        if (isAdapterCompatible())                                           // ����Ͱ� ȣȯ�Ǵ��� Ȯ��
            // ȣȯ�Ǵ� ���� ��ħ �󵵷� ����
            d3dpp.FullScreen_RefreshRateInHz = pMode.RefreshRate;
        else
            throw(GameError(gameErrorNS::FATAL_ERROR,
                "The graphics device does not support the specified resolution and/or format."));
    }

    // �׷��� ī�尡 �ϵ���� �ؽ�ó��, ���� �� ���� ���̴��� �����ϴ��� Ȯ��
    D3DCAPS9 caps;
    DWORD behavior;
    result = direct3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    // ��ġ�� HW T&L�� �������� �ʰų� 1.1 ���� ���̴��� �������� �ʴ� ���
    // �ϵ����� ���̴��� ����ϴ� ��� ����Ʈ���� ���� ó���� ��ȯ�մϴ�.
    if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
        caps.VertexShaderVersion < D3DVS_VERSION(1, 1))
        behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;                      // ����Ʈ���� ���� ó�� ���
    else                                                                     
        behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;                      // �ϵ���� ���� ó�� ���

    // Direct3D ��ġ ����
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

    // �⺻ ������ ���� ȥ�� ����
    device3d->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    device3d->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// D3D ǥ��(���������̼�) �Ķ���͸� �ʱ�ȭ�Ѵ�
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Graphics::initD3Dpp()
{
    try {
        ZeroMemory(&d3dpp, sizeof(d3dpp));                                   // ����ü�� 0���� ä��ϴ�.
        // �ʿ��� �Ű����� ä���                                               
        d3dpp.BackBufferWidth = width;                                       
        d3dpp.BackBufferHeight = height;                                     
        if (fullscreen)                                                      // ��ü ȭ���� ���
            d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;                        // 24��Ʈ ����
        else                                                                 
            d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;                         // ����ȭ�� ���� ���
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
// �ؽ�ó�� �⺻ D3D �޸𸮿� �ε��Ѵ� (�Ϲ����� �ؽ�ó ��� ���)
// ���� ������ ��ȯ�Ѵ�. ��� �ؽ�ó�� �ε��� ���� TextureManager Ŭ������ ����Ѵ�.
// ���� ����(Pre):
//              filename�� �ؽ�ó ������ �̸��̴�.
//              transcolor�� ���� �����̴�.
// ���� ����(Post):
//              width�� height�� �ؽ�ó�� ũ�Ⱑ �ȴ�.
//              texture�� �ؽ�ó�� ����Ų��.
// ��ȯ��: 
//              HRESULT
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Graphics::loadTexture(const char* filename, COLOR_ARGB transcolor,
    UINT& width, UINT& height, LP_TEXTURE& texture)
{
    // ���� ���� �б�� ����ü
    D3DXIMAGE_INFO info;
    result = E_FAIL;

    try {
        if (filename == NULL)
        {
            texture = NULL;
            return D3DERR_INVALIDCALL;
        }

        // ���Ͽ��� �ʺ�� ���� ��������
        result = D3DXGetImageInfoFromFile(filename, &info);
        if (result != D3D_OK)
            return result;
        width = info.Width;
        height = info.Height;

        // ���Ͽ��� �ε��Ͽ� �� �ؽ�ó �����
        result = D3DXCreateTextureFromFileEx(
            device3d,                                                        // 3D ��ġ
            filename,                                                        // �̹��� ���� �̸�
            info.Width,                                                      // �ؽ�ó �ʺ�
            info.Height,                                                     // �ؽ�ó ����
            1,                                                               // �Ӹ� ���� (1�� ü�� ����)
            0,                                                               // ����
            D3DFMT_UNKNOWN,                                                  // ǥ�� ���� (�⺻��)
            D3DPOOL_DEFAULT,                                                 // �ؽ�ó�� �޸� Ŭ����
            D3DX_DEFAULT,                                                    // �̹��� ����
            D3DX_DEFAULT,                                                    // �� ����
            transcolor,                                                      // ������ ���� ���� Ű
            &info,                                                           // ��Ʈ�� ���� ���� (�ε�� ���Ͽ���)
            NULL,                                                            // ���� �ȷ�Ʈ
            &texture);                                                       // ��� �ؽ�ó

    }
    catch (...)
    {
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error in Graphics::loadTexture"));
    }
    return result;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �ؽ�ó�� �ý��� �޸𸮿� �ε��Ѵ� (�ý��� �޸𸮴� �� �� �� �ִ�)
// ���� ������ ���� ���� �� �ִ�. ȭ�� ǥ�ÿ� �ؽ�ó�� TextureManager Ŭ������ ���� �ε��Ѵ�.
// ���� ����(Pre):
//              filename�� �ؽ�ó ������ �̸��̴�.
//              transcolor�� ���� �����̴�.
// ���� ����(Post):
//              width�� height�� �ؽ�ó�� ũ�Ⱑ �ȴ�.
//              texture�� �ؽ�ó�� ����Ų��.
// ��ȯ��: 
//              HRESULT�� ��ȯ�ϸ� TextureData ����ü�� ä���.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Graphics::loadTextureSystemMem(const char* filename, COLOR_ARGB transcolor,
    UINT& width, UINT& height, LP_TEXTURE& texture)
{
    // ��Ʈ�� ���� ������ �б� ���� ����ü
    D3DXIMAGE_INFO info;
    result = E_FAIL;        // ǥ�� Windows ��ȯ �ڵ�

    try {
        if (filename == NULL)
        {
            texture = NULL;
            return D3DERR_INVALIDCALL;
        }

        // ��Ʈ�� ���Ͽ��� �ʺ�� ���̸� ��´�.
        result = D3DXGetImageInfoFromFile(filename, &info);
        if (result != D3D_OK)
            return result;
        width = info.Width;
        height = info.Height;

        // ��Ʈ�� �̹��� ���ϸ� �ε��Ͽ� ���ο� �ؽ�ó�� �����Ѵ�.
        result = D3DXCreateTextureFromFileEx(
            device3d,                                                        // 3D device
            filename,                                                        // ��Ʈ�� ���� �̸�
            info.Width,                                                      // ��Ʈ�� �̹��� �ʺ�
            info.Height,                                                     // ��Ʈ�� �̹��� ����
            1,                                                               // �Ӹ� ���� (1�̸� ü�� ����)
            0,                                                               // ��� �뵵
            D3DFMT_UNKNOWN,                                                  // surface ���� (�⺻��)
            D3DPOOL_SYSTEMMEM,                                               // �ý��� �޸𸮴� �� �� �� �ִ�.
            D3DX_DEFAULT,                                                    // �̹��� ����
            D3DX_DEFAULT,                                                    // �Ӹ� ����
            transcolor,                                                      // ���� ó���� ���� �÷� Ű
            &info,                                                           // (�ε��� ���Ͽ��� ����) ��Ʈ�� ���� ����
            NULL,                                                            // �÷� �ȷ�Ʈ
            &texture);                                                       // ���� �ؽ�ó

    }
    catch (...)
    {
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error in Graphics::loadTexture"));
    }
    return result;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ���ؽ� ���۸� �����Ѵ�.
// ���� ����(Pre):
//              verts[]�� ���ؽ� �����Ͱ� ��� �ִ�.
//              size = verts[]�� ũ���̴�.
// ���� ����(Post):
//              ���� �� &vertexBuffer�� ���۸� ����Ų��.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Graphics::createVertexBuffer(VertexC verts[], UINT size, LP_VERTEXBUFFER& vertexBuffer)
{
    // ǥ�� Windows ��ȯ �ڵ�
    HRESULT result = E_FAIL;

    // ���ؽ� ���۸� �����Ѵ�.
    result = device3d->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX,
        D3DPOOL_DEFAULT, &vertexBuffer, NULL);
    if (FAILED(result))
        return result;

    void* ptr;
    // �����͸� �ֱ� ���� ���۸� �ݵ�� �ᰡ�� �Ѵ�.
    result = vertexBuffer->Lock(0, size, (void**)&ptr, 0);
    if (FAILED(result))
        return result;
    memcpy(ptr, verts, size);                                                // ���ؽ� �����͸� ���ۿ� �����Ѵ�.
    vertexBuffer->Unlock();                                                  // ��� ������ ��������.

    return result;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// Triangle Fan�� �̿��Ͽ� ���� ä���� �簢��(Quad)�� ǥ���Ѵ�.
// ���� ����(Pre):
//              createVertexBuffer�� ȣ���� ���� �����ϰ� �غ��
//              4���� ������ ���� vertexBuffer�� �����ؾ� �Ѵ�.
//              g3ddev->BeginScene()�� ȣ��Ǿ���� �Ѵ�.
// ���� ����(Post):
//              �簢��(Quad)�� �׷�����.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool Graphics::drawQuad(LP_VERTEXBUFFER vertexBuffer)
{
    HRESULT result = E_FAIL;                                                 // ǥ�� Windows ��ȯ �ڵ�

    if (vertexBuffer == NULL)
        return false;

    device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, true);                  // ���� ������ Ȱ��ȭ�Ѵ�.

    device3d->SetStreamSource(0, vertexBuffer, 0, sizeof(VertexC));
    device3d->SetFVF(D3DFVF_VERTEX);
    result = device3d->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, false);                 // ���� ������ ��Ȱ��ȭ�Ѵ�.

    if (FAILED(result))
        return false;

    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ����۸� ȭ�鿡 ǥ���Ѵ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Graphics::showBackbuffer()
{
    result = E_FAIL;                                                         // �⺻���� �����̸�, ���� �� ��ü�ȴ�.
    // ����۸� ȭ�鿡 ǥ���Ѵ�.
    result = device3d->Present(NULL, NULL, NULL, NULL);
    return result;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ����Ͱ� d3dpp�� ������ ����, �ʺ�, ���� �� ��� �� ���� ȣȯ�Ǵ��� Ȯ���Ѵ�.
// ȣȯ�Ǵ� ��带 ã���� pMode ����ü�� �ش� ������ ä���.
// ���� ����(Pre):
//              d3dpp�� �ʱ�ȭ�Ǿ� �־�� �Ѵ�.
// ���� ����(Post):
//              ȣȯ�Ǵ� ��带 ã���� true�� ��ȯ�ϰ� pMode ����ü�� ä���ִ´�.
//              ȣȯ�Ǵ� ��带 ã�� ���ϸ� false�� ��ȯ�Ѵ�.
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
// SpriteData ����ü�� ����Ͽ� ��������Ʈ�� �׸���.
// Color�� ������ �����̸�, ������ ���ȴ�. �⺻�� WHITE(���� ����)�̴�.
// ���� ����(Pre):
//              sprite->Begin()�� ȣ��Ǿ���� �Ѵ�.
// ���� ����(Post):
//              sprite->End()�� ȣ��ȴ�.
// spriteData.rect�� spriteData.texture���� �׸� �κ��� �����Ѵ�.
// spriteData.rect.right�� ������ �� + 1�̾�� �Ѵ�.
// spriteData.rect.bottom�� �Ʒ��� �� + 1�̾�� �Ѵ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Graphics::drawSprite(const SpriteData& spriteData, COLOR_ARGB color)
{
    if (spriteData.texture == NULL)                                          // �ؽ�ó�� ������  
        return;

    // ��������Ʈ�� �߽��� ã�´�.
    D3DXVECTOR2 spriteCenter = D3DXVECTOR2((float)(spriteData.width / 2 * spriteData.scale),
        (float)(spriteData.height / 2 * spriteData.scale));
    // ��������Ʈ�� ȭ�� ��ġ
    D3DXVECTOR2 translate = D3DXVECTOR2((float)spriteData.x, (float)spriteData.y);
    // X, Y �����ϸ� ���
    D3DXVECTOR2 scaling(spriteData.scale, spriteData.scale);
    if (spriteData.flipHorizontal)                                           // �������� ������ ���
    {                                                                        
        scaling.x *= -1;                                                     // ������ ������ X ����� �����´�.
        // ������ �̹����� �߽��� ���Ѵ�.
        spriteCenter.x -= (float)(spriteData.width * spriteData.scale);
        // �������� �����ϸ� ����� �̵��ϸ�,
        // ���������� �̵����Ѽ� ���� ��ġ�� ã�´�.
        translate.x += (float)(spriteData.width * spriteData.scale);
    }
    if (spriteData.flipVertical)                                             // �������� ������ ���
    {                                                                        
        scaling.y *= -1;                                                     // ������ ������ Y ����� �����´�.
        // ������ �̹����� �߽��� ���Ѵ�.
        spriteCenter.y -= (float)(spriteData.height * spriteData.scale);
        // �������� �����ϸ� ����� �̵��ϸ�,
        // �Ʒ������� �̵����Ѽ� ���� ��ġ�� ã�´�.
        translate.y += (float)(spriteData.height * spriteData.scale);
    }
    // ��������Ʈ�� ȸ��, ������, ��ġ��Ű�� ��ȯ ����� �����Ѵ�.
    D3DXMATRIX matrix;
    D3DXMatrixTransformation2D(
        &matrix,                                                             // ���
        NULL,                                                                // �����ϸ� �� �߽ɿ� ���� ��ȯ�� ����Ѵ�.
        0.0f,                                                                // �����ϸ� ȸ�� ���
        &scaling,                                                            // �����ϸ� ũ��
        &spriteCenter,                                                       // ȸ�� �߽�
        (float)(spriteData.angle),                                           // ȸ�� ����
        &translate);                                                         // X, Y ��ǥ ��ġ

    // ��������Ʈ�� ��ȯ�� �����Ѵ�. ("Hello Neo")
    sprite->SetTransform(&matrix);

    // ��������Ʈ�� �׸���.
    sprite->Draw(spriteData.texture, &spriteData.rect, NULL, NULL, color);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �սǵ� ��ġ �׽�Ʈ
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Graphics::getDeviceState()
{
    result = E_FAIL;                                                         // �⺻������ ����, ���� �� ��ü
    if (device3d == NULL)
        return  result;
    result = device3d->TestCooperativeLevel();
    return result;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �׷��� ��ġ �缳��
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Graphics::reset()
{
    result = E_FAIL;                                                         // �⺻������ ����, ���� �� ��ü
    initD3Dpp();                                                             // D3D ���������̼� �Ű����� �ʱ�ȭ
    sprite->OnLostDevice();
    result = device3d->Reset(&d3dpp);                                        // �׷��� ��ġ �缳�� �õ�

    // �⺻ ������ ���� ȥ�� ����
    device3d->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    device3d->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    sprite->OnResetDevice();
    return result;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// â ��� �Ǵ� ��ü ȭ�� ��� ��ȯ
// ���� ����: 
//          ��� ����ڰ� ������ D3DPOOL_DEFAULT ǥ���� �����˴ϴ�.
// ���� ����: 
//          ��� ����� ǥ���� �ٽ� �����˴ϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Graphics::changeDisplayMode(GraphicsNS::DISPLAY_MODE mode)
{
    try {
        switch (mode)
        {
        case GraphicsNS::FULLSCREEN:
            if (fullscreen)                                                  // �̹� ��ü ȭ�� ����� ���
                return;
            fullscreen = true; break;
        case GraphicsNS::WINDOW:
            if (fullscreen == false)                                         // �̹� â ����� ���
                return;
            fullscreen = false; break;
        default:                                                             // �⺻������ â/��ü ȭ�� ��ȯ
            fullscreen = !fullscreen;
        }
        reset();
        if (fullscreen)                                                      // ��ü ȭ��
        {
            SetWindowLong(hwnd, GWL_STYLE, WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP);
        }
        else                                                                 // â ���
        {
            SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
            SetWindowPos(hwnd, HWND_TOP, 0, 0, GAME_WIDTH, GAME_HEIGHT,
                SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

            // Ŭ���̾�Ʈ ������ GAME_WIDTH x GAME_HEIGHT�� �ǵ��� â ũ�� ����
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);                                // â�� Ŭ���̾�Ʈ ���� ũ�� ��������
            MoveWindow(hwnd,
                0,                                                           // ����
                0,                                                           // ����
                GAME_WIDTH + (GAME_WIDTH - clientRect.right),                // ������
                GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom),             // �Ʒ���
                TRUE);                                                       // â �ٽ� �׸���
        }

    }
    catch (...)
    {
        // ������ �߻��߽��ϴ�. â ��带 �õ��ϼ���. 
        SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowPos(hwnd, HWND_TOP, 0, 0, GAME_WIDTH, GAME_HEIGHT,
            SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

        // Ŭ���̾�Ʈ ������ GAME_WIDTH x GAME_HEIGHT�� �ǵ��� â ũ�� ����
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);                                    // â�� Ŭ���̾�Ʈ ���� ũ�� ��������
        MoveWindow(hwnd,
            0,                                                               // ����
            0,                                                               // ����
            GAME_WIDTH + (GAME_WIDTH - clientRect.right),                    // ������
            GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom),                 // �Ʒ���
            TRUE);                                                           // â �ٽ� �׸���
    }
}