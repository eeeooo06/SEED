#include "Audio.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ������
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Audio::Audio()
{
    xactEngine = NULL;
    waveBank = NULL;
    soundBank = NULL;
    cueI = 0;
    mapWaveBank = NULL;                                                      // ���� ������ ���� UnmapViewOfFile() ȣ��
    soundBankData = NULL;

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr))
        coInitialized = true;
    else
        coInitialized = false;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �Ҹ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
Audio::~Audio()
{
    // XACT ����
    if (xactEngine)
    {
        xactEngine->ShutDown();                                              // XACT ������ �����ϰ� ���ҽ��� �����մϴ�.
        xactEngine->Release();
    }

    if (soundBankData)
        delete[] soundBankData;
    soundBankData = NULL;

    // xactEngine->ShutDown()�� ��ȯ�� ��, �޸� ���ε� ������ �����մϴ�.
    if (mapWaveBank)
        UnmapViewOfFile(mapWaveBank);
    mapWaveBank = NULL;

    if (coInitialized)                                                       // CoInitializeEx�� ������ ���
        CoUninitialize();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// �ʱ�ȭ
// �� �Լ��� ������ �����մϴ�:
//      1. xactEngine->Initialize�� ȣ���Ͽ� XACT�� �ʱ�ȭ�մϴ�.
//      2. ����Ϸ��� XACT ���̺� ��ũ�� ����ϴ�.
//      3. ����Ϸ��� XACT ���� ��ũ�� ����ϴ�.
//      4. ���ӿ��� ����ϴ� XACT ť�� ���� �ε����� �����մϴ�.
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

    // XACT ��Ÿ�� �ʱ�ȭ �� ����
    XACT_RUNTIME_PARAMETERS xactParams = { 0 };
    xactParams.lookAheadTime = XACT_ENGINE_LOOKAHEAD_DEFAULT;
    result = xactEngine->Initialize(&xactParams);
    if (FAILED(result))
        return result;

    // �޸� ���� ���� IO�� ����Ͽ� "�޸� ��" XACT ���̺� ��ũ ���� ����
    result = E_FAIL;                                                         // �⺻������ ���� �ڵ�� �����ϰ� ���� �� ��ü
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

                CloseHandle(hMapFile);                                       // mapWaveBank�� ���Ͽ� ���� �ڵ��� �����ϹǷ� �� ���ʿ��� �ڵ��� �ݽ��ϴ�.
            }
        }
        CloseHandle(hFile);                                                  // mapWaveBank�� ���Ͽ� ���� �ڵ��� �����ϹǷ� �� ���ʿ��� �ڵ��� �ݽ��ϴ�.
    }
    if (FAILED(result))
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

    // ���� ��ũ ������ �а� XACT�� ����մϴ�.
    result = E_FAIL;                                                         // �⺻������ ���� �ڵ�� �����ϰ� ���� �� ��ü
    hFile = CreateFile(SOUND_BANK, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        fileSize = GetFileSize(hFile, NULL);
        if (fileSize != -1)
        {
            soundBankData = new BYTE[fileSize];                              // ���� ��ũ�� ���� �޸� ����
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
// �ֱ����� ���� ���� �۾� ����
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Audio::run()
{
    if (xactEngine == NULL)
        return;
    xactEngine->DoWork();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ���� ��ũ���� ť�� ������ ���� ���
// ť�� �������� �ʾƵ� ������ �߻����� ������, �ܼ��� ���尡 ������� ����
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Audio::playCue(const char cue[])
{
    if (soundBank == NULL)
        return;
    cueI = soundBank->GetCueIndex(cue);                                      // ���� ��ũ���� ť �ε��� ��������
    soundBank->Play(cueI, 0, 0, NULL);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ���� ��ũ���� ť�� ������ ��� ���� ���� ����
// ť�� �������� �ʾƵ� ������ �߻����� ����
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void Audio::stopCue(const char cue[])
{
    if (soundBank == NULL)
        return;
    cueI = soundBank->GetCueIndex(cue);                                      // ���� ��ũ���� ť �ε��� ��������
    soundBank->Stop(cueI, XACT_FLAG_SOUNDBANK_STOP_IMMEDIATE);
}