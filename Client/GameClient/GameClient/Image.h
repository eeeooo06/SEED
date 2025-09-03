#ifndef _IMAGE_H                                                             // �� ������ �� �� �̻� ���Ե� ��� ���� ���Ǹ� �����մϴ�.
#define _IMAGE_H                
#define WIN32_LEAN_AND_MEAN

#include "TextureManager.h"
#include "Constants.h"

class Image
{
    // �̹��� �Ӽ�
protected:
    Graphics* graphics;                                                      // �׷��ȿ� ���� ������
    TextureManager* textureManager;                                          // �ؽ�ó �����ڿ� ���� ������
    // spriteData�� Graphics::drawSprite()�� �̹����� �׸��� �� �ʿ��� �����͸� �����մϴ�.
    SpriteData spriteData;                                                   // SpriteData�� "graphics.h"�� ���ǵǾ� �ֽ��ϴ�.
    COLOR_ARGB colorFilter;                                                  // ���� ���ͷ� ����� (�������� �������� WHITE ���)
    int     cols;                                                            // ���� ������ ��������Ʈ�� �� �� (1���� n����)
    int     startFrame;                                                      // ���� �ִϸ��̼��� ù ������
    int     endFrame;                                                        // ���� �ִϸ��̼��� ������ ������
    int     currentFrame;                                                    // ���� �ִϸ��̼� ������
    float   frameDelay;                                                      // �ִϸ��̼� ������ ������ �ð�
    float   animTimer;                                                       // �ִϸ��̼� Ÿ�̸�
    HRESULT hr;                                                              // ǥ�� ��ȯ ����
    bool    loop;                                                            // �������� �ݺ��Ϸ��� true
    bool    visible;                                                         // ���� �� true
    bool    initialized;                                                     // ���������� �ʱ�ȭ�Ǹ� true
    bool    animComplete;                                                    // loop�� false�̰� endFrame ǥ�ð� �Ϸ�Ǹ� true

public:
    // ������
    Image();
    // �Ҹ���
    virtual ~Image();

    ////////////////////////////////////////
    //           Get �Լ�            //
    ////////////////////////////////////////

    // SpriteData ����ü�� ���� ������ ��ȯ�մϴ�.
    const virtual SpriteData& getSpriteInfo() { return spriteData; }

    // visible �Ű������� ��ȯ�մϴ�.
    virtual bool  getVisible() { return visible; }

    // X ��ġ�� ��ȯ�մϴ�.
    virtual float getX() { return spriteData.x; }

    // Y ��ġ�� ��ȯ�մϴ�.
    virtual float getY() { return spriteData.y; }

    // ������ ���͸� ��ȯ�մϴ�.
    virtual float getScale() { return spriteData.scale; }

    // �ʺ� ��ȯ�մϴ�.
    virtual int   getWidth() { return spriteData.width; }

    // ���̸� ��ȯ�մϴ�.
    virtual int   getHeight() { return spriteData.height; }

    // �߽� X�� ��ȯ�մϴ�.
    virtual float getCenterX() { return spriteData.x + spriteData.width / 2 * getScale(); }

    // �߽� Y�� ��ȯ�մϴ�.
    virtual float getCenterY() { return spriteData.y + spriteData.height / 2 * getScale(); }

    // ȸ�� ������ �� ������ ��ȯ�մϴ�.
    virtual float getDegrees() { return spriteData.angle * (180.0f / (float)PI); }

    // ȸ�� ������ ���� ������ ��ȯ�մϴ�.
    virtual float getRadians() { return spriteData.angle; }

    // �ִϸ��̼� ������ ������ ���� �ð��� ��ȯ�մϴ�.
    virtual float getFrameDelay() { return frameDelay; }

    // ���� ������ ��ȣ�� ��ȯ�մϴ�.
    virtual int   getStartFrame() { return startFrame; }

    // ���� ������ ��ȣ�� ��ȯ�մϴ�.
    virtual int   getEndFrame() { return endFrame; }

    // ���� ������ ��ȣ�� ��ȯ�մϴ�.
    virtual int   getCurrentFrame() { return currentFrame; }

    // �̹����� RECT ����ü�� ��ȯ�մϴ�.
    virtual RECT  getSpriteDataRect() { return spriteData.rect; }

    // �ִϸ��̼� �Ϸ� ���¸� ��ȯ�մϴ�.
    virtual bool  getAnimationComplete() { return animComplete; }

    // colorFilter�� ��ȯ�մϴ�.
    virtual COLOR_ARGB getColorFilter() { return colorFilter; }

    ////////////////////////////////////////
    //           Set �Լ�            //
    ////////////////////////////////////////

    // X ��ġ�� �����մϴ�.
    virtual void setX(float newX) { spriteData.x = newX; }

    // Y ��ġ�� �����մϴ�.
    virtual void setY(float newY) { spriteData.y = newY; }

    // �������� �����մϴ�.
    virtual void setScale(float s) { spriteData.scale = s; }

    // ȸ�� ������ �� ������ �����մϴ�.
    // 0���� �����Դϴ�. ������ �ð� �������� ����˴ϴ�.
    virtual void setDegrees(float deg) { spriteData.angle = deg * ((float)PI / 180.0f); }

    // ȸ�� ������ ���� ������ �����մϴ�.
    // 0 ������ �����Դϴ�. ������ �ð� �������� ����˴ϴ�.
    virtual void setRadians(float rad) { spriteData.angle = rad; }

    // visible�� �����մϴ�.
    virtual void setVisible(bool v) { visible = v; }

    // �ִϸ��̼� ������ ������ ���� �ð��� �����մϴ�.
    virtual void setFrameDelay(float d) { frameDelay = d; }

    // �ִϸ��̼��� ���� �� ���� �������� �����մϴ�.
    virtual void setFrames(int s, int e) { startFrame = s; endFrame = e; }

    // ���� �ִϸ��̼� �������� �����մϴ�.
    virtual void setCurrentFrame(int c);

    // currentFrame�� �׸����� spriteData.rect�� �����մϴ�.
    virtual void setRect();

    // spriteData.rect�� r�� �����մϴ�.
    virtual void setSpriteDataRect(RECT r) { spriteData.rect = r; }

    // �ִϸ��̼� ������ �����մϴ�. lp = true�̸� �����մϴ�.
    virtual void setLoop(bool lp) { loop = lp; }

    // �ִϸ��̼� �Ϸ� �ο��� �����մϴ�.
    virtual void setAnimationComplete(bool a) { animComplete = a; };

    // ���� ���͸� �����մϴ�. (�������� �������� WHITE ���)
    virtual void setColorFilter(COLOR_ARGB color) { colorFilter = color; }

    // TextureManager ����
    virtual void setTextureManager(TextureManager* textureM)
    {
        textureManager = textureM;
    }

    ////////////////////////////////////////
    //         ��Ÿ �Լ�            //
    ////////////////////////////////////////

    // �̹��� �ʱ�ȭ
    // ���� ����: 
    //          *g = Graphics ��ü�� ���� ������
    //          width = �̹����� �ʺ� (�ȼ� ����) (0 = ��ü �ؽ�ó �ʺ� ���)
    //          height = �̹����� ���� (�ȼ� ����) (0 = ��ü �ؽ�ó ���� ���)
    //          ncols = �ؽ�ó�� �� �� (1���� n����) (0�� 1�� ����)
    //          *textureM = TextureManager ��ü�� ���� ������
    virtual bool initialize(Graphics* g, int width, int height,
        int ncols, TextureManager* textureM);

    // �̹����� �������� ������ (�̷�)
    virtual void flipHorizontal(bool flip) { spriteData.flipHorizontal = flip; }

    // �̹����� �������� ������
    virtual void flipVertical(bool flip) { spriteData.flipVertical = flip; }

    // ������ ���ͷ� ����Ͽ� �̹����� �׸��ϴ�. �⺻ ������ WHITE�Դϴ�.
    virtual void draw(COLOR_ARGB color = GraphicsNS::WHITE);

    // ������ SpriteData�� ����Ͽ� �� �̹����� �׸��ϴ�.
    // ���� SpriteData.rect�� �ؽ�ó�� �����ϴ� �� ���˴ϴ�.
    virtual void draw(SpriteData sd, COLOR_ARGB color = GraphicsNS::WHITE);  // ������ ���ͷ� ����Ͽ� SpriteData�� �׸��ϴ�.

    // �ִϸ��̼��� ������Ʈ�մϴ�. frameTime�� �ӵ��� �����ϴ� �� ���˴ϴ�.
    virtual void update(float frameTime);
};

#endif