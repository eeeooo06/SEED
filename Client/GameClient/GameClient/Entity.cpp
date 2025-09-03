#include "Entity.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// ������
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
Entity::Entity() : Image()
{
    radius = 1.0;
    edge.left = -1;
    edge.top = -1;
    edge.right = 1;
    edge.bottom = 1;
    mass = 1.0;
    velocity.x = 0.0;
    velocity.y = 0.0;
    deltaV.x = 0.0;
    deltaV.y = 0.0;
    active = true;                                                           // ��ƼƼ�� Ȱ�� ������
    rotatedBoxReady = false;
    collisionType = entityNS::CIRCLE;
    health = 100;
    gravity = entityNS::GRAVITY;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// ��ƼƼ�� �ʱ�ȭ�մϴ�.
// ���� ����: 
//          *gamePtr = Game ��ü�� ���� ������
//          width = �̹����� �ʺ� (�ȼ� ����) (0 = ��ü �ؽ�ó �ʺ� ���)
//          height = �̹����� ���� (�ȼ� ����) (0 = ��ü �ؽ�ó ���� ���)
//          ncols = �ؽ�ó�� �� �� (1���� n����) (0�� 1�� ����)
//          *textureM = TextureManager ��ü�� ���� ������
// ���� ����: 
//          �����ϸ� true, �����ϸ� false�� ��ȯ�մϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::initialize(Game* gamePtr, int width, int height, int ncols,
    TextureManager* textureM)
{
    input = gamePtr->getInput();                                             // �Է� �ý���
    audio = gamePtr->getAudio();                                             // ����� �ý���
    return(Image::initialize(gamePtr->getGraphics(), width, height, ncols, textureM));
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// ��ƼƼ Ȱ��ȭ
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
void Entity::activate()
{
    active = true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// ������Ʈ
// �Ϲ������� �����Ӵ� �� �� ȣ��˴ϴ�.
// frameTime�� �����Ӱ� �ִϸ��̼��� �ӵ��� �����ϴ� �� ���˴ϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
void Entity::update(float frameTime)
{
    velocity += deltaV;
    deltaV.x = 0;
    deltaV.y = 0;
    Image::update(frameTime);
    rotatedBoxReady = false;                                                 // ȸ���� ���� �浹 ������ ����
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// ai (�ΰ� ����)
// �Ϲ������� �����Ӵ� �� �� ȣ��˴ϴ�.
// ai ����� �����ϸ�, ��ȣ �ۿ��� ���� ent�� ���޵˴ϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
void Entity::ai(float frameTime, Entity& ent)
{
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// �� ��ƼƼ�� �ٸ� ��ƼƼ ���� �浹 ������ �����մϴ�.
// �� ��ƼƼ�� ���� �浹 ������ ����ؾ� �մϴ�. ������ ����� �ʿ��� ���
// ���� �浹 ������ �� �κ��� ������ ��ƼƼ�� ó���Ͽ� ������ �� �ֽ��ϴ�.
// ��ƼƼ.
// �Ϲ������� �����Ӵ� �� �� ȣ��˴ϴ�.
// �浹 ����: CIRCLE, BOX �Ǵ� ROTATED_BOX.
// ���� ����: 
//          �浹 �� true, �׷��� ������ false�� ��ȯ�մϴ�.
//          �浹 �� collisionVector�� �����մϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::collidesWith(Entity& ent, VECTOR2& collisionVector)
{
    // ��� �� ��ƼƼ�� Ȱ�� ���°� �ƴϸ� �浹�� �߻��� �� �����ϴ�.
    if (!active || !ent.getActive())
        return false;

    // �� ��ƼƼ�� ��� CIRCLE �浹�� ���
    if (collisionType == entityNS::CIRCLE && ent.getCollisionType() == entityNS::CIRCLE)
        return collideCircle(ent, collisionVector);
    // �� ��ƼƼ�� ��� BOX �浹�� ���
    if (collisionType == entityNS::BOX && ent.getCollisionType() == entityNS::BOX)
        return collideBox(ent, collisionVector);
    // �ٸ� ��� ������ �и� �� �׽�Ʈ�� ����մϴ�.
    // �� ��ƼƼ ��� CIRCLE �浹�� ������� �ʴ� ���
    if (collisionType != entityNS::CIRCLE && ent.getCollisionType() != entityNS::CIRCLE)
        return collideRotatedBox(ent, collisionVector);
    else    // ��ƼƼ �� �ϳ��� ���Դϴ�.
        if (collisionType == entityNS::CIRCLE)                               // �� ��ƼƼ�� CIRCLE �浹�� ����ϴ� ���
        {
            // �ٸ� ���ڿ� �츮 ���� �浹�� Ȯ���մϴ�.
            bool collide = ent.collideRotatedBoxCircle(*this, collisionVector);
            // �浹 ���͸� �ùٸ� �������� �����մϴ�.
            collisionVector *= -1;                                           // �浹 ���� ����
            return collide;
        }
        else    // �ٸ� ��ƼƼ�� CIRCLE �浹�� ����մϴ�.
            return collideRotatedBoxCircle(ent, collisionVector);
    return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// ���� �浹 ���� ���
// collision()�� ���� ȣ��Ǵ� �⺻ �浹 ���� ���
// ���� ����: 
//          �浹 �� true, �׷��� ������ false�� ��ȯ�մϴ�.
//          �浹 �� collisionVector�� �����մϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::collideCircle(Entity& ent, VECTOR2& collisionVector)
{
    // �߽� ���� ����
    distSquared = *getCenter() - *ent.getCenter();
    distSquared.x = distSquared.x * distSquared.x;                           // ������ ����
    distSquared.y = distSquared.y * distSquared.y;

    // �������� ���� ����մϴ� (������ ������).
    sumRadiiSquared = (radius * getScale()) + (ent.radius * ent.getScale());
    sumRadiiSquared *= sumRadiiSquared;                                      // �����մϴ�.

    // ��ƼƼ�� �浹�ϴ� ���
    if (distSquared.x + distSquared.y <= sumRadiiSquared)
    {
        // �浹 ���� ����
        collisionVector = *ent.getCenter() - *getCenter();
        return true;
    }
    return false;                                                            // �浹���� ����
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// �� ���� ��� ���� �浹 ���� ���
// collision()�� ���� ȣ���
// ���� ����: 
//          �浹 �� true, �׷��� ������ false�� ��ȯ�մϴ�.
//          �浹 �� collisionVector�� �����մϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::collideBox(Entity& ent, VECTOR2& collisionVector)
{
    // ��� �� ��ƼƼ�� Ȱ�� ���°� �ƴϸ� �浹�� �߻��� �� �����ϴ�.
    if (!active || !ent.getActive())
        return false;

    // �� ���� ��� ���ڸ� ����Ͽ� �浹�� Ȯ���մϴ�.
    if ((getCenterX() + edge.right * getScale() >= ent.getCenterX() + ent.getEdge().left * ent.getScale()) &&
        (getCenterX() + edge.left * getScale() <= ent.getCenterX() + ent.getEdge().right * ent.getScale()) &&
        (getCenterY() + edge.bottom * getScale() >= ent.getCenterY() + ent.getEdge().top * ent.getScale()) &&
        (getCenterY() + edge.top * getScale() <= ent.getCenterY() + ent.getEdge().bottom * ent.getScale()))
    {
        // �浹 ���� ����
        collisionVector = *ent.getCenter() - *getCenter();
        return true;
    }
    return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// ȸ���� ���� �浹 ���� ���
// collision()�� ���� ȣ���
// ���� ����: 
//          �浹 �� true, �׷��� ������ false�� ��ȯ�մϴ�.
//          �浹 �� collisionVector�� �����մϴ�.
// �и� �� �׽�Ʈ�� ����Ͽ� �浹�� �����մϴ�.
// �и� �� �׽�Ʈ:
//          �� ������ ���� ���� ������ ��ġ�� ������ �浹���� �ʽ��ϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::collideRotatedBox(Entity& ent, VECTOR2& collisionVector)
{
    computeRotatedBox();                                                     // ȸ���� ���� �غ�
    ent.computeRotatedBox();                                                 // ȸ���� ���� �غ�
    if (projectionsOverlap(ent) && ent.projectionsOverlap(*this))
    {
        // �浹 ���� ����
        collisionVector = *ent.getCenter() - *getCenter();
        return true;
    }
    return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// �ٸ� ���ڸ� �� edge01 �� edge03�� �����մϴ�.
// collideRotatedBox()�� ���� ȣ���
// ���� ����: 
//          ������ ��ġ�� true, �׷��� ������ false�� ��ȯ�մϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::projectionsOverlap(Entity& ent)
{
    float projection, min01, max01, min03, max03;

    // �ٸ� ���ڸ� edge01�� ����
    projection = graphics->Vector2Dot(&edge01, ent.getCorner(0));            // �𼭸� 0 ����
    min01 = projection;
    max01 = projection;
    // ������ �� �𼭸��� ����
    for (int c = 1; c < 4; c++)
    {
        // �𼭸��� edge01�� ����
        projection = graphics->Vector2Dot(&edge01, ent.getCorner(c));
        if (projection < min01)
            min01 = projection;
        else if (projection > max01)
            max01 = projection;
    }
    if (min01 > edge01Max || max01 < edge01Min)                              // ������ ��ġ�� ������
        return false;                                                        // �浹�� �Ұ�����

    // �ٸ� ���ڸ� edge03�� ����
    projection = graphics->Vector2Dot(&edge03, ent.getCorner(0));            // �𼭸� 0 ����
    min03 = projection;
    max03 = projection;
    // ������ �� �𼭸��� ����
    for (int c = 1; c < 4; c++)
    {
        // �𼭸��� edge03�� ����
        projection = graphics->Vector2Dot(&edge03, ent.getCorner(c));
        if (projection < min03)
            min03 = projection;
        else if (projection > max03)
            max03 = projection;
    }
    if (min03 > edge03Max || max03 < edge03Min)                              // ������ ��ġ�� ������
        return false;                                                        // �浹�� �Ұ�����

    return true;                                                             // ������ ��ħ
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// ȸ���� ���ڿ� �� �浹 ���� ���
// collision()�� ���� ȣ���
// ������ �����ڸ��� ���� �������� �и� �� �׽�Ʈ�� ����մϴ�.
// ���� �߽��� �浹 ���� �����ڸ����� Ȯ��� ��(Voronoi �����̶�� ��) �ܺο� �ִ� ���
// ���� ����� ���� �𼭸��� �Ÿ� �˻縦 ����Ͽ� �浹�� Ȯ���մϴ�.
// �Ÿ� �˻縦 ����Ͽ� �浹�� Ȯ���մϴ�.
// ���� ����� �𼭸��� ��ħ �׽�Ʈ���� �����˴ϴ�.
//
//   Voronoi0 |   | Voronoi1
//         ---0---1---
//            |   |
//         ---3---2---
//   Voronoi3 |   | Voronoi2
//
// ���� ����: 
//          �� ��ƼƼ�� ���ڿ��� �ϰ� �ٸ� ��ƼƼ(ent)�� ���̾�� �մϴ�.
// ���� ����: 
//          �浹 �� true, �׷��� ������ false�� ��ȯ�մϴ�.
//          �浹 �� collisionVector�� �����մϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::collideRotatedBoxCircle(Entity& ent, VECTOR2& collisionVector)
{
    float min01, min03, max01, max03, center01, center03;

    computeRotatedBox();                                                     // ȸ���� ���� �غ�

    // �� �߽��� edge01�� ����
    center01 = graphics->Vector2Dot(&edge01, ent.getCenter());
    min01 = center01 - ent.getRadius() * ent.getScale();                     // �ּ� �� �ִ�� �߽����κ����� �������Դϴ�.
    max01 = center01 + ent.getRadius() * ent.getScale();
    if (min01 > edge01Max || max01 < edge01Min)                              // ������ ��ġ�� ������
        return false;                                                        // �浹�� �Ұ�����

    // �� �߽��� edge03�� ����
    center03 = graphics->Vector2Dot(&edge03, ent.getCenter());
    min03 = center03 - ent.getRadius() * ent.getScale();                     // �ּ� �� �ִ�� �߽����κ����� �������Դϴ�.
    max03 = center03 + ent.getRadius() * ent.getScale();
    if (min03 > edge03Max || max03 < edge03Min)                              // ������ ��ġ�� ������
        return false;                                                        // �浹�� �Ұ�����

    // �� ������ ���� ������ ��ħ
    // ���� �浹 ������ ���γ��� ������ �ִ��� Ȯ��
    if (center01 < edge01Min && center03 < edge03Min)                        // ���� Voronoi0�� �ִ� ���
        return collideCornerCircle(corners[0], ent, collisionVector);
    if (center01 > edge01Max && center03 < edge03Min)                        // ���� Voronoi1�� �ִ� ���
        return collideCornerCircle(corners[1], ent, collisionVector);
    if (center01 > edge01Max && center03 > edge03Max)                        // ���� Voronoi2�� �ִ� ���
        return collideCornerCircle(corners[2], ent, collisionVector);
    if (center01 < edge01Min && center03 > edge03Max)                        // ���� Voronoi3�� �ִ� ���
        return collideCornerCircle(corners[3], ent, collisionVector);

    // ���� ���γ��� ������ �����Ƿ� ������ �����ڸ��� �浹�ϰ� �ֽ��ϴ�.
    // �浹 ���͸� �����ϰ�, ���� �߽ɿ��� ������ �߽ɱ����� �ܼ��� ���͸� ����մϴ�.
    collisionVector = *ent.getCenter() - *getCenter();
    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// ���� �𼭸��� �Ÿ� �˻縦 ����Ͽ� ������ �浹�� Ȯ���մϴ�.
// collideRotatedBoxCircle()�� ���� ȣ���
// ���� ����: 
//          �浹 �� true, �׷��� ������ false�� ��ȯ�մϴ�.
//          �浹 �� collisionVector�� �����մϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
bool Entity::collideCornerCircle(VECTOR2 corner, Entity& ent, VECTOR2& collisionVector)
{
    distSquared = corner - *ent.getCenter();                                 // �𼭸� - ��
    distSquared.x = distSquared.x * distSquared.x;                           // ������ ����
    distSquared.y = distSquared.y * distSquared.y;

    // �������� ���� ����� ���� �����մϴ�.                                      
    sumRadiiSquared = ent.getRadius() * ent.getScale();                      // (0 + ���� ������)
    sumRadiiSquared *= sumRadiiSquared;                                      // �����մϴ�.

    // �𼭸��� ���� �浹�ϴ� ���
    if (distSquared.x + distSquared.y <= sumRadiiSquared)
    {
        // �浹 ���� ����
        collisionVector = *ent.getCenter() - corner;
        return true;
    }
    return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// ȸ���� ������ �𼭸�, ���� �����ڸ� �� �ּ�/�ִ� ���� ���
// 0---1  �𼭸� ��ȣ
// |   |
// 3---2
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Entity::computeRotatedBox()
{
    if (rotatedBoxReady)
        return;
    float projection;

    VECTOR2 rotatedX(cos(spriteData.angle), sin(spriteData.angle));
    VECTOR2 rotatedY(-sin(spriteData.angle), cos(spriteData.angle));

    const VECTOR2* center = getCenter();
    corners[0] = *center + rotatedX * ((float)edge.left * getScale()) +
        rotatedY * ((float)edge.top * getScale());
    corners[1] = *center + rotatedX * ((float)edge.right * getScale()) +
        rotatedY * ((float)edge.top * getScale());
    corners[2] = *center + rotatedX * ((float)edge.right * getScale()) +
        rotatedY * ((float)edge.bottom * getScale());
    corners[3] = *center + rotatedX * ((float)edge.left * getScale()) +
        rotatedY * ((float)edge.bottom * getScale());

    // corners[0]�� �������� ���˴ϴ�.
    // corners[0]�� ����� �� �����ڸ��� ���������� ���˴ϴ�.
    edge01 = VECTOR2(corners[1].x - corners[0].x, corners[1].y - corners[0].y);
    graphics->Vector2Normalize(&edge01);
    edge03 = VECTOR2(corners[3].x - corners[0].x, corners[3].y - corners[0].y);
    graphics->Vector2Normalize(&edge03);

    // �� ��ƼƼ�� �����ڸ��� ���� �ּ� �� �ִ� ����
    projection = graphics->Vector2Dot(&edge01, &corners[0]);
    edge01Min = projection;
    edge01Max = projection;
    // edge01�� ����
    projection = graphics->Vector2Dot(&edge01, &corners[1]);
    if (projection < edge01Min)
        edge01Min = projection;
    else if (projection > edge01Max)
        edge01Max = projection;
    // edge03�� ����
    projection = graphics->Vector2Dot(&edge03, &corners[0]);
    edge03Min = projection;
    edge03Max = projection;
    projection = graphics->Vector2Dot(&edge03, &corners[3]);
    if (projection < edge03Min)
        edge03Min = projection;
    else if (projection > edge03Max)
        edge03Max = projection;

    rotatedBoxReady = true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// �� ��ƼƼ�� ������ �簢�� �ܺο� �ֽ��ϱ�?
// ���� ����: 
//          �簢�� �ܺο� ������ true, �׷��� ������ false�� ��ȯ�մϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
bool Entity::outsideRect(RECT rect)
{
    if (spriteData.x + spriteData.width * getScale() < rect.left ||
        spriteData.x > rect.right ||
        spriteData.y + spriteData.height * getScale() < rect.top ||
        spriteData.y > rect.bottom)
        return true;
    return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// ����
// �� ��ƼƼ�� ���⿡ ���� ���ظ� �Ծ����ϴ�.
// ��� Ŭ�������� �� �Լ��� �������Ͻʽÿ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Entity::damage(int weapon)
{
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// �ٸ� ��ƼƼ�� �浹 �� ��ƼƼ�� ƨ�� ���ɴϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Entity::bounce(VECTOR2& collisionVector, Entity& ent)
{
    VECTOR2 Vdiff = ent.getVelocity() - velocity;
    VECTOR2 cUV = collisionVector;                                           // �浹 ���� ����
    Graphics::Vector2Normalize(&cUV);
    float cUVdotVdiff = Graphics::Vector2Dot(&cUV, &Vdiff);
    float massRatio = 2.0f;
    if (getMass() != 0)
        massRatio *= (ent.getMass() / (getMass() + ent.getMass()));

    // ��ƼƼ�� �̹� ���� �־����� �ִٸ� ������ bounce�� ȣ��Ǿ���
    // ������ �浹�ϰ� �ִ� ���¿��� �մϴ�.
    // collisionVector�� ���� ��ƼƼ�� ���� �����ϴ�.
    if (cUVdotVdiff > 0)
    {
        setX(getX() - cUV.x * massRatio);
        setY(getY() - cUV.y * massRatio);
    }
    else
        deltaV += ((massRatio * cUVdotVdiff) * cUV);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// �ٸ� ��ƼƼ�� �� ��ƼƼ�� ���ϴ� �߷�
// �� ��ƼƼ�� �ӵ� ���Ϳ� �߷��� ���մϴ�.
// �� = �߷»�� * ����1 * ����2 / �Ÿ�^2
//                    2              2
// �Ÿ�^2  =   (Ax - Bx)   +  (Ay - By)
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Entity::gravityForce(Entity* ent, float frameTime)
{
    // ��� �� ��ƼƼ�� Ȱ�� ���°� �ƴϸ� �߷� ȿ���� �����ϴ�.
    if (!active || !ent->getActive())
        return;

    rr = pow((ent->getCenterX() - getCenterX()), 2) +
        pow((ent->getCenterY() - getCenterY()), 2);
    force = gravity * ent->getMass() * mass / rr;

    // --- ���� ������ ����Ͽ� �߷� ���� ���� ---
    // ��ƼƼ ���� ���� ����
    VECTOR2 gravityV(ent->getCenterX() - getCenterX(),
        ent->getCenterY() - getCenterY());
    // ���� ����ȭ
    Graphics::Vector2Normalize(&gravityV);
    // �߷��� ���� ���Ͽ� �߷� ���� ����
    gravityV *= force * frameTime;
    // �����̴� �ӵ� ���Ϳ� �߷� ���͸� ���Ͽ� ���� ����
    velocity += gravityV;
}