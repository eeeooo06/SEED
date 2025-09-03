#ifndef _TEXTUREMANAGER_H                                                    // 이 파일이 여러 위치에 포함될 경우
#define _TEXTUREMANAGER_H                                                    // 중복 정의 방지
#define WIN32_LEAN_AND_MEAN                                                  

#include "Graphics.h"                                                        
#include "Constants.h"                                                       

class TextureManager
{
    // TextureManager 속성                                                    
private:
    UINT       width;                                                        // 텍스처의 너비 (픽셀 단위)
    UINT       height;                                                       // 텍스처의 높이 (픽셀 단위)
    LP_TEXTURE texture;                                                      // 텍스처 포인터
    const char* file;                                                        // 파일 이름
    Graphics* graphics;                                                      // 그래픽 객체 포인터 저장
    bool    initialized;                                                     // 성공적으로 초기화되면 true
    HRESULT hr;                                                              // 표준 반환 타입

public:
    // 생성자
    TextureManager();

    // 소멸자
    virtual ~TextureManager();

    // 텍스처에 대한 포인터를 반환합니다
    LP_TEXTURE getTexture() const { return texture; }

    // 텍스처 너비를 반환합니다
    UINT getWidth() const { return width; }

    // 텍스처 높이를 반환합니다
    UINT getHeight() const { return height; }

    // textureManager를 초기화합니다
    // 사전조건: 
    //          *g는 Graphics 객체를 가리킵니다
    //          *file은 로드할 텍스처 파일의 이름을 가리킵니다
    // 사후조건: 
    //          텍스처 파일이 로드됩니다
    virtual bool initialize(Graphics* g, const char* file);

    // 리소스 해제
    virtual void onLostDevice();

    // 리소스 복원
    virtual void onResetDevice();
};

#endif