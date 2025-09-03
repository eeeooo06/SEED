#ifndef _AUDIO_H                                                             // �� ������ ���� ��ġ�� ���Ե� ���
#define _AUDIO_H                                                             // �ߺ� ���� ����
#define WIN32_LEAN_AND_MEAN                                                  

#include <xact3.h>                                                           
#include "Constants.h"                                                       

class Audio
{
    // �Ӽ�                                                                   
private:
    IXACT3Engine* xactEngine;                                                // XACT ���� ���� ������
    IXACT3WaveBank* waveBank;                                                // XACT ���̺� ��ũ ������
    IXACT3SoundBank* soundBank;                                              // XACT ���� ��ũ ������
    XACTINDEX cueI;                                                          // XACT ���� �ε���
    void* mapWaveBank;                                                       // ���� ������ ���� UnmapViewOfFile() ȣ��
    void* soundBankData;
    bool coInitialized;                                                      // coInitialize�� �����ϸ� true�� ����

public:
    // ������
    Audio();

    // �Ҹ���
    virtual ~Audio();

    // ��� �Լ�

    // ����� �ʱ�ȭ
    HRESULT initialize();

    // �ֱ����� ���� ���� �۾��� �����մϴ�.
    void run();

    // ���� ��ũ���� ť�� ������ ���带 ����մϴ�.
    // ť�� �������� �ʾƵ� ������ �߻����� ������, �ܼ��� ���尡 ������� �ʽ��ϴ�.
    void playCue(const char cue[]);

    // ���� ��ũ���� ť�� ������ ��� ���� ���带 �����մϴ�.
    // ť�� �������� �ʾƵ� ������ �߻����� �ʽ��ϴ�.
    void stopCue(const char cue[]);
};

#endif