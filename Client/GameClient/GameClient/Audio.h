#ifndef _AUDIO_H                                                             // 이 파일이 여러 위치에 포함될 경우
#define _AUDIO_H                                                             // 중복 정의 방지
#define WIN32_LEAN_AND_MEAN                                                  

#include <xact3.h>                                                           
#include "Constants.h"                                                       

class Audio
{
    // 속성                                                                   
private:
    IXACT3Engine* xactEngine;                                                // XACT 사운드 엔진 포인터
    IXACT3WaveBank* waveBank;                                                // XACT 웨이브 뱅크 포인터
    IXACT3SoundBank* soundBank;                                              // XACT 사운드 뱅크 포인터
    XACTINDEX cueI;                                                          // XACT 사운드 인덱스
    void* mapWaveBank;                                                       // 파일 해제를 위해 UnmapViewOfFile() 호출
    void* soundBankData;
    bool coInitialized;                                                      // coInitialize가 성공하면 true로 설정

public:
    // 생성자
    Audio();

    // 소멸자
    virtual ~Audio();

    // 멤버 함수

    // 오디오 초기화
    HRESULT initialize();

    // 주기적인 사운드 엔진 작업을 수행합니다.
    void run();

    // 사운드 뱅크에서 큐로 지정된 사운드를 재생합니다.
    // 큐가 존재하지 않아도 오류가 발생하지 않으며, 단순히 사운드가 재생되지 않습니다.
    void playCue(const char cue[]);

    // 사운드 뱅크에서 큐로 지정된 재생 중인 사운드를 중지합니다.
    // 큐가 존재하지 않아도 오류가 발생하지 않습니다.
    void stopCue(const char cue[]);
};

#endif