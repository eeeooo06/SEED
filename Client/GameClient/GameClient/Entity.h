#ifndef _ENTITY_H                                                            // �� ������ �� �� �̻� ���Ե� ��� ���� ���Ǹ� �����մϴ�.
#define _ENTITY_H               
#define WIN32_LEAN_AND_MEAN

#include "Image.h"
#include "Input.h"
#include "Game.h"

namespace entityNS
{
    enum COLLISION_TYPE { NONE, CIRCLE, BOX, ROTATED_BOX };
    const float GRAVITY = 6.67428e-11f;                                      // �߷� ���
}

class Entity : public Image
{
    // ��ƼƼ �Ӽ�
protected:
    entityNS::COLLISION_TYPE collisionType;
    VECTOR2 center;                                                          // ��ƼƼ�� �߽�
    float   radius;                                                          // �浹 ���� ������
    VECTOR2 distSquared;                                                     // �� �浹 ��꿡 ����
    float   sumRadiiSquared;
    // edge�� ��ƼƼ �߽ɿ� ���� �浹 ���ڸ� �����մϴ�.
    // ���ʰ� ������ �Ϲ������� �����Դϴ�.
    RECT    edge;                                                            // BOX �� ROTATED_BOX �浹 ������ ����
    VECTOR2 corners[4];                                                      // ROTATED_BOX �浹 ������ ����
    VECTOR2 edge01, edge03;                                                  // ������ ���Ǵ� �����ڸ�
    float   edge01Min, edge01Max, edge03Min, edge03Max;                      // �ּ� �� �ִ� ����
    VECTOR2 velocity;                                                        // �ӵ�
    VECTOR2 deltaV;                                                          // ���� update() ȣ�� �� �ӵ��� �߰���
    float   mass;                                                            // ��ƼƼ�� ����
    float   health;                                                          // ü�� 0���� 100
    float   rr;                                                              // ������ ���� ����
    float   force;                                                           // �߷��� ��
    float   gravity;                                                         // ���� ������ �߷� ���
    Input* input;                                                            // �Է� �ý��ۿ� ���� ������
    Audio* audio;                                                            // ����� �ý��ۿ� ���� ������
    HRESULT hr;                                                              // ǥ�� ��ȯ ����
    bool    active;                                                          // Ȱ�� ��ƼƼ�� �浹�� �� ����
    bool    rotatedBoxReady;                                                 // ȸ���� �浹 ���ڰ� �غ�Ǹ� true

    // --- ���� �Լ����� Ŭ���� �ܺο��� ȣ��ǵ��� �ǵ����� �ʾұ� ������ protected�� ����Ǿ����ϴ�.
    // --- Ŭ���� �ܺο��� ȣ��˴ϴ�.
    // ���� �浹 ����
    // ���� ����: 
    //          &ent = �ٸ� ��ƼƼ
    // ���� ����: 
    //          &collisionVector�� �浹 ���͸� �����մϴ�.
    virtual bool collideCircle(Entity& ent, VECTOR2& collisionVector);
    // �� ���� ���� �浹 ����
    // ���� ����: 
    //          &ent = �ٸ� ��ƼƼ
    // ���� ����: 
    //          &collisionVector�� �浹 ���͸� �����մϴ�.
    virtual bool collideBox(Entity& ent, VECTOR2& collisionVector);
    // ���� ���� �и� �� �浹 ����
    // ���� ����: 
    //          &ent = �ٸ� ��ƼƼ
    // ���� ����: 
    //          &collisionVector�� �浹 ���͸� �����մϴ�.
    virtual bool collideRotatedBox(Entity& ent, VECTOR2& collisionVector);
    // ���ڿ� �� ���� �и� �� �浹 ����
    // ���� ����: 
    //          &ent = �ٸ� ��ƼƼ
    // ���� ����: 
    //          &collisionVector�� �浹 ���͸� �����մϴ�.
    virtual bool collideRotatedBoxCircle(Entity& ent, VECTOR2& collisionVector);
    // �и� �� �浹 ���� ����� �Լ�
    void computeRotatedBox();
    bool projectionsOverlap(Entity& ent);
    bool collideCornerCircle(VECTOR2 corner, Entity& ent, VECTOR2& collisionVector);

public:
    // ������
    Entity();
    // �Ҹ���
    virtual ~Entity() {}

    ////////////////////////////////////////
    //           Get �Լ�            //
    ////////////////////////////////////////

    // �����ϵ� ��ƼƼ�� �߽��� ȭ�� x,y�� ��ȯ�մϴ�.
    virtual const VECTOR2* getCenter()
    {
        center = VECTOR2(getCenterX(), getCenterY());
        return &center;
    }

    // �浹 ���� �������� ��ȯ�մϴ�.
    virtual float getRadius() const { return radius; }

    // BOX �� ROTATED_BOX �浹 ������ ���Ǵ� RECT ����ü�� ��ȯ�մϴ�.
    virtual const RECT& getEdge() const { return edge; }

    // ROTATED_BOX�� �𼭸� c�� ��ȯ�մϴ�.
    virtual const VECTOR2* getCorner(UINT c) const
    {
        if (c >= 4)
            c = 0;
        return &corners[c];
    }

    // �ӵ� ���͸� ��ȯ�մϴ�.
    virtual const VECTOR2 getVelocity() const { return velocity; }

    // active ���¸� ��ȯ�մϴ�.
    virtual bool  getActive()         const { return active; }

    // ������ ��ȯ�մϴ�.
    virtual float getMass()           const { return mass; }

    // �߷� ����� ��ȯ�մϴ�.
    virtual float getGravity()        const { return gravity; }

    // ü���� ��ȯ�մϴ�.
    virtual float getHealth()         const { return health; }

    // �浹 ����(NONE, CIRCLE, BOX, ROTATED_BOX)�� ��ȯ�մϴ�.
    virtual entityNS::COLLISION_TYPE getCollisionType() { return collisionType; }

    ////////////////////////////////////////
    //           Set �Լ�            //
    ////////////////////////////////////////

    // �ӵ��� �����մϴ�.
    virtual void  setVelocity(VECTOR2 v) { velocity = v; }

    // ��Ÿ �ӵ��� �����մϴ�. update()���� �ӵ��� �߰��˴ϴ�.
    virtual void  setDeltaV(VECTOR2 dv) { deltaV = dv; }

    // active ���¸� �����մϴ�.
    virtual void  setActive(bool a) { active = a; }

    // ü���� �����մϴ�.
    virtual void setHealth(float h) { health = h; }

    // ������ �����մϴ�.
    virtual void  setMass(float m) { mass = m; }

    // �߷� ����� �����մϴ�. �⺻���� 6.67428e-11�Դϴ�.
    virtual void  setGravity(float g) { gravity = g; }

    // �浹 ���� �������� �����մϴ�.
    virtual void setCollisionRadius(float r) { radius = r; }

    // �浹 ����(NONE, CIRCLE, BOX, ROTATED_BOX)�� �����մϴ�.
    virtual void setCollisionType(entityNS::COLLISION_TYPE ctype)
    {
        collisionType = ctype;
    }

    // BOX �� ROTATED_BOX �浹 ������ ���Ǵ� RECT ����ü�� �����մϴ�.
    void setEdge(RECT e) { edge = e; }


    ////////////////////////////////////////
    //         ��Ÿ �Լ�            //
    ////////////////////////////////////////

    // ��ƼƼ�� ������Ʈ�մϴ�.
    // �Ϲ������� �����Ӵ� �� �� ȣ��˴ϴ�.
    // frameTime�� �����Ӱ� �ִϸ��̼��� �ӵ��� �����ϴ� �� ���˴ϴ�.
    virtual void update(float frameTime);

    // ��ƼƼ �ʱ�ȭ
    // ���� ����: 
    //          *gamePtr = Game ��ü�� ���� ������
    //          width = �̹����� �ʺ� (�ȼ� ����) (0 = ��ü �ؽ�ó �ʺ� ���)
    //          height = �̹����� ���� (�ȼ� ����) (0 = ��ü �ؽ�ó ���� ���)
    //          ncols = �ؽ�ó�� �� �� (1���� n����) (0�� 1�� ����)
    //          *textureM = TextureManager ��ü�� ���� ������
    virtual bool initialize(Game* gamePtr, int width, int height, int ncols,
        TextureManager* textureM);
    // ��ƼƼ�� Ȱ��ȭ�մϴ�.
    virtual void activate();

    // ��ƼƼ ��ü�� �ν��Ͻ�ȭ�� �� �ֵ��� �ϴ� �� ai �Լ��Դϴ�.
    virtual void ai(float frameTime, Entity& ent);

    // �� ��ƼƼ�� ������ �簢�� �ۿ� �ֽ��ϱ�?
    virtual bool outsideRect(RECT rect);

    // �� ��ƼƼ�� ent�� �浹�մϱ�?
    virtual bool collidesWith(Entity& ent, VECTOR2& collisionVector);

    // ����� �� ��ƼƼ�� ���ظ� �ݴϴ�.
    virtual void damage(int weapon);

    // �ٸ� ��ƼƼ�� �浹 �� ��ƼƼ�� ƨ�� ���ɴϴ�.
    void bounce(VECTOR2& collisionVector, Entity& ent);

    // �� ��ƼƼ�� �ӵ� ���Ϳ� �߷��� ���մϴ�.
    void gravityForce(Entity* other, float frameTime);
};

#endif