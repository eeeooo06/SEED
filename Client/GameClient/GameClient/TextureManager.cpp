#include "TextureManager.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ������
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
TextureManager::TextureManager()
{
    texture = NULL;
    width = 0;
    height = 0;
    file = NULL;
    graphics = NULL;
    initialized = false;                                                     // ���������� �ʱ�ȭ�Ǹ� true�� ����
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �Ҹ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
TextureManager::~TextureManager()
{
    SAFE_RELEASE(texture);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ��ũ���� �ؽ�ó ������ �ε��մϴ�.
// ��������: 
//          �����ϸ� true, �����ϸ� false�� ��ȯ�մϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool TextureManager::initialize(Graphics* g, const char* f)
{
    try {
        graphics = g;                                                        // �׷��� ��ü
        file = f;                                                            // �ؽ�ó ����

        hr = graphics->loadTexture(file, TRANSCOLOR, width, height, texture);
        if (FAILED(hr))
        {
            SAFE_RELEASE(texture);
            return false;
        }
    }
    catch (...) { return false; }
    initialized = true;                                                      // ���������� �ʱ�ȭ�Ǹ� true�� ����
    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �׷��� ��ġ�� �Ҿ������ �� ȣ��˴ϴ�
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void TextureManager::onLostDevice()
{
    if (!initialized)
        return;
    SAFE_RELEASE(texture);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �׷��� ��ġ�� �缳���� �� ȣ��˴ϴ�
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void TextureManager::onResetDevice()
{
    if (!initialized)
        return;
    graphics->loadTexture(file, TRANSCOLOR, width, height, texture);
}