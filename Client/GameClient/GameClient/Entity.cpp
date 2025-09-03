#include "Entity.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// 생성자
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
    active = true;                                                           // 엔티티가 활성 상태임
    rotatedBoxReady = false;
    collisionType = entityNS::CIRCLE;
    health = 100;
    gravity = entityNS::GRAVITY;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// 엔티티를 초기화합니다.
// 사전 조건: 
//          *gamePtr = Game 객체에 대한 포인터
//          width = 이미지의 너비 (픽셀 단위) (0 = 전체 텍스처 너비 사용)
//          height = 이미지의 높이 (픽셀 단위) (0 = 전체 텍스처 높이 사용)
//          ncols = 텍스처의 열 수 (1에서 n까지) (0은 1과 동일)
//          *textureM = TextureManager 객체에 대한 포인터
// 사후 조건: 
//          성공하면 true, 실패하면 false를 반환합니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::initialize(Game* gamePtr, int width, int height, int ncols,
    TextureManager* textureM)
{
    input = gamePtr->getInput();                                             // 입력 시스템
    audio = gamePtr->getAudio();                                             // 오디오 시스템
    return(Image::initialize(gamePtr->getGraphics(), width, height, ncols, textureM));
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// 엔티티 활성화
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
void Entity::activate()
{
    active = true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// 업데이트
// 일반적으로 프레임당 한 번 호출됩니다.
// frameTime은 움직임과 애니메이션의 속도를 조절하는 데 사용됩니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
void Entity::update(float frameTime)
{
    velocity += deltaV;
    deltaV.x = 0;
    deltaV.y = 0;
    Image::update(frameTime);
    rotatedBoxReady = false;                                                 // 회전된 상자 충돌 감지를 위함
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// ai (인공 지능)
// 일반적으로 프레임당 한 번 호출됩니다.
// ai 계산을 수행하며, 상호 작용을 위해 ent가 전달됩니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
void Entity::ai(float frameTime, Entity& ent)
{
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// 이 엔티티와 다른 엔티티 간의 충돌 감지를 수행합니다.
// 각 엔티티는 단일 충돌 유형을 사용해야 합니다. 복잡한 모양이 필요한 경우
// 여러 충돌 유형은 각 부분을 별도의 엔티티로 처리하여 수행할 수 있습니다.
// 엔티티.
// 일반적으로 프레임당 한 번 호출됩니다.
// 충돌 유형: CIRCLE, BOX 또는 ROTATED_BOX.
// 사후 조건: 
//          충돌 시 true, 그렇지 않으면 false를 반환합니다.
//          충돌 시 collisionVector를 설정합니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::collidesWith(Entity& ent, VECTOR2& collisionVector)
{
    // 어느 한 엔티티라도 활성 상태가 아니면 충돌이 발생할 수 없습니다.
    if (!active || !ent.getActive())
        return false;

    // 두 엔티티가 모두 CIRCLE 충돌인 경우
    if (collisionType == entityNS::CIRCLE && ent.getCollisionType() == entityNS::CIRCLE)
        return collideCircle(ent, collisionVector);
    // 두 엔티티가 모두 BOX 충돌인 경우
    if (collisionType == entityNS::BOX && ent.getCollisionType() == entityNS::BOX)
        return collideBox(ent, collisionVector);
    // 다른 모든 조합은 분리 축 테스트를 사용합니다.
    // 두 엔티티 모두 CIRCLE 충돌을 사용하지 않는 경우
    if (collisionType != entityNS::CIRCLE && ent.getCollisionType() != entityNS::CIRCLE)
        return collideRotatedBox(ent, collisionVector);
    else    // 엔티티 중 하나는 원입니다.
        if (collisionType == entityNS::CIRCLE)                               // 이 엔티티가 CIRCLE 충돌을 사용하는 경우
        {
            // 다른 상자와 우리 원의 충돌을 확인합니다.
            bool collide = ent.collideRotatedBoxCircle(*this, collisionVector);
            // 충돌 벡터를 올바른 방향으로 설정합니다.
            collisionVector *= -1;                                           // 충돌 벡터 반전
            return collide;
        }
        else    // 다른 엔티티가 CIRCLE 충돌을 사용합니다.
            return collideRotatedBoxCircle(ent, collisionVector);
    return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// 원형 충돌 감지 방법
// collision()에 의해 호출되는 기본 충돌 감지 방법
// 사후 조건: 
//          충돌 시 true, 그렇지 않으면 false를 반환합니다.
//          충돌 시 collisionVector를 설정합니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::collideCircle(Entity& ent, VECTOR2& collisionVector)
{
    // 중심 간의 차이
    distSquared = *getCenter() - *ent.getCenter();
    distSquared.x = distSquared.x * distSquared.x;                           // 차이의 제곱
    distSquared.y = distSquared.y * distSquared.y;

    // 반지름의 합을 계산합니다 (스케일 조정됨).
    sumRadiiSquared = (radius * getScale()) + (ent.radius * ent.getScale());
    sumRadiiSquared *= sumRadiiSquared;                                      // 제곱합니다.

    // 엔티티가 충돌하는 경우
    if (distSquared.x + distSquared.y <= sumRadiiSquared)
    {
        // 충돌 벡터 설정
        collisionVector = *ent.getCenter() - *getCenter();
        return true;
    }
    return false;                                                            // 충돌하지 않음
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// 축 정렬 경계 상자 충돌 감지 방법
// collision()에 의해 호출됨
// 사후 조건: 
//          충돌 시 true, 그렇지 않으면 false를 반환합니다.
//          충돌 시 collisionVector를 설정합니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::collideBox(Entity& ent, VECTOR2& collisionVector)
{
    // 어느 한 엔티티라도 활성 상태가 아니면 충돌이 발생할 수 없습니다.
    if (!active || !ent.getActive())
        return false;

    // 축 정렬 경계 상자를 사용하여 충돌을 확인합니다.
    if ((getCenterX() + edge.right * getScale() >= ent.getCenterX() + ent.getEdge().left * ent.getScale()) &&
        (getCenterX() + edge.left * getScale() <= ent.getCenterX() + ent.getEdge().right * ent.getScale()) &&
        (getCenterY() + edge.bottom * getScale() >= ent.getCenterY() + ent.getEdge().top * ent.getScale()) &&
        (getCenterY() + edge.top * getScale() <= ent.getCenterY() + ent.getEdge().bottom * ent.getScale()))
    {
        // 충돌 벡터 설정
        collisionVector = *ent.getCenter() - *getCenter();
        return true;
    }
    return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// 회전된 상자 충돌 감지 방법
// collision()에 의해 호출됨
// 사후 조건: 
//          충돌 시 true, 그렇지 않으면 false를 반환합니다.
//          충돌 시 collisionVector를 설정합니다.
// 분리 축 테스트를 사용하여 충돌을 감지합니다.
// 분리 축 테스트:
//          두 상자의 선에 대한 투영이 겹치지 않으면 충돌하지 않습니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::collideRotatedBox(Entity& ent, VECTOR2& collisionVector)
{
    computeRotatedBox();                                                     // 회전된 상자 준비
    ent.computeRotatedBox();                                                 // 회전된 상자 준비
    if (projectionsOverlap(ent) && ent.projectionsOverlap(*this))
    {
        // 충돌 벡터 설정
        collisionVector = *ent.getCenter() - *getCenter();
        return true;
    }
    return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// 다른 상자를 이 edge01 및 edge03에 투영합니다.
// collideRotatedBox()에 의해 호출됨
// 사후 조건: 
//          투영이 겹치면 true, 그렇지 않으면 false를 반환합니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::projectionsOverlap(Entity& ent)
{
    float projection, min01, max01, min03, max03;

    // 다른 상자를 edge01에 투영
    projection = graphics->Vector2Dot(&edge01, ent.getCorner(0));            // 모서리 0 투영
    min01 = projection;
    max01 = projection;
    // 나머지 각 모서리에 대해
    for (int c = 1; c < 4; c++)
    {
        // 모서리를 edge01에 투영
        projection = graphics->Vector2Dot(&edge01, ent.getCorner(c));
        if (projection < min01)
            min01 = projection;
        else if (projection > max01)
            max01 = projection;
    }
    if (min01 > edge01Max || max01 < edge01Min)                              // 투영이 겹치지 않으면
        return false;                                                        // 충돌이 불가능함

    // 다른 상자를 edge03에 투영
    projection = graphics->Vector2Dot(&edge03, ent.getCorner(0));            // 모서리 0 투영
    min03 = projection;
    max03 = projection;
    // 나머지 각 모서리에 대해
    for (int c = 1; c < 4; c++)
    {
        // 모서리를 edge03에 투영
        projection = graphics->Vector2Dot(&edge03, ent.getCorner(c));
        if (projection < min03)
            min03 = projection;
        else if (projection > max03)
            max03 = projection;
    }
    if (min03 > edge03Max || max03 < edge03Min)                              // 투영이 겹치지 않으면
        return false;                                                        // 충돌이 불가능함

    return true;                                                             // 투영이 겹침
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
// 회전된 상자와 원 충돌 감지 방법
// collision()에 의해 호출됨
// 상자의 가장자리와 원의 반지름에 분리 축 테스트를 사용합니다.
// 원의 중심이 충돌 상자 가장자리에서 확장된 선(Voronoi 영역이라고도 함) 외부에 있는 경우
// 가장 가까운 상자 모서리가 거리 검사를 사용하여 충돌을 확인합니다.
// 거리 검사를 사용하여 충돌을 확인합니다.
// 가장 가까운 모서리는 겹침 테스트에서 결정됩니다.
//
//   Voronoi0 |   | Voronoi1
//         ---0---1---
//            |   |
//         ---3---2---
//   Voronoi3 |   | Voronoi2
//
// 사전 조건: 
//          이 엔티티는 상자여야 하고 다른 엔티티(ent)는 원이어야 합니다.
// 사후 조건: 
//          충돌 시 true, 그렇지 않으면 false를 반환합니다.
//          충돌 시 collisionVector를 설정합니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
bool Entity::collideRotatedBoxCircle(Entity& ent, VECTOR2& collisionVector)
{
    float min01, min03, max01, max03, center01, center03;

    computeRotatedBox();                                                     // 회전된 상자 준비

    // 원 중심을 edge01에 투영
    center01 = graphics->Vector2Dot(&edge01, ent.getCenter());
    min01 = center01 - ent.getRadius() * ent.getScale();                     // 최소 및 최대는 중심으로부터의 반지름입니다.
    max01 = center01 + ent.getRadius() * ent.getScale();
    if (min01 > edge01Max || max01 < edge01Min)                              // 투영이 겹치지 않으면
        return false;                                                        // 충돌이 불가능함

    // 원 중심을 edge03에 투영
    center03 = graphics->Vector2Dot(&edge03, ent.getCenter());
    min03 = center03 - ent.getRadius() * ent.getScale();                     // 최소 및 최대는 중심으로부터의 반지름입니다.
    max03 = center03 + ent.getRadius() * ent.getScale();
    if (min03 > edge03Max || max03 < edge03Min)                              // 투영이 겹치지 않으면
        return false;                                                        // 충돌이 불가능함

    // 원 투영이 상자 투영과 겹침
    // 원이 충돌 상자의 보로노이 영역에 있는지 확인
    if (center01 < edge01Min && center03 < edge03Min)                        // 원이 Voronoi0에 있는 경우
        return collideCornerCircle(corners[0], ent, collisionVector);
    if (center01 > edge01Max && center03 < edge03Min)                        // 원이 Voronoi1에 있는 경우
        return collideCornerCircle(corners[1], ent, collisionVector);
    if (center01 > edge01Max && center03 > edge03Max)                        // 원이 Voronoi2에 있는 경우
        return collideCornerCircle(corners[2], ent, collisionVector);
    if (center01 < edge01Min && center03 > edge03Max)                        // 원이 Voronoi3에 있는 경우
        return collideCornerCircle(corners[3], ent, collisionVector);

    // 원이 보로노이 영역에 없으므로 상자의 가장자리와 충돌하고 있습니다.
    // 충돌 벡터를 설정하고, 원의 중심에서 상자의 중심까지의 단순한 벡터를 사용합니다.
    collisionVector = *ent.getCenter() - *getCenter();
    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 상자 모서리는 거리 검사를 사용하여 원과의 충돌을 확인합니다.
// collideRotatedBoxCircle()에 의해 호출됨
// 사후 조건: 
//          충돌 시 true, 그렇지 않으면 false를 반환합니다.
//          충돌 시 collisionVector를 설정합니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
bool Entity::collideCornerCircle(VECTOR2 corner, Entity& ent, VECTOR2& collisionVector)
{
    distSquared = corner - *ent.getCenter();                                 // 모서리 - 원
    distSquared.x = distSquared.x * distSquared.x;                           // 차이의 제곱
    distSquared.y = distSquared.y * distSquared.y;

    // 반지름의 합을 계산한 다음 제곱합니다.                                      
    sumRadiiSquared = ent.getRadius() * ent.getScale();                      // (0 + 원의 반지름)
    sumRadiiSquared *= sumRadiiSquared;                                      // 제곱합니다.

    // 모서리와 원이 충돌하는 경우
    if (distSquared.x + distSquared.y <= sumRadiiSquared)
    {
        // 충돌 벡터 설정
        collisionVector = *ent.getCenter() - corner;
        return true;
    }
    return false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 회전된 상자의 모서리, 투영 가장자리 및 최소/최대 투영 계산
// 0---1  모서리 번호
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

    // corners[0]은 원점으로 사용됩니다.
    // corners[0]에 연결된 두 가장자리는 투영선으로 사용됩니다.
    edge01 = VECTOR2(corners[1].x - corners[0].x, corners[1].y - corners[0].y);
    graphics->Vector2Normalize(&edge01);
    edge03 = VECTOR2(corners[3].x - corners[0].x, corners[3].y - corners[0].y);
    graphics->Vector2Normalize(&edge03);

    // 이 엔티티의 가장자리에 대한 최소 및 최대 투영
    projection = graphics->Vector2Dot(&edge01, &corners[0]);
    edge01Min = projection;
    edge01Max = projection;
    // edge01에 투영
    projection = graphics->Vector2Dot(&edge01, &corners[1]);
    if (projection < edge01Min)
        edge01Min = projection;
    else if (projection > edge01Max)
        edge01Max = projection;
    // edge03에 투영
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
// 이 엔티티가 지정된 사각형 외부에 있습니까?
// 사후 조건: 
//          사각형 외부에 있으면 true, 그렇지 않으면 false를 반환합니다.
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
// 피해
// 이 엔티티는 무기에 의해 피해를 입었습니다.
// 상속 클래스에서 이 함수를 재정의하십시오.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Entity::damage(int weapon)
{
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 다른 엔티티와 충돌 후 엔티티가 튕겨 나옵니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Entity::bounce(VECTOR2& collisionVector, Entity& ent)
{
    VECTOR2 Vdiff = ent.getVelocity() - velocity;
    VECTOR2 cUV = collisionVector;                                           // 충돌 단위 벡터
    Graphics::Vector2Normalize(&cUV);
    float cUVdotVdiff = Graphics::Vector2Dot(&cUV, &Vdiff);
    float massRatio = 2.0f;
    if (getMass() != 0)
        massRatio *= (ent.getMass() / (getMass() + ent.getMass()));

    // 엔티티가 이미 서로 멀어지고 있다면 이전에 bounce가 호출되었고
    // 여전히 충돌하고 있는 상태여야 합니다.
    // collisionVector를 따라 엔티티를 떼어 놓습니다.
    if (cUVdotVdiff > 0)
    {
        setX(getX() - cUV.x * massRatio);
        setY(getY() - cUV.y * massRatio);
    }
    else
        deltaV += ((massRatio * cUVdotVdiff) * cUV);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 다른 엔티티가 이 엔티티에 가하는 중력
// 이 엔티티의 속도 벡터에 중력을 더합니다.
// 힘 = 중력상수 * 질량1 * 질량2 / 거리^2
//                    2              2
// 거리^2  =   (Ax - Bx)   +  (Ay - By)
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Entity::gravityForce(Entity* ent, float frameTime)
{
    // 어느 한 엔티티라도 활성 상태가 아니면 중력 효과가 없습니다.
    if (!active || !ent->getActive())
        return;

    rr = pow((ent->getCenterX() - getCenterX()), 2) +
        pow((ent->getCenterY() - getCenterY()), 2);
    force = gravity * ent->getMass() * mass / rr;

    // --- 벡터 수학을 사용하여 중력 벡터 생성 ---
    // 엔티티 간의 벡터 생성
    VECTOR2 gravityV(ent->getCenterX() - getCenterX(),
        ent->getCenterY() - getCenterY());
    // 벡터 정규화
    Graphics::Vector2Normalize(&gravityV);
    // 중력의 힘을 곱하여 중력 벡터 생성
    gravityV *= force * frameTime;
    // 움직이는 속도 벡터에 중력 벡터를 더하여 방향 변경
    velocity += gravityV;
}