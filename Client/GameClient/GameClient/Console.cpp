#include "Console.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// ������
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
Console::Console()
{
    initialized = false;                                                     // ���������� �ʱ�ȭ�Ǹ� true�� ����
    graphics = NULL;
    visible = false;                                                         // ������ ����
    fontColor = consoleNS::FONT_COLOR;
    backColor = consoleNS::BACK_COLOR;

    x = consoleNS::X;                                                        // �ܼ� ���� ��ġ
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
// �Ҹ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
Console::~Console()
{
    onLostDevice();                                                          // ��� �׷��� �׸� ���� onLostDevice() ȣ��
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// �ܼ� �ʱ�ȭ
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
bool Console::initialize(Graphics* g, Input* in)
{
    try {
        graphics = g;                                                        // �׷��� �ý���
        input = in;

        // ���� ���
        vtx[0].x = x;
        vtx[0].y = y;
        vtx[0].z = 0.0f;
        vtx[0].rhw = 1.0f;
        vtx[0].color = backColor;

        // ������ ���
        vtx[1].x = x + consoleNS::WIDTH;
        vtx[1].y = y;
        vtx[1].z = 0.0f;
        vtx[1].rhw = 1.0f;
        vtx[1].color = backColor;

        // ������ �ϴ�
        vtx[2].x = x + consoleNS::WIDTH;
        vtx[2].y = y + consoleNS::HEIGHT;
        vtx[2].z = 0.0f;
        vtx[2].rhw = 1.0f;
        vtx[2].color = backColor;

        // ���� �ϴ�
        vtx[3].x = x;
        vtx[3].y = y + consoleNS::HEIGHT;
        vtx[3].z = 0.0f;
        vtx[3].rhw = 1.0f;
        vtx[3].color = backColor;

        graphics->createVertexBuffer(vtx, sizeof vtx, vertexBuffer);

        // DirectX �۲� �ʱ�ȭ
        if (dxFont.initialize(graphics, consoleNS::FONT_HEIGHT, false,
            false, consoleNS::FONT) == false)
            return false;                                                    // ������ ���
        dxFont.setFontColor(fontColor);

    }
    catch (...) {
        return false;
    }

    initialized = true;
    return true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// �ܼ� �׸���
// ���� ����: 
//          BeginScene/EndScene ����
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
const void Console::draw()
{
    if (!visible || graphics == NULL || !initialized)
        return;

    graphics->drawQuad(vertexBuffer);                                        // ��� �׸���
    if (text.size() == 0)
        return;

    graphics->spriteBegin();                                                 // ��������Ʈ �׸��� ����

    // �ֿܼ� �ؽ�Ʈ ǥ��
    textRect.left = 0;
    textRect.top = 0;

    // textRect �ϴ��� 1�� ���̷� ����
    dxFont.print("|", textRect, DT_CALCRECT);
    int rowHeight = textRect.bottom + 2;                                     // 1�� ���� (+2�� �� ����)
    if (rowHeight <= 0)                                                      // �̰��� ���� true�� �Ǿ�� �� �˴ϴ�.
        rowHeight = 20;                                                      // �۵� ������ ��� ����

    // �ֿܼ� �� �� �ִ� �� ��
    rows = (consoleNS::HEIGHT - 2 * consoleNS::MARGIN) / rowHeight;
    rows -= 2;                                                               // �ϴܿ� �Է� ������Ʈ�� ���� ����
    if (rows <= 0)                                                           // �̰��� ���� true�� �Ǿ�� �� �˴ϴ�.
        rows = 5;                                                            // �۵� ������ ��� ����

    // �� �ٿ� ���� �ؽ�Ʈ ǥ�� �簢�� ����
    textRect.left = (long)(x + consoleNS::MARGIN);
    textRect.right = (long)(textRect.right + consoleNS::WIDTH - consoleNS::MARGIN);
    // -2*rowHeight�� �Է� ������Ʈ�� ���� ����
    textRect.bottom = (long)(y + consoleNS::HEIGHT - 2 * consoleNS::MARGIN - 2 * rowHeight);
    // ��� �ٿ� ���� (�ִ� text.size()) �Ʒ����� ����
    for (int r = scrollAmount; r < rows + scrollAmount && r < (int)(text.size()); r++)
    {
        // �� �ٿ� ���� �ؽ�Ʈ ǥ�� �簢�� ��� ����
        textRect.top = textRect.bottom - rowHeight;
        // �� ���� �ؽ�Ʈ ǥ��
        dxFont.print(text[r], textRect, DT_LEFT);
        // ���� �ٿ� ���� �ؽ�Ʈ ǥ�� �簢�� �ϴ� ����
        textRect.bottom -= rowHeight;
    }

    // ��ɾ� ������Ʈ�� ���� ��ɾ� ���ڿ� ǥ��
    // ������Ʈ�� ���� �ؽ�Ʈ ǥ�� �簢�� ����
    textRect.bottom = (long)(y + consoleNS::HEIGHT - consoleNS::MARGIN);
    textRect.top = textRect.bottom - rowHeight;
    std::string prompt = "> ";                                               // ������Ʈ ���ڿ� ����
    prompt += input->getTextIn();
    dxFont.print(prompt, textRect, DT_LEFT);                                 // ������Ʈ�� ��ɾ� ǥ��

    graphics->spriteEnd();                                                   // ��������Ʈ �׸��� ����
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// �ܼ� ���̱�/�����
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Console::showHide()
{
    if (!initialized)
        return;
    visible = !visible;
    input->clear(inputNS::KEYS_PRESSED | inputNS::TEXT_IN);                  // ���� �Է� �����
}


// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// �ֿܼ� �ؽ�Ʈ �߰�
// str�� ù ��° �ٸ� ǥ�õ˴ϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Console::print(const std::string& str)                                  // �ֿܼ� �ؽ�Ʈ �߰�
{
    if (!initialized)
        return;
    text.push_front(str);                                                    // �ؽ�Ʈ ��ũ�� str �߰�
    if (text.size() > consoleNS::MAX_LINES)
        text.pop_back();                                                     // ���� ������ �� ����
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// �ܼ� ��ɾ� ��ȯ
// �ܼ� ���� Ű ��ɾ ó���մϴ�.
// �ٸ� ��� ��ɾ�� �������� ��ȯ�մϴ�.
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
std::string Console::getCommand()
{
    // �ܼ��� �ʱ�ȭ���� �ʾҰų� ������ �ʴ� ���
    if (!initialized || !visible)
        return "";

    // �ܼ� Ű Ȯ��
    if (input->wasKeyPressed(CONSOLE_KEY))
        hide();                                                              // �ܼ� ����

    // Esc Ű Ȯ��                                                            
    if (input->wasKeyPressed(ESC_KEY))
        return "";

    // ��ũ�� Ȯ��                                                            
    if (input->wasKeyPressed(VK_UP))                                         // ���� ȭ��ǥ�� ���
        scrollAmount++;
    else if (input->wasKeyPressed(VK_DOWN))                                  // �Ʒ��� ȭ��ǥ�� ���
        scrollAmount--;
    else if (input->wasKeyPressed(VK_PRIOR))                                 // Page Up Ű�� ���
        scrollAmount += rows;
    else if (input->wasKeyPressed(VK_NEXT))                                  // Page Down Ű�� ���
        scrollAmount -= rows;
    if (scrollAmount < 0)
        scrollAmount = 0;
    if (scrollAmount > consoleNS::MAX_LINES - 1)
        scrollAmount = consoleNS::MAX_LINES - 1;
    if (scrollAmount > (int)(text.size()) - 1)
        scrollAmount = (int)(text.size()) - 1;

    commandStr = input->getTextIn();                                         // ����ڰ� �Է��� �ؽ�Ʈ ��������
    // Ű�� �������� �������� ����
    input->clear(inputNS::KEYS_DOWN | inputNS::KEYS_PRESSED | inputNS::MOUSE);

    if (commandStr.length() == 0)                                            // �Էµ� ��ɾ ���� ���
        return "";
    if (commandStr.at(commandStr.length() - 1) != '\r')                      // 'Enter' Ű�� ������ ���� ���
        return "";                                                           // ��ȯ, ��ɾ �� �� ����

    commandStr.erase(commandStr.length() - 1);                               // ��ɾ� ���ڿ� ������ '\r' ����
    input->clearTextIn();                                                    // �Է� �� �����
    inputStr = commandStr;                                                   // �Է� �ؽ�Ʈ ����
    return commandStr;                                                       // ��ɾ� ��ȯ
}


// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// �׷��� ��ġ�� �Ҿ������ �� ȣ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Console::onLostDevice()
{
    if (!initialized)
        return;
    dxFont.onLostDevice();
    SAFE_RELEASE(vertexBuffer);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
// �׷��� ��ġ�� �缳���� �� ȣ���
// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====      
void Console::onResetDevice()
{
    if (!initialized)
        return;
    graphics->createVertexBuffer(vtx, sizeof vtx, vertexBuffer);
    dxFont.onResetDevice();
}
