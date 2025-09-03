#include "Image.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ������
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Image::Image()
{
    initialized = false;                                                     // ���������� �ʱ�ȭ�Ǹ� true�� ����
    spriteData.width = 2;
    spriteData.height = 2;
    spriteData.x = 0.0;
    spriteData.y = 0.0;
    spriteData.scale = 1.0;
    spriteData.angle = 0.0;
    spriteData.rect.left = 0;                                                // ���� ������ �̹������� �� �������� �����ϴ� �� ����
    spriteData.rect.top = 0;
    spriteData.rect.right = spriteData.width;
    spriteData.rect.bottom = spriteData.height;
    spriteData.texture = NULL;                                               // ��������Ʈ �ؽ�ó (�׸�)
    spriteData.flipHorizontal = false;
    spriteData.flipVertical = false;
    cols = 1;
    textureManager = NULL;
    startFrame = 0;
    endFrame = 0;
    currentFrame = 0;
    frameDelay = 1.0;                                                        // �⺻������ �ִϸ��̼� �����Ӵ� 1��
    animTimer = 0.0;
    visible = true;                                                          // �̹����� ����
    loop = true;                                                             // ������ �ݺ�
    animComplete = false;
    graphics = NULL;                                                         // �׷��� �ý��ۿ� ���� ��ũ
    colorFilter = GraphicsNS::WHITE;                                         // ���� ������ WHITE
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �Ҹ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Image::~Image()
{
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �̹����� �ʱ�ȭ�մϴ�.
// ���� ����: 
//          �����ϸ� true, �����ϸ� false�� ��ȯ�մϴ�.
// Graphics�� ���� ������
// �̹����� �ʺ� (�ȼ� ����) (0 = ��ü �ؽ�ó �ʺ� ���)
// �̹����� ���� (�ȼ� ����) (0 = ��ü �ؽ�ó ���� ���)
// �ؽ�ó�� �� �� (1���� n����) (0�� 1�� ����)
// TextureManager�� ���� ������
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
bool Image::initialize(Graphics* g, int width, int height, int ncols,
    TextureManager* textureM)
{
    try {
        graphics = g;                                                        // �׷��� ��ü
        textureManager = textureM;                                           // �ؽ�ó ��ü�� ���� ������

        spriteData.texture = textureManager->getTexture();
        if (width == 0)
            width = textureManager->getWidth();                              // �ؽ�ó�� ��ü �ʺ� ���
        spriteData.width = width;
        if (height == 0)
            height = textureManager->getHeight();                            // �ؽ�ó�� ��ü ���� ���
        spriteData.height = height;
        cols = ncols;
        if (cols == 0)
            cols = 1;                                                        // ���� 0�̸� 1�� ���

        // currentFrame�� �׸����� spriteData.rect ����
        spriteData.rect.left = (currentFrame % cols) * spriteData.width;
        // ������ �����ڸ� + 1
        spriteData.rect.right = spriteData.rect.left + spriteData.width;
        spriteData.rect.top = (currentFrame / cols) * spriteData.height;
        // �Ʒ��� �����ڸ� + 1
        spriteData.rect.bottom = spriteData.rect.top + spriteData.height;
    }
    catch (...) { return false; }
    initialized = true;                                                      // ���������� �ʱ�ȭ��
    return true;
}


// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ������ ���ͷ� ����Ͽ� �̹��� �׸���
// color �Ű������� ���� �����̸�, image.h���� �⺻������ WHITE�� �Ҵ�˴ϴ�.
// ���� ����: 
//          spriteBegin()�� ȣ���
// ���� ����: 
//          spriteEnd()�� ȣ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Image::draw(COLOR_ARGB color)
{
    if (!visible || graphics == NULL)
        return;
    // onReset()�� ȣ��� ��츦 ����Ͽ� ���ο� �ؽ�ó ��������
    spriteData.texture = textureManager->getTexture();
    if (color == GraphicsNS::FILTER)                                         // ���ͷ� �׸��� ���
        graphics->drawSprite(spriteData, colorFilter);                       // colorFilter ���
    else
        graphics->drawSprite(spriteData, color);                             // color�� ���ͷ� ���
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ������ SpriteData�� ����Ͽ� �� �̹��� �׸���
// ���� SpriteData.rect�� �ؽ�ó�� �����ϴ� �� ���˴ϴ�.
// ���� ����: 
//          spriteBegin()�� ȣ���
// ���� ����: 
//          spriteEnd()�� ȣ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Image::draw(SpriteData sd, COLOR_ARGB color)
{
    if (!visible || graphics == NULL)
        return;
    sd.rect = spriteData.rect;                                               // �� �̹����� rect�� ����Ͽ� �ؽ�ó ����
    sd.texture = textureManager->getTexture();                               // onReset()�� ȣ��� ��츦 ����Ͽ� ���ο� �ؽ�ó ��������

    if (color == GraphicsNS::FILTER)                                         // ���ͷ� �׸��� ���
        graphics->drawSprite(sd, colorFilter);                               // colorFilter ���
    else
        graphics->drawSprite(sd, color);                                     // color�� ���ͷ� ���
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ������Ʈ
// �Ϲ������� �����Ӵ� �� �� ȣ��˴ϴ�.
// frameTime�� �����Ӱ� �ִϸ��̼��� �ӵ��� �����ϴ� �� ���˴ϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Image::update(float frameTime)
{
    if (endFrame - startFrame > 0)                                           // �ִϸ��̼� ��������Ʈ�� ���
    {
        animTimer += frameTime;                                              // �� ��� �ð�
        if (animTimer > frameDelay)
        {
            animTimer -= frameDelay;
            currentFrame++;
            if (currentFrame < startFrame || currentFrame > endFrame)
            {
                if (loop == true)                                            // �ִϸ��̼��� �ݺ��ϴ� ���
                    currentFrame = startFrame;
                else                                                         // �ִϸ��̼��� �ݺ����� ����
                {
                    currentFrame = endFrame;
                    animComplete = true;                                     // �ִϸ��̼� �Ϸ�
                }
            }
            setRect();                                                       // spriteData.rect ����
        }
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �̹����� ���� ������ ����
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Image::setCurrentFrame(int c)
{
    if (c >= 0)
    {
        currentFrame = c;
        animComplete = false;
        setRect();                                                           // spriteData.rect ����
    }
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
//  currentFrame�� �׸����� spriteData.rect ����
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
inline void Image::setRect()
{
    // currentFrame�� �׸����� spriteData.rect ����
    spriteData.rect.left = (currentFrame % cols) * spriteData.width;
    // ������ �����ڸ� + 1
    spriteData.rect.right = spriteData.rect.left + spriteData.width;
    spriteData.rect.top = (currentFrame / cols) * spriteData.height;
    // �Ʒ��� �����ڸ� + 1
    spriteData.rect.bottom = spriteData.rect.top + spriteData.height;
}