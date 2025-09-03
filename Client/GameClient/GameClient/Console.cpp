#include "Console.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 생성자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
Console::Console()
{
    initialized = false;                                                     // 성공적으로 초기화되면 true로 설정
    graphics = NULL;
    visible = false;                                                         // 보이지 않음
    fontColor = consoleNS::FONT_COLOR;
    backColor = consoleNS::BACK_COLOR;

    x = consoleNS::X;                                                        // 콘솔 시작 위치
    y = consoleNS::Y;

    textRect.bottom = consoleNS::Y + consoleNS::HEIGHT - consoleNS::MARGIN;
    textRect.left = consoleNS::X + consoleNS::MARGIN;
    textRect.right = consoleNS::X + consoleNS::WIDTH - consoleNS::MARGIN;
    textRect.top = consoleNS::Y + consoleNS::MARGIN;

    vertexBuffer = NULL;

    rows = 0;
    scrollAmount = 0;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 소멸자
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
Console::~Console()
{
    onLostDevice();                                                          // 모든 그래픽 항목에 대해 onLostDevice() 호출
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 콘솔 초기화
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
bool Console::initialize(Graphics* g, Input* in)
{
    try {
        graphics = g;                                                        // 그래픽 시스템
        input = in;

        // 왼쪽 상단
        vtx[0].x = x;
        vtx[0].y = y;
        vtx[0].z = 0.0f;
        vtx[0].rhw = 1.0f;
        vtx[0].color = backColor;

        // 오른쪽 상단
        vtx[1].x = x + consoleNS::WIDTH;
        vtx[1].y = y;
        vtx[1].z = 0.0f;
        vtx[1].rhw = 1.0f;
        vtx[1].color = backColor;

        // 오른쪽 하단
        vtx[2].x = x + consoleNS::WIDTH;
        vtx[2].y = y + consoleNS::HEIGHT;
        vtx[2].z = 0.0f;
        vtx[2].rhw = 1.0f;
        vtx[2].color = backColor;

        // 왼쪽 하단
        vtx[3].x = x;
        vtx[3].y = y + consoleNS::HEIGHT;
        vtx[3].z = 0.0f;
        vtx[3].rhw = 1.0f;
        vtx[3].color = backColor;

        graphics->createVertexBuffer(vtx, sizeof vtx, vertexBuffer);

        // DirectX 글꼴 초기화
        if (dxFont.initialize(graphics, consoleNS::FONT_HEIGHT, false,
            false, consoleNS::FONT) == false)
            return false;                                                    // 실패한 경우
        dxFont.setFontColor(fontColor);

    }
    catch (...) {
        return false;
    }

    initialized = true;
    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 콘솔 그리기
// 사전 조건: 
//          BeginScene/EndScene 내부
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
const void Console::draw()
{
    if (!visible || graphics == NULL || !initialized)
        return;

    graphics->drawQuad(vertexBuffer);                                        // 배경 그리기
    if (text.size() == 0)
        return;

    graphics->spriteBegin();                                                 // 스프라이트 그리기 시작

    // 콘솔에 텍스트 표시
    textRect.left = 0;
    textRect.top = 0;

    // textRect 하단을 1줄 높이로 설정
    dxFont.print("|", textRect, DT_CALCRECT);
    int rowHeight = textRect.bottom + 2;                                     // 1줄 높이 (+2는 줄 간격)
    if (rowHeight <= 0)                                                      // 이것은 절대 true가 되어서는 안 됩니다.
        rowHeight = 20;                                                      // 작동 가능한 결과 강제

    // 콘솔에 들어갈 수 있는 줄 수
    rows = (consoleNS::HEIGHT - 2 * consoleNS::MARGIN) / rowHeight;
    rows -= 2;                                                               // 하단에 입력 프롬프트를 위한 공간
    if (rows <= 0)                                                           // 이것은 절대 true가 되어서는 안 됩니다.
        rows = 5;                                                            // 작동 가능한 결과 강제

    // 한 줄에 대한 텍스트 표시 사각형 설정
    textRect.left = (long)(x + consoleNS::MARGIN);
    textRect.right = (long)(textRect.right + consoleNS::WIDTH - consoleNS::MARGIN);
    // -2*rowHeight는 입력 프롬프트를 위한 공간
    textRect.bottom = (long)(y + consoleNS::HEIGHT - 2 * consoleNS::MARGIN - 2 * rowHeight);
    // 모든 줄에 대해 (최대 text.size()) 아래에서 위로
    for (int r = scrollAmount; r < rows + scrollAmount && r < (int)(text.size()); r++)
    {
        // 이 줄에 대한 텍스트 표시 사각형 상단 설정
        textRect.top = textRect.bottom - rowHeight;
        // 한 줄의 텍스트 표시
        dxFont.print(text[r], textRect, DT_LEFT);
        // 다음 줄에 대한 텍스트 표시 사각형 하단 조정
        textRect.bottom -= rowHeight;
    }

    // 명령어 프롬프트와 현재 명령어 문자열 표시
    // 프롬프트에 대한 텍스트 표시 사각형 설정
    textRect.bottom = (long)(y + consoleNS::HEIGHT - consoleNS::MARGIN);
    textRect.top = textRect.bottom - rowHeight;
    std::string prompt = "> ";                                               // 프롬프트 문자열 빌드
    prompt += input->getTextIn();
    dxFont.print(prompt, textRect, DT_LEFT);                                 // 프롬프트와 명령어 표시

    graphics->spriteEnd();                                                   // 스프라이트 그리기 종료
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 콘솔 보이기/숨기기
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Console::showHide()
{
    if (!initialized)
        return;
    visible = !visible;
    input->clear(inputNS::KEYS_PRESSED | inputNS::TEXT_IN);                  // 이전 입력 지우기
}


// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 콘솔에 텍스트 추가
// str의 첫 번째 줄만 표시됩니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Console::print(const std::string& str)                                  // 콘솔에 텍스트 추가
{
    if (!initialized)
        return;
    text.push_front(str);                                                    // 텍스트 데크에 str 추가
    if (text.size() > consoleNS::MAX_LINES)
        text.pop_back();                                                     // 가장 오래된 줄 삭제
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 콘솔 명령어 반환
// 콘솔 단일 키 명령어를 처리합니다.
// 다른 모든 명령어는 게임으로 반환합니다.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
std::string Console::getCommand()
{
    // 콘솔이 초기화되지 않았거나 보이지 않는 경우
    if (!initialized || !visible)
        return "";

    // 콘솔 키 확인
    if (input->wasKeyPressed(CONSOLE_KEY))
        hide();                                                              // 콘솔 끄기

    // Esc 키 확인                                                            
    if (input->wasKeyPressed(ESC_KEY))
        return "";

    // 스크롤 확인                                                            
    if (input->wasKeyPressed(VK_UP))                                         // 위쪽 화살표인 경우
        scrollAmount++;
    else if (input->wasKeyPressed(VK_DOWN))                                  // 아래쪽 화살표인 경우
        scrollAmount--;
    else if (input->wasKeyPressed(VK_PRIOR))                                 // Page Up 키인 경우
        scrollAmount += rows;
    else if (input->wasKeyPressed(VK_NEXT))                                  // Page Down 키인 경우
        scrollAmount -= rows;
    if (scrollAmount < 0)
        scrollAmount = 0;
    if (scrollAmount > consoleNS::MAX_LINES - 1)
        scrollAmount = consoleNS::MAX_LINES - 1;
    if (scrollAmount > (int)(text.size()) - 1)
        scrollAmount = (int)(text.size()) - 1;

    commandStr = input->getTextIn();                                         // 사용자가 입력한 텍스트 가져오기
    // 키를 게임으로 전달하지 않음
    input->clear(inputNS::KEYS_DOWN | inputNS::KEYS_PRESSED | inputNS::MOUSE);

    if (commandStr.length() == 0)                                            // 입력된 명령어가 없는 경우
        return "";
    if (commandStr.at(commandStr.length() - 1) != '\r')                      // 'Enter' 키가 눌리지 않은 경우
        return "";                                                           // 반환, 명령어가 될 수 없음

    commandStr.erase(commandStr.length() - 1);                               // 명령어 문자열 끝에서 '\r' 삭제
    input->clearTextIn();                                                    // 입력 줄 지우기
    inputStr = commandStr;                                                   // 입력 텍스트 저장
    return commandStr;                                                       // 명령어 반환
}


// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 그래픽 장치를 잃어버렸을 때 호출됨
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Console::onLostDevice()
{
    if (!initialized)
        return;
    dxFont.onLostDevice();
    SAFE_RELEASE(vertexBuffer);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// 그래픽 장치가 재설정될 때 호출됨
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Console::onResetDevice()
{
    if (!initialized)
        return;
    graphics->createVertexBuffer(vtx, sizeof vtx, vertexBuffer);
    dxFont.onResetDevice();
}
