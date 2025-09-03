#include "Audio.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 생성자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Audio::Audio()
{
    xactEngine = NULL;
    waveBank = NULL;
    soundBank = NULL;
    cueI = 0;
    mapWaveBank = NULL;                                                      // 파일 해제를 위해 UnmapViewOfFile() 호출
    soundBankData = NULL;

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr))
        coInitialized = true;
    else
        coInitialized = false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 소멸자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Audio::~Audio()
{
    // XACT 종료
    if (xactEngine)
    {
        xactEngine->ShutDown();                                              // XACT 엔진을 종료하고 리소스를 해제합니다.
        xactEngine->Release();
    }

    if (soundBankData)
        delete[] soundBankData;
    soundBankData = NULL;

    // xactEngine->ShutDown()이 반환된 후, 메모리 매핑된 파일을 해제합니다.
    if (mapWaveBank)
        UnmapViewOfFile(mapWaveBank);
    mapWaveBank = NULL;

    if (coInitialized)                                                       // CoInitializeEx가 성공한 경우
        CoUninitialize();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 초기화
// 이 함수는 다음을 수행합니다:
//      1. xactEngine->Initialize를 호출하여 XACT를 초기화합니다.
//      2. 사용하려는 XACT 웨이브 뱅크를 만듭니다.
//      3. 사용하려는 XACT 사운드 뱅크를 만듭니다.
//      4. 게임에서 사용하는 XACT 큐에 대한 인덱스를 저장합니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
HRESULT Audio::initialize()
{
    HRESULT result = E_FAIL;
    HANDLE hFile;
    DWORD fileSize;
    DWORD bytesRead;
    HANDLE hMapFile;

    if (coInitialized == false)
        return E_FAIL;

    result = XACT3CreateEngine(0, &xactEngine);
    if (FAILED(result) || xactEngine == NULL)
        return E_FAIL;

    // XACT 런타임 초기화 및 생성
    XACT_RUNTIME_PARAMETERS xactParams = { 0 };
    xactParams.lookAheadTime = XACT_ENGINE_LOOKAHEAD_DEFAULT;
    result = xactEngine->Initialize(&xactParams);
    if (FAILED(result))
        return result;

    // 메모리 매핑 파일 IO를 사용하여 "메모리 내" XACT 웨이브 뱅크 파일 생성
    result = E_FAIL;                                                         // 기본적으로 실패 코드로 설정하고 성공 시 교체
    hFile = CreateFile(WAVE_BANK, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        fileSize = GetFileSize(hFile, NULL);
        if (fileSize != -1)
        {
            hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
            if (hMapFile)
            {
                mapWaveBank = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
                if (mapWaveBank)
                    result = xactEngine->CreateInMemoryWaveBank(mapWaveBank, fileSize, 0, 0, &waveBank);

                CloseHandle(hMapFile);                                       // mapWaveBank가 파일에 대한 핸들을 유지하므로 이 불필요한 핸들을 닫습니다.
            }
        }
        CloseHandle(hFile);                                                  // mapWaveBank가 파일에 대한 핸들을 유지하므로 이 불필요한 핸들을 닫습니다.
    }
    if (FAILED(result))
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

    // 사운드 뱅크 파일을 읽고 XACT에 등록합니다.
    result = E_FAIL;                                                         // 기본적으로 실패 코드로 설정하고 성공 시 교체
    hFile = CreateFile(SOUND_BANK, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        fileSize = GetFileSize(hFile, NULL);
        if (fileSize != -1)
        {
            soundBankData = new BYTE[fileSize];                              // 사운드 뱅크를 위한 메모리 예약
            if (soundBankData)
            {
                if (0 != ReadFile(hFile, soundBankData, fileSize, &bytesRead, NULL))
                    result = xactEngine->CreateSoundBank(soundBankData, fileSize, 0, 0, &soundBank);
            }
        }
        CloseHandle(hFile);
    }
    if (FAILED(result))
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

    return S_OK;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 주기적인 사운드 엔진 작업 수행
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Audio::run()
{
    if (xactEngine == NULL)
        return;
    xactEngine->DoWork();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 사운드 뱅크에서 큐로 지정된 사운드 재생
// 큐가 존재하지 않아도 오류가 발생하지 않으며, 단순히 사운드가 재생되지 않음
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Audio::playCue(const char cue[])
{
    if (soundBank == NULL)
        return;
    cueI = soundBank->GetCueIndex(cue);                                      // 사운드 뱅크에서 큐 인덱스 가져오기
    soundBank->Play(cueI, 0, 0, NULL);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 사운드 뱅크에서 큐로 지정된 재생 중인 사운드 중지
// 큐가 존재하지 않아도 오류가 발생하지 않음
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Audio::stopCue(const char cue[])
{
    if (soundBank == NULL)
        return;
    cueI = soundBank->GetCueIndex(cue);                                      // 사운드 뱅크에서 큐 인덱스 가져오기
    soundBank->Stop(cueI, XACT_FLAG_SOUNDBANK_STOP_IMMEDIATE);
}