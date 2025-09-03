#ifndef _ENTITY_H                                                            // 이 파일이 두 번 이상 포함될 경우 다중 정의를 방지합니다.
#define _ENTITY_H               
#define WIN32_LEAN_AND_MEAN

#include "Image.h"
#include "Input.h"
#include "Game.h"

namespace entityNS
{
    enum COLLISION_TYPE { NONE, CIRCLE, BOX, ROTATED_BOX };
    const float GRAVITY = 6.67428e-11f;                                      // 중력 상수
}

class Entity : public Image
{
    // 엔티티 속성
protected:
    entityNS::COLLISION_TYPE collisionType;
    VECTOR2 center;                                                          // 엔티티의 중심
    float   radius;                                                          // 충돌 원의 반지름
    VECTOR2 distSquared;                                                     // 원 충돌 계산에 사용됨
    float   sumRadiiSquared;
    // edge는 엔티티 중심에 대한 충돌 상자를 지정합니다.
    // 왼쪽과 위쪽은 일반적으로 음수입니다.
    RECT    edge;                                                            // BOX 및 ROTATED_BOX 충돌 감지를 위함
    VECTOR2 corners[4];                                                      // ROTATED_BOX 충돌 감지를 위함
    VECTOR2 edge01, edge03;                                                  // 투영에 사용되는 가장자리
    float   edge01Min, edge01Max, edge03Min, edge03Max;                      // 최소 및 최대 투영
    VECTOR2 velocity;                                                        // 속도
    VECTOR2 deltaV;                                                          // 다음 update() 호출 시 속도에 추가됨
    float   mass;                                                            // 엔티티의 질량
    float   health;                                                          // 체력 0에서 100
    float   rr;                                                              // 반지름 제곱 변수
    float   force;                                                           // 중력의 힘
    float   gravity;                                                         // 게임 세계의 중력 상수
    Input* input;                                                            // 입력 시스템에 대한 포인터
    Audio* audio;                                                            // 오디오 시스템에 대한 포인터
    HRESULT hr;                                                              // 표준 반환 유형
    bool    active;                                                          // 활성 엔티티만 충돌할 수 있음
    bool    rotatedBoxReady;                                                 // 회전된 충돌 상자가 준비되면 true

    // --- 다음 함수들은 클래스 외부에서 호출되도록 의도되지 않았기 때문에 protected로 선언되었습니다.
    // --- 클래스 외부에서 호출됩니다.
    // 원형 충돌 감지
    // 사전 조건: 
    //          &ent = 다른 엔티티
    // 사후 조건: 
    //          &collisionVector는 충돌 벡터를 포함합니다.
    virtual bool collideCircle(Entity& ent, VECTOR2& collisionVector);
    // 축 정렬 상자 충돌 감지
    // 사전 조건: 
    //          &ent = 다른 엔티티
    // 사후 조건: 
    //          &collisionVector는 충돌 벡터를 포함합니다.
    virtual bool collideBox(Entity& ent, VECTOR2& collisionVector);
    // 상자 간의 분리 축 충돌 감지
    // 사전 조건: 
    //          &ent = 다른 엔티티
    // 사후 조건: 
    //          &collisionVector는 충돌 벡터를 포함합니다.
    virtual bool collideRotatedBox(Entity& ent, VECTOR2& collisionVector);
    // 상자와 원 간의 분리 축 충돌 감지
    // 사전 조건: 
    //          &ent = 다른 엔티티
    // 사후 조건: 
    //          &collisionVector는 충돌 벡터를 포함합니다.
    virtual bool collideRotatedBoxCircle(Entity& ent, VECTOR2& collisionVector);
    // 분리 축 충돌 감지 도우미 함수
    void computeRotatedBox();
    bool projectionsOverlap(Entity& ent);
    bool collideCornerCircle(VECTOR2 corner, Entity& ent, VECTOR2& collisionVector);

public:
    // 생성자
    Entity();
    // 소멸자
    virtual ~Entity() {}

    ////////////////////////////////////////
    //           Get 함수            //
    ////////////////////////////////////////

    // 스케일된 엔티티의 중심을 화면 x,y로 반환합니다.
    virtual const VECTOR2* getCenter()
    {
        center = VECTOR2(getCenterX(), getCenterY());
        return &center;
    }

    // 충돌 원의 반지름을 반환합니다.
    virtual float getRadius() const { return radius; }

    // BOX 및 ROTATED_BOX 충돌 감지에 사용되는 RECT 구조체를 반환합니다.
    virtual const RECT& getEdge() const { return edge; }

    // ROTATED_BOX의 모서리 c를 반환합니다.
    virtual const VECTOR2* getCorner(UINT c) const
    {
        if (c >= 4)
            c = 0;
        return &corners[c];
    }

    // 속도 벡터를 반환합니다.
    virtual const VECTOR2 getVelocity() const { return velocity; }

    // active 상태를 반환합니다.
    virtual bool  getActive()         const { return active; }

    // 질량을 반환합니다.
    virtual float getMass()           const { return mass; }

    // 중력 상수를 반환합니다.
    virtual float getGravity()        const { return gravity; }

    // 체력을 반환합니다.
    virtual float getHealth()         const { return health; }

    // 충돌 유형(NONE, CIRCLE, BOX, ROTATED_BOX)을 반환합니다.
    virtual entityNS::COLLISION_TYPE getCollisionType() { return collisionType; }

    ////////////////////////////////////////
    //           Set 함수            //
    ////////////////////////////////////////

    // 속도를 설정합니다.
    virtual void  setVelocity(VECTOR2 v) { velocity = v; }

    // 델타 속도를 설정합니다. update()에서 속도에 추가됩니다.
    virtual void  setDeltaV(VECTOR2 dv) { deltaV = dv; }

    // active 상태를 설정합니다.
    virtual void  setActive(bool a) { active = a; }

    // 체력을 설정합니다.
    virtual void setHealth(float h) { health = h; }

    // 질량을 설정합니다.
    virtual void  setMass(float m) { mass = m; }

    // 중력 상수를 설정합니다. 기본값은 6.67428e-11입니다.
    virtual void  setGravity(float g) { gravity = g; }

    // 충돌 원의 반지름을 설정합니다.
    virtual void setCollisionRadius(float r) { radius = r; }

    // 충돌 유형(NONE, CIRCLE, BOX, ROTATED_BOX)을 설정합니다.
    virtual void setCollisionType(entityNS::COLLISION_TYPE ctype)
    {
        collisionType = ctype;
    }

    // BOX 및 ROTATED_BOX 충돌 감지에 사용되는 RECT 구조체를 설정합니다.
    void setEdge(RECT e) { edge = e; }


    ////////////////////////////////////////
    //         기타 함수            //
    ////////////////////////////////////////

    // 엔티티를 업데이트합니다.
    // 일반적으로 프레임당 한 번 호출됩니다.
    // frameTime은 움직임과 애니메이션의 속도를 조절하는 데 사용됩니다.
    virtual void update(float frameTime);

    // 엔티티 초기화
    // 사전 조건: 
    //          *gamePtr = Game 객체에 대한 포인터
    //          width = 이미지의 너비 (픽셀 단위) (0 = 전체 텍스처 너비 사용)
    //          height = 이미지의 높이 (픽셀 단위) (0 = 전체 텍스처 높이 사용)
    //          ncols = 텍스처의 열 수 (1에서 n까지) (0은 1과 동일)
    //          *textureM = TextureManager 객체에 대한 포인터
    virtual bool initialize(Game* gamePtr, int width, int height, int ncols,
        TextureManager* textureM);
    // 엔티티를 활성화합니다.
    virtual void activate();

    // 엔티티 객체를 인스턴스화할 수 있도록 하는 빈 ai 함수입니다.
    virtual void ai(float frameTime, Entity& ent);

    // 이 엔티티가 지정된 사각형 밖에 있습니까?
    virtual bool outsideRect(RECT rect);

    // 이 엔티티가 ent와 충돌합니까?
    virtual bool collidesWith(Entity& ent, VECTOR2& collisionVector);

    // 무기로 이 엔티티에 피해를 줍니다.
    virtual void damage(int weapon);

    // 다른 엔티티와 충돌 후 엔티티가 튕겨 나옵니다.
    void bounce(VECTOR2& collisionVector, Entity& ent);

    // 이 엔티티의 속도 벡터에 중력을 더합니다.
    void gravityForce(Entity* other, float frameTime);
};

#endif