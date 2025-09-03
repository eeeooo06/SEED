// 2D Game Engine
// Copyright (c) 2025 by
// eeeooo
// Game Engine version 1.0

#pragma once

#ifndef _GAME_H																// ���� ������ �� ������ �����ϴ� ���
#define _GAME_H																// ���� ���Ǹ� �����Ѵ�.
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Mmsystem.h>

#include "Audio.h"
#include "Console.h"
#include "Constants.h"
#include "GameError.h"
#include "Graphics.h"
#include "Input.h"
#include "InputDialog.h"
#include "MessageDialog.h"
#include "TextDX.h"

namespace GameNS
{
	const char FONT[] = "Courier New";										// ��Ʈ
	const int FONT_SIZE = 14;												// ��Ʈ ũ��
	const COLOR_ARGB FONT_COLOR = SETCOLOR_ARGB(255, 255, 255, 255);		// ��Ʈ ���� (���)
}
class Game
{
protected:
	// ���� ���� �Ӽ�
	Audio* audio;															// Audio ��ü�� ����Ű�� ������
	Console* console;														// Console ��ü�� ����Ű�� ������
	GameError* gameError;													// GameError ��ü�� ����Ű�� ������
	Graphics* graphics;														// Graphics ��ü�� ����Ű�� ������
	Input* input;															// Input ��ü�� ����Ű�� ������
	InputDialog* inputDialog;												// InputDialog ��ü�� ����Ű�� ������
	MessageDialog* messageDialog;

	// ���� ���� �Ӽ�
	HWND hwnd;																// ������ �ڵ�
	HRESULT hr;																// �Ϲ����� ��ȯ ��

	LARGE_INTEGER timeStart;												// ���� ī���� ���� ��
	LARGE_INTEGER timeEnd;													// ���� ī���� ���� ��
	LARGE_INTEGER timerFreq;												// ���� ī���� �󵵼�
	float frameTime;														// ������ �����ӿ� �ʿ��� �ð�
	float fps;																// �ʴ� ������ ��
	bool fpsOn;																// fps ǥ�� ����
	DWORD sleepTime;														// ������ ���̿� ��ٸ� �и��� ���� �ð�
	bool paused;															// �Ͻ� ���� ����, ������ �Ͻ� ������ ��� true
	bool initialized;														// ������ �ʱ�ȭ�Ǿ����� ����

	TextDX dxFont;															// TextDX ��ü
	std::string command;													// �ܼ� ���ɾ�


public:
	// ������
	Game();
	// �Ҹ���
	virtual ~Game();

	/* ��� �Լ� */

	// ������ �޽��� �ڵ鷯
	LRESULT messageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	// ���� �ʱ�ȭ
	virtual void initialize(HWND hwnd);
	// ���� ���� ����
	virtual void run(HWND hwnd);
	// �׷��� ��ġ�� �սǵǾ��� �� ȣ��˴ϴ�. �׷��� ��ġ�� �缳���� �� �ֵ��� ��� ����� ���� �޸𸮸� ����
	virtual void releaseAll();
	// ��� ǥ���� �ٽ� �����ϰ� ��� ��ƼƼ�� �ʱ�ȭ
	virtual void resetAll();
	// ��� ����� �޸𸮸� ����
	virtual void deleteAll();
	// �ܼ� ������ ó��
	virtual void consoleCommand();
	// ��Ʈ��ũ ����� ����
	//virtual void communicate(float frameTime);
	// ���� �������� ������
	virtual void renderGame();
	// �սǵ� �׷��� ��ġ�� ó��
	virtual void handleLostGraphicsDevice();
	// ���÷��� ��带 ����(��ü ȭ��, â ���, �Ǵ� ���)
	void setDisplayMode(GraphicsNS::DISPLAY_MODE mode = GraphicsNS::TOGGLE);

	// ������� ���� �����͸� ��ȯ
	Audio* getAudio() { return audio; }
	// �׷��ȿ� ���� �����͸� ��ȯ
	Graphics* getGraphics() { return graphics; }
	// �Է¿� ���� �����͸� ��ȯ
	Input* getInput() { return input; }

	// ������ ����
	    void exitgame() { PostMessage(hwnd, WM_DESTROY, 0, 0); }

    // 'paused' 변수에 접근하기 위한 함수 (Accessor Functions)
    void setPaused(bool p) { paused = p; }
    bool getPaused() { return paused; }


	/* ���� ���� �Լ� */

	// ���� �������� ������Ʈ
	virtual void update() = 0;
	// AI ����� ����
	virtual void ai() = 0;
	// �浹�� ó��
	virtual void collisions() = 0;
	// �׷����� ������
	virtual void render() = 0;
};
#endif
